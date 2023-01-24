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
static void my_timer(lv_timer_t * timer);
static void lv_example_keyboard_1(void);
static void ta_event_cb(lv_event_t * e);

// Pipes for NSH Shell: : stdin, stdout, stderr
static int nsh_stdin[2];
static int nsh_stdout[2];
static int nsh_stderr[2];

#define READ_PIPE  0  // Read Pipes: stdin, stdout, stderr
#define WRITE_PIPE 1  // Write Pipes: stdin, stdout, stderr

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
  static uint32_t user_data = 10;
  lv_timer_t *timer = lv_timer_create(
    my_timer,   // Callback
    1000,       // Timer Period (Milliseconds)
    &user_data  // Callback Data
  );
  UNUSED(timer);

  // Create the LVGL Terminal Widgets
  lv_example_keyboard_1();
}

// Callback for LVGL Timer
static void my_timer(lv_timer_t *timer) {
  int ret;

  // Get the Callback Data
  uint32_t *user_data = timer->user_data;
  _info("my_timer called with callback data: %d\n", *user_data);
  *user_data += 1;

  // Send a command to NSH stdin
  if (*user_data % 5 == 0) {
    const char cmd[] = "ls\r";
    DEBUGASSERT(nsh_stdin[WRITE_PIPE] != 0);
    ret = write(
      nsh_stdin[WRITE_PIPE],
      cmd,
      sizeof(cmd)
    );
    _info("write nsh_stdin: %d\n", ret);
  }

  // Read the output from NSH stdout
  static char buf[64];
  DEBUGASSERT(nsh_stdout[READ_PIPE] != 0);
  if (has_input(nsh_stdout[READ_PIPE])) {
    ret = read(
      nsh_stdout[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );
    _info("read nsh_stdout: %d\n", ret);
    if (ret > 0) { buf[ret] = 0; _info("%s\n", buf); }
  }

  // Read the output from NSH stderr
  DEBUGASSERT(nsh_stderr[READ_PIPE] != 0);
  if (has_input(nsh_stderr[READ_PIPE])) {
    ret = read(    
      nsh_stderr[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );
    _info("read nsh_stderr: %d\n", ret);
    if (ret > 0) { buf[ret] = 0; _info("%s\n", buf); }
  }

  // TODO: Write the NSH Output to LVGL Label Widget
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
      _info("has input: fd=%d\n", fd);
      return true;
    }

    // Else report No Input
    _info("no input: fd=%d\n", fd);
    return false;

  } else if (ret == 0) {
    // Ignore Timeout
    _info("timeout: fd=%d\n", fd);
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

static void lv_example_keyboard_1(void)
{
    /*Create a keyboard to use it with an of the text areas*/
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());

    /*Create a text area. The keyboard will write here*/
    lv_obj_t * ta;
    ta = lv_textarea_create(lv_scr_act());
    lv_obj_align(ta, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "Hello");
    lv_obj_set_size(ta, 140, 80);

    ta = lv_textarea_create(lv_scr_act());
    lv_obj_align(ta, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_set_size(ta, 140, 80);

    lv_keyboard_set_textarea(kb, ta);
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
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
295288 bytes read in 17 ms (16.6 MiB/s)
Uncompressed size: 10428416 = 0x9F2000
36162 bytes read in 4 ms (8.6 MiB/s)
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
my_timer: my_timer called with callback data: 10
has_input: has input: fd=8
my_timer: read nsh_stdout: 63
my_timer: createWidgetsWrapped: start
createWidgetsWrapped: end

NuttShel
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 11
has_input: has input: fd=8
my_timer: read nsh_stdout: 29
my_timer: l (NSH) NuttX-12.0.0
nsh> 
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 12
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 13
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 14
my_timer: write nsh_stdin: 4
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 15
has_input: has input: fd=8
my_timer: read nsh_stdout: 33
my_timer: ls
/:
 dev/
 proc/
 var/
nsh> 
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 16
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 17
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 18
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 19
my_timer: write nsh_stdin: 4
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 20
has_input: has input: fd=8
my_timer: read nsh_stdout: 33
my_timer: ls
/:
 dev/
 proc/
 var/
nsh> 
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 21
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 22
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 23
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 24
my_timer: write nsh_stdin: 4
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 25
has_input: has input: fd=8
my_timer: read nsh_stdout: 33
my_timer: ls
/:
 dev/
 proc/
 var/
nsh> 
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 26
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 27
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 28
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 29
my_timer: write nsh_stdin: 4
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 30
has_input: has input: fd=8
my_timer: read nsh_stdout: 33
my_timer: ls
/:
 dev/
 proc/
 var/
nsh> 
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 31
has_input: timeout: fd=8
has_input: timeout: fd=10
my_timer: my_timer called with callback data: 32
has_input: timeout: fd=8
has_input: timeout: fd=10
*/
