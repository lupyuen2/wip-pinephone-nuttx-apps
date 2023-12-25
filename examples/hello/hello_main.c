/****************************************************************************
 * apps/examples/hello/hello_main.c
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
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>

#include <nuttx/leds/userled.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * hello_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  printf("Hello, World!!\n");

  // Open the LED Driver
  printf("Opening /dev/userleds\n");
  int fd = open("/dev/userleds", O_WRONLY);
  if (fd < 0)
    {
      int errcode = errno;
      printf("ERROR: Failed to open /dev/userleds: %d\n",
             errcode);
      return EXIT_FAILURE;
    }

  // Turn on LED
  puts("Set LED 0 to 1");
  int ret = ioctl(fd, ULEDIOC_SETALL, 1);
  if (ret < 0)
    {
      int errcode = errno;
      printf("ERROR: ioctl(ULEDIOC_SUPPORTED) failed: %d\n",
              errcode);
      return EXIT_FAILURE;
    }

  // Sleep a while
  puts("Waiting...");
  usleep(500 * 1000L);

  // Turn off LED
  puts("Set LED 0 to 0");
  ret = ioctl(fd, ULEDIOC_SETALL, 0);
  if (ret < 0)
    {
      int errcode = errno;
      printf("ERROR: ioctl(ULEDIOC_SUPPORTED) failed: %d\n",
              errcode);
      return EXIT_FAILURE;
    }

  // Close the LED Driver
  close(fd);

  return 0;
}
