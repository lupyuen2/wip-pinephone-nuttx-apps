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
#include <stdio.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * hello_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int i;
  printf("Hello, World!!\n");

  for (i = 0; ; i++)
    {
      void *p = malloc(8192);
      if (p == NULL) { break; }
      if (i % 10 == 0) { printf("i=%d\n", i); }
    }
  printf("malloc failed at i=%d\n", i);

  return 0;
}

/*
malloc failed at i=430
nsh> free
                   total       used       free    largest  nused  nfree
        Kmem:    2069496      13736    2055760    2054128     41      3
        Page:    4194304     606208    3588096    3588096
nsh> 
*/
