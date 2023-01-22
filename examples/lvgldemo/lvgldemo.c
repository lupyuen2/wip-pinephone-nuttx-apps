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
#include "nshlib/nshlib.h"

////#define TEST_PTY 
#ifdef TEST_PTY
//////////////////////////////////////////////////////////////////////////
// Pseudo Terminal Implementation

#ifndef CONFIG_PSEUDOTERM
#error Please enable "Device Drivers > Serial Driver Support > Pseudo-Terminal (PTY) support"
#endif

#include <termios.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <nuttx/serial/pty.h>

struct term_pair_s
{
  ////int fd_uart;
  int fd_pty;
};

void test_terminal(void)
{
  _info("test_terminal\n");
  struct term_pair_s termpair;
  /////struct termios tio;
  pthread_t si_thread;
  pthread_t so_thread;
  char buffer[16];
  pid_t pid;
  int fd_pts;
  int ret;

  printf("Create pseudo-terminal\n");

  /* Open pseudo-terminal master (pts factory) */

  termpair.fd_pty = open("/dev/ptmx", O_RDWR | O_NOCTTY);
  if (termpair.fd_pty < 0)
    {
      fprintf(stderr, "ERROR: Failed to opening /dev/ptmx: %d\n",
              errno);
      return;
    }

  /* Grant access and unlock the slave PTY */

  ret = grantpt(termpair.fd_pty);
  if (ret < 0)
    {
      fprintf(stderr, "ERROR: grantpt() failed: %d\n", errno);
      goto error_pts;
    }

  ret = unlockpt(termpair.fd_pty);
  if (ret < 0)
    {
      fprintf(stderr, "ERROR: unlockpt() failed: %d\n", errno);
      goto error_pts;
    }

  /* Get the create pts file-name */

  ret = ptsname_r(termpair.fd_pty, buffer, 16);
  if (ret < 0)
    {
      fprintf(stderr, "ERROR: ptsname_r() failed: %d\n", errno);
      goto error_pts;
    }

  /* Open the created pts */

  fd_pts = open(buffer, O_RDWR);
  if (fd_pts < 0)
    {
      fprintf(stderr, "ERROR: Failed to open %s: %d\n", buffer, errno);
      goto error_pts;
    }

#ifdef NOTUSED
  /* Open the second serial port to create a new console there */

  termpair.fd_uart = open(CONFIG_EXAMPLES_PTYTEST_SERIALDEV, O_RDWR);
  if (termpair.fd_uart < 0)
    {
#ifdef CONFIG_EXAMPLES_PTYTEST_WAIT_CONNECTED
      /* if ENOTCONN is received, re-attempt to open periodically */

      if (errno == ENOTCONN)
        {
          fprintf(stderr, "ERROR: device not connected, will continue"
                          " trying\n");
        }

      while (termpair.fd_uart < 0 && errno == ENOTCONN)
        {
          sleep(1);

          termpair.fd_uart = open(CONFIG_EXAMPLES_PTYTEST_SERIALDEV, O_RDWR);
        }

      /* if we exited due to an error different than ENOTCONN */

      if (termpair.fd_uart < 0)
        {
          fprintf(stderr, "Failed to open %s: %i\n",
                 CONFIG_EXAMPLES_PTYTEST_SERIALDEV, errno);
          goto error_serial;
        }
#else
      fprintf(stderr, "Failed to open %s: %i\n",
             CONFIG_EXAMPLES_PTYTEST_SERIALDEV, errno);
      goto error_serial;
#endif
    }

#ifdef CONFIG_SERIAL_TERMIOS
  /* Enable \n -> \r\n conversion during write */

  ret = tcgetattr(termpair.fd_uart, &tio);
  if (ret)
    {
      fprintf(stderr, "ERROR: tcgetattr() failed: %d\n", errno);
      goto error_serial;
    }

  tio.c_oflag = OPOST | ONLCR;
  ret = tcsetattr(termpair.fd_uart, TCSANOW, &tio);
  if (ret)
    {
      fprintf(stderr, "ERROR: tcsetattr() failed: %d\n", errno);
      goto error_serial;
    }
#endif
#endif  // NOTUSED

  printf("Starting a new NSH Session using %s\n", buffer);

  /* Close default stdin, stdout and stderr */

  close(0);
  close(1);
  close(2);

  /* Use this pts file as stdin, stdout, and stderr */

  dup2(fd_pts, 0);
  dup2(fd_pts, 1);
  dup2(fd_pts, 2);

  /* Create a new console using this /dev/pts/N */

  char *argv[] = { NULL };
  pid = task_create(
    "NSH Console", 
    100,  // Priority
    CONFIG_DEFAULT_TASK_STACKSIZE,
    nsh_consolemain,
    argv
  );
  _info("task_create: pid=%d\n", pid);
  if (pid < 0)
    {
      _err("task_create failed: %d\n", pid);
      goto error_console;
    }

  /* Start a thread to read from serial */

  ret = pthread_create(&si_thread, NULL,
                       (pthread_startroutine_t)serial_in,
                       (pthread_addr_t)&termpair);
  _info("pthread_create serial_in: %d\n", ret);
  if (ret != 0)
    {
      _err("pthread_create serial_in failed: %d\n", pid);
      goto error_thread1;
    }

  /* Start a thread to write to serial */

  ret = pthread_create(&so_thread, NULL,
                       (pthread_startroutine_t)serial_out,
                       (pthread_addr_t)&termpair);
  _info("pthread_create serial_out: %d\n", ret);
  if (ret != 0)
    {
      _err("pthread_create serial_out failed: %d\n", pid);
      goto error_thread2;
    }

  /* Stay here to keep the threads running */

  while (true)
    {
      /* Nothing to do, then sleep to avoid eating all cpu time */

      pause();
    }

  return;

error_thread2:
error_thread1:
error_console:
  ////close(termpair.fd_uart);
error_serial:
  close(fd_pts);
error_pts:
  close(termpair.fd_pty);
}

