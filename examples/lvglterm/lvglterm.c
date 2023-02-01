/****************************************************************************
 * apps/examples/lvglterm/lvglterm.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <sys/boardctl.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <debug.h>
#include <poll.h>
#include <lvgl/lvgl.h>
#include <port/lv_port.h>
#include "nshlib/nshlib.h"

static void create_terminal(void);

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Should we perform board-specific driver initialization?  There are two
 * ways that board initialization can occur:  1) automatically via
 * board_late_initialize() during bootupif CONFIG_BOARD_LATE_INITIALIZE
 * or 2).
 * via a call to boardctl() if the interface is enabled
 * (CONFIG_BOARDCTL=y).
 * If this task is running as an NSH built-in application, then that
 * initialization has probably already been performed otherwise we do it
 * here.
 */

#undef NEED_BOARDINIT

#if defined(CONFIG_BOARDCTL) && !defined(CONFIG_NSH_ARCHINIT)
#  define NEED_BOARDINIT 1
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main or lvglterm_main
 *
 * Description:
 *
 * Input Parameters:
 *   Standard argc and argv
 *
 * Returned Value:
 *   Zero on success; a positive, non-zero value on failure.
 *
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
#ifdef NEED_BOARDINIT
  /* Perform board-specific driver initialization */

  boardctl(BOARDIOC_INIT, 0);

#ifdef CONFIG_BOARDCTL_FINALINIT
  /* Perform architecture-specific final-initialization (if configured) */

  boardctl(BOARDIOC_FINALINIT, 0);
#endif
#endif

  /* LVGL initialization */

  lv_init();

  /* LVGL port initialization */

  lv_port_init();

  /* Create an LVGL Terminal that will let us interact with NuttX NSH Shell */

  create_terminal();

  /* Handle LVGL tasks */

  while (1)
    {
      uint32_t idle;
      idle = lv_timer_handler();

      /* Minimum sleep of 1ms */

      idle = idle ? idle : 1;
      usleep(idle * 1000);
    }

  return EXIT_SUCCESS;
}

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifndef CONFIG_DEV_PIPE_SIZE
#error "Device Drivers > FIFO and named pipe drivers" should be enabled in menuconfig
#endif

static bool has_input(int fd);
static void timer_callback(lv_timer_t * timer);
static void create_widgets(void);
static void input_callback(lv_event_t * e);
static void remove_escape_codes(char *buf, int len);

// Pipes for NSH Shell: : stdin, stdout, stderr
static int nsh_stdin[2];
static int nsh_stdout[2];
static int nsh_stderr[2];

#define READ_PIPE  0  // Read Pipes: stdin, stdout, stderr
#define WRITE_PIPE 1  // Write Pipes: stdin, stdout, stderr

// LVGL Container with Column Flex Direction
static lv_obj_t *col;

// LVGL Text Area Widgets for NSH Input and Output
static lv_obj_t *input;
static lv_obj_t *output;

// LVGL Keyboard Widget
static lv_obj_t *kb;

// Font Style for NSH Input and Output
static lv_style_t terminal_style;

// Create an LVGL Terminal that will let us interact with NuttX NSH Shell
static void create_terminal(void) {
  _info("create_terminal\n");

  // Create the pipes for NSH Shell
  int ret;
  ret = pipe(nsh_stdin);  if (ret < 0) { _err("stdin pipe failed: %d\n", errno);  return; }
  ret = pipe(nsh_stdout); if (ret < 0) { _err("stdout pipe failed: %d\n", errno); return; }
  ret = pipe(nsh_stderr); if (ret < 0) { _err("stderr pipe failed: %d\n", errno); return; }

  // Close default stdin, stdout and stderr
  close(0);
  close(1);
  close(2);

  // Use the pipes as NSH stdin, stdout and stderr
  dup2(nsh_stdin[READ_PIPE], 0);
  dup2(nsh_stdout[WRITE_PIPE], 1);
  dup2(nsh_stderr[WRITE_PIPE], 2);

  // Create a new NSH Console using the pipes
  char *argv[] = { NULL };
  pid_t pid = task_create(
    "NSH Console",
    CONFIG_EXAMPLES_LVGLTERM_PRIORITY,
    CONFIG_EXAMPLES_LVGLTERM_STACKSIZE,
    nsh_consolemain,
    argv
  );
  if (pid < 0) { _err("task_create failed: %d\n", errno); return; }
  _info("pid=%d\n", pid);

  // Create an LVGL Timer to poll for output from NSH Shell
  // Based on https://docs.lvgl.io/master/overview/timer.html#create-a-timer
  static uint32_t user_data = 10;
  lv_timer_t *timer = lv_timer_create(
    timer_callback,  // Callback Function
    100,        // Timer Period (Milliseconds)
    &user_data  // Callback Data
  );
  UNUSED(timer);

  // Create the LVGL Terminal Widgets
  create_widgets();
}

