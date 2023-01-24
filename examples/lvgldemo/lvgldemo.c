/****************************************************************************
 * apps/examples/lvgldemo/lvgldemo.c
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

#include <lvgl/lvgl.h>
#include <port/lv_port.h>
#include <lvgl/demos/lv_demos.h>

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
 * Private Type Declarations
 ****************************************************************************/

typedef CODE void (*demo_create_func_t)(void);

struct func_key_pair_s
{
  FAR const char *name;
  demo_create_func_t func;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct func_key_pair_s func_key_pair[] =
{
#ifdef CONFIG_LV_USE_DEMO_WIDGETS
  { "widgets",        lv_demo_widgets        },
#endif

#ifdef CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
  { "keypad_encoder", lv_demo_keypad_encoder },
#endif

#ifdef CONFIG_LV_USE_DEMO_BENCHMARK
  { "benchmark",      lv_demo_benchmark      },
#endif

#ifdef CONFIG_LV_USE_DEMO_STRESS
  { "stress",         lv_demo_stress         },
#endif

#ifdef CONFIG_LV_USE_DEMO_MUSIC
  { "music",          lv_demo_music          },
#endif
  { "", NULL }
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: show_usage
 ****************************************************************************/

static void show_usage(void)
{
  int i;
  const int len = sizeof(func_key_pair)
                  / sizeof(struct func_key_pair_s) - 1;

  if (len == 0)
    {
      printf("lvgldemo: no demo available!\n");
      exit(EXIT_FAILURE);
      return;
    }

  printf("\nUsage: lvgldemo demo_name\n");
  printf("\ndemo_name:\n");

  for (i = 0; i < len; i++)
    {
      printf("  %s\n", func_key_pair[i].name);
    }

  exit(EXIT_FAILURE);
}

/****************************************************************************
 * Name: find_demo_create_func
 ****************************************************************************/

static demo_create_func_t find_demo_create_func(FAR const char *name)
{
  int i;
  const int len = sizeof(func_key_pair)
                  / sizeof(struct func_key_pair_s) - 1;

  for (i = 0; i < len; i++)
    {
      if (strcmp(name, func_key_pair[i].name) == 0)
        {
          return func_key_pair[i].func;
        }
    }

  printf("lvgldemo: '%s' not found.\n", name);
  return NULL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main or lvgldemo_main
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
  demo_create_func_t demo_create_func;
  FAR const char *demo = NULL;
  const int func_key_pair_len = sizeof(func_key_pair) /
                                sizeof(func_key_pair[0]);

  /* If no arguments are specified and only 1 demo exists, select the demo */

  if (argc == 1 && func_key_pair_len == 2)  /* 2 because of NULL sentinel */
    {
      demo = func_key_pair[0].name;
    }
  else if (argc != 2)
    {
      show_usage();
      return EXIT_FAILURE;
    }
  else
    {
      demo = argv[1];
    }

  demo_create_func = find_demo_create_func(demo);

  if (demo_create_func == NULL)
    {
      show_usage();
      return EXIT_FAILURE;
    }

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

  // Create an LVGL Terminal that will let us interact with NuttX NSH Shell
  void test_terminal(void);
  test_terminal();

  /* LVGL demo creation */

  ////demo_create_func();

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

//////////////////////////////////////////////////////////////////////////
// LVGL Terminal

#ifndef CONFIG_DEV_PIPE_SIZE
#error Please enable "Device Drivers > FIFO and named pipe drivers" in menuconfig
#endif

#include <poll.h>
#include "nshlib/nshlib.h"

static bool has_input(int fd);
static void timer_callback(lv_timer_t * timer);
static void create_widgets(void);
static void input_callback(lv_event_t * e);

// Pipes for NSH Shell: : stdin, stdout, stderr
static int nsh_stdin[2];
static int nsh_stdout[2];
static int nsh_stderr[2];

#define READ_PIPE  0  // Read Pipes: stdin, stdout, stderr
#define WRITE_PIPE 1  // Write Pipes: stdin, stdout, stderr

// LVGL Text Areas for NSH Input and Output
static lv_obj_t *input;
static lv_obj_t *output;

// Create an LVGL Terminal that will let us interact with NuttX NSH Shell
void test_terminal(void) {
  _info("test_terminal\n");

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
    100,  // Priority
    CONFIG_DEFAULT_TASK_STACKSIZE,
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
    1000,       // Timer Period (Milliseconds)
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

  // Get the Callback Data
  uint32_t *user_data = timer->user_data;
  // _info("timer_callback called with callback data: %d\n", *user_data);
  *user_data += 1;

  // Read the output from NSH stdout
  static char buf[64];
  DEBUGASSERT(nsh_stdout[READ_PIPE] != 0);
  if (has_input(nsh_stdout[READ_PIPE])) {
    ret = read(
      nsh_stdout[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );
    // _info("read nsh_stdout: %d\n", ret);
    if (ret > 0) {
      // Add to NSH Output Text Area
      buf[ret] = 0;
      DEBUGASSERT(output != NULL);
      lv_textarea_add_text(output, buf);
      _info("%s\n", buf); 
    }
  }

  // Read the output from NSH stderr
  DEBUGASSERT(nsh_stderr[READ_PIPE] != 0);
  if (has_input(nsh_stderr[READ_PIPE])) {
    ret = read(    
      nsh_stderr[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );
    // _info("read nsh_stderr: %d\n", ret);
    if (ret > 0) {
      // Add to NSH Output Text Area
      buf[ret] = 0;
      DEBUGASSERT(output != NULL);
      lv_textarea_add_text(output, buf);
      _info("%s\n", buf); 
    }
  }
}

// PinePhone LCD Panel Width and Height (pixels)
#define PINEPHONE_LCD_PANEL_WIDTH  720
#define PINEPHONE_LCD_PANEL_HEIGHT 1440

// Margin of 10 pixels all around
#define TERMINAL_MARGIN 10

// Terminal Width is LCD Width minus Left and Right Margins
#define TERMINAL_WIDTH  (PINEPHONE_LCD_PANEL_WIDTH - 2 * TERMINAL_MARGIN)

// Keyboard is Lower Half of LCD.
// Terminal Height is Upper Half of LCD minus Top and Bottom Margins.
#define TERMINAL_HEIGHT ((PINEPHONE_LCD_PANEL_HEIGHT / 2) - 2 * TERMINAL_MARGIN)

// Height of Input Text Area
#define INPUT_HEIGHT 100

// Height of Output Text Area is Terminal Height minus Input Height minus Middle Margin
#define OUTPUT_HEIGHT (TERMINAL_HEIGHT - INPUT_HEIGHT - TERMINAL_MARGIN)

// Create the LVGL Widgets for the LVGL Terminal.
// Based on https://docs.lvgl.io/master/widgets/keyboard.html#keyboard-with-text-area
static void create_widgets(void) {

  // Create an LVGL Keyboard Widget
  lv_obj_t *kb = lv_keyboard_create(lv_scr_act());

  // Create an LVGL Text Area Widget for NSH Output
  output = lv_textarea_create(lv_scr_act());
  lv_obj_align(output, LV_ALIGN_TOP_LEFT, TERMINAL_MARGIN, TERMINAL_MARGIN);
  lv_textarea_set_placeholder_text(output, "Hello");
  lv_obj_set_size(output, TERMINAL_WIDTH, OUTPUT_HEIGHT);

  // Create an LVGL Text Area Widget for NSH Input
  input = lv_textarea_create(lv_scr_act());
  lv_obj_align(input, LV_ALIGN_TOP_LEFT, TERMINAL_MARGIN, OUTPUT_HEIGHT + 2 * TERMINAL_MARGIN);
  lv_obj_add_event_cb(input, input_callback, LV_EVENT_ALL, kb);
  lv_obj_set_size(input, TERMINAL_WIDTH, INPUT_HEIGHT);

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

    // Get the Keyboard Widget from the LVGL Event
    const lv_obj_t *kb = lv_event_get_user_data(e);
    DEBUGASSERT(kb != NULL);

    // Get the Button Index of the Keyboard Button Pressed
    const uint16_t id = lv_keyboard_get_selected_btn(kb);
    // _info("btn=%d\n", id);

    // Get the Text of the Keyboard Button
    const char *key = lv_keyboard_get_btn_text(kb, id);
    if (key == NULL) { return; }
    _info("key[0]=%d, key=%s\n", key[0], key);
    infodumpbuffer("input_callback", (const uint8_t *)key, strlen(key));

    // If Enter is pressed...
    if (key[0] == 0xef && key[1] == 0xa2 && key[2] == 0xa2) {

      // Read the NSH Input
      DEBUGASSERT(input != NULL);
      const char *cmd = lv_textarea_get_text(input);
      if (cmd == NULL || cmd[0] == 0) { return; }
      infodumpbuffer("input_callback", (const uint8_t *)cmd, strlen(cmd));

      // Send the Command to NSH stdin
      DEBUGASSERT(nsh_stdin[WRITE_PIPE] != 0);
      ret = write(
        nsh_stdin[WRITE_PIPE],
        cmd,
        strlen(cmd)
      );
      _info("write nsh_stdin: %d\n", ret);

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
    (struct pollfd *)&fdp,  // File Descriptors
    1,  // Number of File Descriptors
    0   // Poll Timeout (Milliseconds)
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
    // Ignore Timeout
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

/* Output:
DRAM: 2048 MiB
Trying to boot from MMC1
NOTICE:  BL31: v2.2(release):v2.2-904-gf9ea3a629
NOTICE:  BL31: Built : 15:32:12, Apr  9 2020
NOTICE:  BL31: Detected Allwinner A64/H64/R18 SoC (1689)
NOTICE:  BL31: Found U-Boot DTB at 0x4064410, model: PinePhone
NOTICE:  PSCI: System suspend is unavailable


U-Boot 2020.07 (Nov 08 2020 - 00:15:12 +0100)

DRAM:  2 GiB
MMC:   Device 'mmc@1c11000': seq 1 is in use by 'mmc@1c10000'
mmc@1c0f000: 0, mmc@1c10000: 2, mmc@1c11000: 1
Loading Environment from FAT... *** Warning - bad CRC, using default environment

starting USB...
No working controllers found
Hit any key to stop autoboot:  0 
switch to partitions #0, OK
mmc0 is current device
Scanning mmc 0:1...
Found U-Boot script /boot.scr
653 bytes read in 3 ms (211.9 KiB/s)
## Executing script at 4fc00000
gpio: pin 114 (gpio 114) value is 1
290206 bytes read in 16 ms (17.3 MiB/s)
Uncompressed size: 10412032 = 0x9EE000
36162 bytes read in 5 ms (6.9 MiB/s)
1078500 bytes read in 51 ms (20.2 MiB/s)
## Flattened Device Tree blob at 4fa00000
   Booting using the fdt blob at 0x4fa00000
   Loading Ramdisk to 49ef8000, end 49fff4e4 ... OK
   Loading Device Tree to 0000000049eec000, end 0000000049ef7d41 ... OK

Starting kernel ...

- Ready to Boot CPU
- Boot from EL2
- Boot from EL1
- Boot to C runtime for OS Initialize
test_terminal: test_terminal
test_terminal: pid=3
timer_callback: 
NuttShell (NSH) NuttX-12.0.0
nsh> 
input_callback: key[0]=113, key=q
input_callback (0x400fbf76):
0000  71                                               q               
input_callback: key[0]=119, key=w
input_callback (0x400fbf6f):
0000  77                                               w               
input_callback: key[0]=101, key=e
input_callback (0x400fbf69):
0000  65                                               e               
input_callback: key[0]=114, key=r
input_callback (0x400fbbaa):
0000  72                                               r               
input_callback: key[0]=239, key=Ô¢¢
input_callback (0x400fadf3):
0000  ef a2 a2                                         ...             
input_callback (0x401129e8):
0000  71 77 65 72 0a                                   qwer.           
input_callback: write nsh_stdin: 5
input_callback: key[0]=239, key=Ô¢¢
input_callback (0x400fadf3):
0000  ef a2 a2                                         ...             
timer_callback: qwer
nsh> 
timer_callback: nsh: qwer: command not found

input_callback: key[0]=108, key=l
input_callback (0x400fc36b):
0000  6c                                               l               
input_callback: key[0]=115, key=s
input_callback (0x400fb2b8):
0000  73                                               s               
input_callback: key[0]=239, key=Ô¢¢
input_callback (0x400fadf3):
0000  ef a2 a2                                         ...             
input_callback (0x401125a0):
0000  6c 73 0a                                         ls.             
input_callback: write nsh_stdin: 3
input_callback: key[0]=239, key=Ô¢¢
input_callback (0x400fadf3):
0000  ef a2 a2                                         ...             
timer_callback: ls
/:
 dev/
 proc/
 var/
nsh> 
input_callback: key[0]=117, key=u
input_callback (0x400fbdf0):
0000  75                                               u               
input_callback: key[0]=110, key=n
input_callback (0x400fbf60):
0000  6e                                               n               
input_callback: key[0]=97, key=a
input_callback (0x400fbf8d):
0000  61                                               a               
input_callback: key[0]=110, key=n
input_callback (0x400fbf60):
0000  6e                                               n               
input_callback: key[0]=101, key=e
input_callback (0x400fbf69):
0000  65                                               e               
input_callback: key[0]=239, key=Ôïö
input_callback (0x400fadff):
0000  ef 95 9a                                         ...             
input_callback: key[0]=239, key=Ôïö
input_callback (0x400fadff):
0000  ef 95 9a                                         ...             
input_callback: key[0]=109, key=m
input_callback (0x400faaa3):
0000  6d                                               m               
input_callback: key[0]=101, key=e
input_callback (0x400fbf69):
0000  65                                               e               
input_callback: key[0]=32, key= 
input_callback (0x400fbeab):
0000  20                                                               
input_callback: key[0]=45, key=-
input_callback (0x400fbb68):
0000  2d                                               -               
input_callback: key[0]=97, key=a
input_callback (0x400fbf8d):
0000  61                                               a               
input_callback: key[0]=239, key=Ô¢¢
input_callback (0x400fadf3):
0000  ef a2 a2                                         ...             
input_callback (0x401125a0):
0000  75 6e 61 6d 65 20 2d 61 0a                       uname -a.       
input_callback: write nsh_stdin: 9
input_callback: key[0]=239, key=Ô¢¢
input_callback (0x400fadf3):
0000  ef a2 a2                                         ...             
timer_callback: uname -a
NuttX 12.0.0 bd6a0b0 Jan 22 2023 20:01:15 arm64 pineph
timer_callback: one
nsh> 
input_callback: key[0]=239, key=Ô¢¢
input_callback (0x400fadf3):
0000  ef a2 a2                                         ...             
input_callback (0x401125a0):
0000  0a                                               .               
input_callback: write nsh_stdin: 1
input_callback: key[0]=239, key=Ô¢¢
input_callback (0x400fadf3):
0000  ef a2 a2                                         ...             
timer_callback: 
nsh> 
*/