#else  // !TEST_PTY
//////////////////////////////////////////////////////////////////////////
// Pipe Implementation

#ifndef CONFIG_DEV_PIPE_SIZE
#error Please enable "Device Drivers > FIFO and named pipe drivers" in menuconfig
#endif

void test_terminal(void)
{
  _info("test_terminal\n");

  /* Create the pipes */

  int nsh_stdin[2];
  int nsh_stdout[2];
  int nsh_stderr[2];
  int ret;
  ret = pipe(nsh_stdin);  if (ret < 0) { _err("stdin pipe failed: %d\n", errno);  return; }
  ret = pipe(nsh_stdout); if (ret < 0) { _err("stdout pipe failed: %d\n", errno); return; }
  ret = pipe(nsh_stderr); if (ret < 0) { _err("stderr pipe failed: %d\n", errno); return; }

  /* Close default stdin, stdout and stderr */

  close(0);
  close(1);
  close(2);

  /* Use the pipes as stdin, stdout, and stderr */

  #define READ_PIPE  0  // Read Pipes: stdin, stdout, stderr
  #define WRITE_PIPE 1  // Write Pipes: stdin, stdout, stderr
  dup2(nsh_stdin[READ_PIPE], 0);
  dup2(nsh_stdout[WRITE_PIPE], 1);
  dup2(nsh_stderr[WRITE_PIPE], 2);

  /* Create a new console using the pipes */

  char *argv[] = { NULL };
  pid_t pid = task_create(
    "NSH Console",
    100,  // Priority
    CONFIG_DEFAULT_TASK_STACKSIZE,
    nsh_consolemain,
    argv
  );
  if (pid < 0)
    {
      _err("task_create failed: %d\n", errno);
      return;
    }
  _info("pid=%d\n", pid);

  // Wait a while
  sleep(1);

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

    // Read the output from NSH stdout
    static char buf[64];
    ret = read(
      nsh_stdout[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );
    _info("read nsh_stdout: %d\n", ret);
    if (ret > 0) {
      buf[ret] = 0;
      _info("%s\n", buf);
    }

    // Wait a while
    sleep(1);

#ifdef NOTUSED
    // Read the output from NSH stderr
    ret = read(    
      nsh_stderr[READ_PIPE],
      buf,
      sizeof(buf) - 1
    );
    _info("read nsh_stderr: %d\n", ret);
    if (ret > 0) {
      buf[ret] = 0;
      _info("%s\n", buf);
    }
#endif

  }
}

/* Output:
NuttShell (NSH) NuttX-12.0.0
nsh> ls
/:
 dev/
 var/
nsh> est_terminal: write nsh_stdin: 9
test_terminal: read nsh_stdout: 63
test_terminal: K
nsh> 
nsh> 
nsh> 
nsh> 
nsh> ls
/:
 dev/
 var/
test_terminal: write nsh_stdin: 9
test_terminal: read nsh_stdout: 63
test_terminal: 
nsh> 
nsh> 
nsh> 
nsh> 
nsh> 
nsh> ls
/:
 de
test_terminal: write nsh_stdin: 9
test_terminal: read nsh_stdout: 63
test_terminal: v/
 var/
nsh> 
nsh> 
nsh> 
nsh> 
nsh> 
nsh> l
test_terminal: write nsh_stdin: 9
test_terminal: read nsh_stdout: 63
test_terminal: s
/:
 dev/
 var/
nsh> 
nsh> 
nsh> 
nsh> 
nsh> 
n
*/
#endif  // !TEST_PTY