// Callback Function for LVGL Timer.
// Based on https://docs.lvgl.io/master/overview/timer.html#create-a-timer
static void timer_callback(lv_timer_t *timer) {
  int ret;

  // Poll NSH stdout to check if there's output to be processed
  static char buf[64];
  DEBUGASSERT(nsh_stdout[READ_PIPE] != 0);
  if (has_input(nsh_stdout[READ_PIPE])) {

    // Read the output from NSH stdout
    ret = read(
      nsh_stdout[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );

    // Add to NSH Output Text Area
    if (ret > 0) {
      buf[ret] = 0;
      // _info("%s\n", buf); 
      // infodumpbuffer("timer_callback", (const uint8_t *)buf, ret);

      remove_escape_codes(buf, ret);
      DEBUGASSERT(output != NULL);
      lv_textarea_add_text(output, buf);
    }
  }

  // Poll NSH stderr to check if there's output to be processed
  DEBUGASSERT(nsh_stderr[READ_PIPE] != 0);
  if (has_input(nsh_stderr[READ_PIPE])) {

    // Read the output from NSH stderr
    ret = read(    
      nsh_stderr[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );

    // Add to NSH Output Text Area
    if (ret > 0) {
      buf[ret] = 0;
      // _info("%s\n", buf); 
      // infodumpbuffer("timer_callback", (const uint8_t *)buf, ret);

      remove_escape_codes(buf, ret);
      DEBUGASSERT(output != NULL);
      lv_textarea_add_text(output, buf);
    }
  }
}

#ifndef CONFIG_LV_FONT_UNSCII_16
#error "LVGL configuration > Font usage > Enable built-in fonts > UNSCII 16" should be enabled in menuconfig
#endif

// Create the LVGL Widgets for the LVGL Terminal.
// Based on https://docs.lvgl.io/master/widgets/keyboard.html#keyboard-with-text-area
static void create_widgets(void) {

  // Set the Font Style for NSH Input and Output to a Monospaced Font
  lv_style_init(&terminal_style);
  lv_style_set_text_font(&terminal_style, &lv_font_unscii_16);

  // Create an LVGL Container with Column Flex Direction
  col = lv_obj_create(lv_scr_act());
  lv_obj_set_size(col, LV_PCT(100), LV_PCT(100));
  lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(col, 0, 0);  // No padding

  // Create an LVGL Text Area Widget for NSH Output
  output = lv_textarea_create(col);
  lv_obj_add_style(output, &terminal_style, 0);
  lv_obj_set_width(output, LV_PCT(100));
  lv_obj_set_flex_grow(output, 1);  // Fill the column

  // Create an LVGL Text Area Widget for NSH Input
  input = lv_textarea_create(col);
  lv_obj_add_style(input, &terminal_style, 0);
  lv_obj_set_size(input, LV_PCT(100), LV_SIZE_CONTENT);

  // Create an LVGL Keyboard Widget
  kb = lv_keyboard_create(col);
  lv_obj_set_style_pad_all(kb, 0, 0);  // No padding

  // Register the Callback Function for NSH Input
  lv_obj_add_event_cb(input, input_callback, LV_EVENT_ALL, NULL);

  // Set the Keyboard to populate the NSH Input Text Area
  lv_keyboard_set_textarea(kb, input);
}

// Callback Function for NSH Input Text Area.
// Based on https://docs.lvgl.io/master/widgets/keyboard.html#keyboard-with-text-area
static void input_callback(lv_event_t *e) {
  int ret;

  // Decode the LVGL Event
  const lv_event_code_t code = lv_event_get_code(e);
  // const lv_obj_t *ta = lv_event_get_target(e);
  // _info("code=%d\n", code);

  // If Enter has been pressed, send the Command to NSH Input
  if (code == LV_EVENT_VALUE_CHANGED) {

    // Get the Button Index of the Keyboard Button Pressed
    const uint16_t id = lv_keyboard_get_selected_btn(kb);
    // _info("btn=%d\n", id);

    // Get the Text of the Keyboard Button
    const char *key = lv_keyboard_get_btn_text(kb, id);
    if (key == NULL) { return; }
    // _info("key[0]=%d, key=%s\n", key[0], key);
    // infodumpbuffer("input_callback", (const uint8_t *)key, strlen(key));

    // If Enter is pressed...
    if (key[0] == 0xef && key[1] == 0xa2 && key[2] == 0xa2) {

      // Read the NSH Input
      DEBUGASSERT(input != NULL);
      const char *cmd = lv_textarea_get_text(input);
      if (cmd == NULL || cmd[0] == 0) { return; }
      // infodumpbuffer("input_callback", (const uint8_t *)cmd, strlen(cmd));

      // Send the Command to NSH stdin
      DEBUGASSERT(nsh_stdin[WRITE_PIPE] != 0);
      ret = write(
        nsh_stdin[WRITE_PIPE],
        cmd,
        strlen(cmd)
      );
      // _info("write nsh_stdin: %d\n", ret);
      DEBUGASSERT(ret == strlen(cmd));

      // Erase the NSH Input
      lv_textarea_set_text(input, "");
    }
  }
}

// Return true if the File Descriptor has data to be read
static bool has_input(int fd) {

  // Poll the File Descriptor for Input
  struct pollfd fdp;
  fdp.fd = fd;
  fdp.events = POLLIN;
  int ret = poll(
    &fdp,  // File Descriptors
    1,     // Number of File Descriptors
    0      // Poll Timeout (Milliseconds)
  );

  if (ret > 0) {
    // If Poll is OK and there is Input...
    if ((fdp.revents & POLLIN) != 0) {
      // Report that there's Input
      // _info("has input: fd=%d\n", fd);
      return true;
    }

    // Else report No Input
    // _info("no input: fd=%d\n", fd);
    return false;

  } else if (ret == 0) {
    // If Timeout, report No Input
    // _info("timeout: fd=%d\n", fd);
    return false;

  } else if (ret < 0) {
    // Handle Error
    _err("poll failed: %d, fd=%d\n", ret, fd);
    return false;
  }

  // Never comes here
  DEBUGASSERT(false);
  return false;
}

// Remove Escape Codes from the string. Assumes that buf[len] is 0.
static void remove_escape_codes(char *buf, int len) {
  for (int i = 0; i < len; i++) {
    // Escape Code looks like 0x1b 0x5b 0x4b
    if (buf[i] == 0x1b) {
      // Remove 3 bytes
      for (int j = i; j + 2 < len; j++) {
        buf[j] = buf[j + 3];
      }
    }
  }
}
