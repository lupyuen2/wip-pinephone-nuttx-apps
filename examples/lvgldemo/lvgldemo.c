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
  ////TODO: Begin
  void test_terminal(void);
  test_terminal();
  ////TODO: End

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

  /* LVGL demo creation */

  demo_create_func();

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
// Testing LVGL Terminal

#ifndef CONFIG_DEV_PIPE_SIZE
#error Please enable "Device Drivers > FIFO and named pipe drivers" in menuconfig
#endif

#include <lvgl/lvgl.h>
#include "nshlib/nshlib.h"

void my_timer(lv_timer_t * timer);

// Create an LVGL Terminal that will let us interact with NuttX NSH Shell
void test_terminal(void)
{
  _info("test_terminal\n");

  // Create an LVGL Timer to poll for output from NSH Shell
  static uint32_t user_data = 10;
  lv_timer_t *timer = lv_timer_create(
    my_timer,   // Callback
    5000,       // Timer Period (Milliseconds)
    &user_data  // Callback Data
  );
  UNUSED(timer);

  // Create the pipes for NSH Shell
  int nsh_stdin[2];
  int nsh_stdout[2];
  int nsh_stderr[2];
  int ret;
  ret = pipe(nsh_stdin);  if (ret < 0) { _err("stdin pipe failed: %d\n", errno);  return; }
  ret = pipe(nsh_stdout); if (ret < 0) { _err("stdout pipe failed: %d\n", errno); return; }
  ret = pipe(nsh_stderr); if (ret < 0) { _err("stderr pipe failed: %d\n", errno); return; }

  // Close default stdin, stdout and stderr
  close(0);
  close(1);
  close(2);

  // Use the pipes as NSH stdin, stdout and stderr
  #define READ_PIPE  0  // Read Pipes: stdin, stdout, stderr
  #define WRITE_PIPE 1  // Write Pipes: stdin, stdout, stderr
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

  // Wait a while
  sleep(1);

  // Send a few commands to NSH
  for (int i = 0; i < 5; i++) {

    // Send a command to NSH stdin
    const char cmd[] = "ls\r\n\r\n\r\n";
    ret = write(
      nsh_stdin[WRITE_PIPE],
      cmd,
      sizeof(cmd)
    );
    _info("write nsh_stdin: %d\n", ret);

    // Wait a while
    sleep(1);

    // Read the output from NSH stdout.
    // TODO: This will block if there's nothing to read.
    static char buf[64];
    ret = read(
      nsh_stdout[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );
    _info("read nsh_stdout: %d\n", ret);
    if (ret > 0) { buf[ret] = 0; _info("%s\n", buf); }

    // Wait a while
    sleep(1);

#ifdef NOTUSED
    // Read the output from NSH stderr.
    // TODO: This will block if there's nothing to read.
    ret = read(    
      nsh_stderr[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );
    _info("read nsh_stderr: %d\n", ret);
    if (ret > 0) { buf[ret] = 0; _info("%s\n", buf); }
#endif

  }
}

// Callback for LVGL Timer
void my_timer(lv_timer_t *timer) {

  // Get the Callback Data
  uint32_t *user_data = timer->user_data;
  _info("my_timer called with callback data: %d\n", *user_data);
  *user_data += 1;

  // TODO: Call poll() to check if NSH Stdout has data to be read

  // TODO: Read the NSH Stdout

  // TODO: Write NSH Output to LVGL Label Widget
}

// TODO: Read input from LVGL Text Area Widget

// TODO: Send input to NSH Stdin

/* Output:
- Ready to Boot CPU
- Boot from EL2
- Boot from EL1
- Boot to C runtime for OS Initialize
test_terminal: test_terminal
up_dump_register: stack = 0x40a77cf0
up_dump_register: x0:   0xffffffff          x1:   0xeafffffe
up_dump_register: x2:   0x18                x3:   0x3
up_dump_register: x4:   0x40a77e3c          x5:   0x40a77e38
up_dump_register: x6:   0x18                x7:   0x0
up_dump_register: x8:   0x0                 x9:   0x0
up_dump_register: x10:  0x0                 x11:  0x0
up_dump_register: x12:  0x0                 x13:  0x0
up_dump_register: x14:  0x0                 x15:  0x1c28000
up_dump_register: x16:  0x0                 x17:  0x0
up_dump_register: x18:  0x0                 x19:  0xe24ee004e59ff0e8
up_dump_register: x20:  0x18                x21:  0x4008a2c8
up_dump_register: x22:  0x1388              x23:  0x0
up_dump_register: x24:  0x0                 x25:  0x0
up_dump_register: x26:  0x0                 x27:  0x0
up_dump_register: x28:  0x0                 x29:  0x0
up_dump_register: x30:  0x400a303c        
up_dump_register: 
up_dump_register: STATUS Registers:
up_dump_register: SPSR:      0x80000005        
up_dump_register: ELR:       0x400a30bc        
up_dump_register: SP_EL0:    0x40a77e60        
up_dump_register: SP_ELX:    0x40a77cf0        
up_dump_register: TPIDR_EL0: 0x40a75b80        
up_dump_register: TPIDR_EL1: 0x40a75b80        
up_dump_register: EXE_DEPTH: 0x0  
*/
