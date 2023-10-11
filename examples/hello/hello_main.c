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
Default Page Heap of 4 MB:

malloc failed at i=430
nsh> free
                   total       used       free    largest  nused  nfree
        Kmem:    2069496      13736    2055760    2054128     41      3
        Page:    4194304     606208    3588096    3588096
430 * 8192 = 3.5 MB

After increasing Page Heap from 4 MB to 8 MB:

Increase Page Pool Size from 4 MB to 8 MB:
CONFIG_ARCH_PGPOOL_SIZE=8388608
CONFIG_ARCH_PGPOOL_VBASE=0x80400000

malloc failed at i=940
nsh> free
                   total       used       free    largest  nused  nfree
        Kmem:    2069496      13864    2055632    2054128     41      3
        Page:    8388608     606208    7782400    7782400
940 * 8192 = 7.7 MB

If we don't update CONFIG_ARCH_PGPOOL_SIZE:
Crash at 0x80008c84 (memset)

i=420
[    6.696000] riscv_exception: EXCEPTION: Store/AMO access fault. MCAUSE: 0000000000000007, EPC: 0000000080008c84, MTVAL: 0000000000000000
[    6.696000] riscv_exception: PANIC!!! Exception = 0000000000000007
[    6.696000] _assert: Current Version: NuttX  12.3.0-RC0 6f87f26-dirty Oct 11 2023 13:06:26 risc-v
[    6.696000] _assert: Assertion failed panic: at file: common/riscv_exception.c:85 task: /system/bin/init process: /system/bin/init 0xc000004a
[    6.696000] up_dump_register: EPC: 0000000080008c84
[    6.696000] up_dump_register: A0: 0000000000000000 A1: 0000000000000000 A2: 0000000000001000 A3: 0000000000001000
[    6.696000] up_dump_register: A4: 0000000000000000 A5: 0000000000000000 A6: 0000000000000000 A7: 0000000000000000
[    6.696000] up_dump_register: T0: 0000000000000003 T1: 0000000000000007 T2: 0000000000000020 T3: 0000000000001000
[    6.696000] up_dump_register: T4: 0000000000000000 T5: 00000000000000ff T6: 000000000000000f
[    6.696000] up_dump_register: S0: 000000008069d000 S1: 0000000000000001 S2: ffffffff7ff00000 S3: 00000000c0562000
[    6.696000] up_dump_register: S4: ffffffff7fc00000 S5: 00000000802099f0 S6: 0000000000400000 S7: 0000000000100000
[    6.696000] up_dump_register: S8: ffffffff7fefffff S9: 00000000000ffffe S10: 0000000080800000 S11: 00000000c0561000
[    6.696000] up_dump_register: SP: 000000008020b180 FP: 000000008069d000 TP: 0000000000000000 RA: 000000008000b17c
[    6.696000] dump_stacks: ERROR: Stack pointer is not within the stack
[    6.696000] dump_stack: IRQ Stack:
[    6.696000] dump_stack:   base: 0x802002b0
[    6.696000] dump_stack:   size: 00002048
[    6.696000] stack_dump: 0x802002a0: 000000ff 00000000 00000000 00000000 deadbeef deadbeef 80017b80 00000000
[    6.696000] stack_dump: 0x802002c0: deadbeef deadbeef deadbeef deadbeef 00000000 00000000 0000002e 00000000
[    6.696000] stack_dump: 0x802002e0: 0000006a 00000000 000001ff 00000000 00000032 00000000 80200298 00000000
[    6.696000] stack_dump: 0x80200300: 00000030 00000000 00000030 00000000 00000010 00000000 80200140 00000000
[    6.696000] stack_dump: 0x80200320: 00000008 00000000 00042120 00000002 00000009 00000000 00000000 00000000
[    6.696000] stack_dump: 0x80200340: fffffffc ffffffff 00000045 00000000 00000001 00000000 00000001 00000000
[    6.696000] stack_dump: 0x80200360: 8001b3f4 00000000 00000a00 00000000 80200818 00000000 00000006 00000000
[    6.696000] stack_dump: 0x80200380: 00000081 00000000 00000081 00000000 0000006c 00000000 00000068 00000000
[    6.696000] stack_dump: 0x802003a0: 00000009 00000000 0000002a 00000000 deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802003c0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802003e0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200400: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200420: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200440: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200460: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200480: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802004a0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802004c0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802004e0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200500: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200520: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200540: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200560: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200580: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802005a0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802005c0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802005e0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200600: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200620: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200640: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200660: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x80200680: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802006a0: deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef
[    6.696000] stack_dump: 0x802006c0: deadbeef deadbeef 00000020 00000000 80200818 00000000 80016d14 00000000
[    6.696000] stack_dump: 0x802006e0: 00000020 00000000 00000006 00000000 0000000e 00000000 fffffffc ffffffff
[    6.696000] stack_dump: 0x80200700: 80018880 00000000 0000000a 00000000 80200818 00000000 0000000a 00000000
[    6.696000] stack_dump: 0x80200720: 80200818 00000000 80016d14 00000000 0000000a 00000000 00000008 00000000
[    6.696000] stack_dump: 0x80200740: 0000005f 00000000 80007cba 00000000 802008f0 00000000 c0202c00 00000000
[    6.696000] stack_dump: 0x80200760: ffff9fef ffffffff 00004010 00000000 00042120 00000002 32633030 63303230
[    6.696000] stack_dump: 0x80200780: 802002b0 00000000 80018580 00000000 80200868 00000000 c0561000 00000000
[    6.696000] stack_dump: 0x802007a0: 802088e0 00000000 c0202c00 00000000 00000bc0 00000000 80201898 00000000
[    6.696000] stack_dump: 0x802007c0: 00042120 00000002 c0202040 00000000 802002b0 00000000 80018580 00000000
[    6.696000] stack_dump: 0x802007e0: 80200868 00000000 80018580 00000000 0000000f 00000000 80016454 00000000
[    6.696000] stack_dump: 0x80200800: 80200888 00000000 00000006 00000000 297c1e00 00000000 0000006f 00000000
[    6.696000] stack_dump: 0x80200820: 80016ce2 00000000 80016c82 00000000 80008b42 00000000 0000000a 00000000
[    6.696000] stack_dump: 0x80200840: 80018880 00000000 80200aa0 00000000 80200840 00000000 80008d4a 00000000
[    6.696000] stack_dump: 0x80200860: 00000000 00000000 802008a0 00000000 00000800 00000000 80008d6a 00000000
[    6.696000] stack_dump: 0x80200880: 80008b42 00000000 802008a0 00000000 00000001 00000000 80001bfa 00000000
[    6.696000] stack_dump: 0x802008a0: 80018880 00000000 80200880 00000000 80008b42 ffffffff 00000000 00000000
[    6.696000] stack_dump: 0x802008c0: 80200880 ffffffff 00000000 00000000 80008b42 ffffffff ffffffff ffffffff
[    6.696000] stack_dump: 0x802008e0: 00000001 ffffffff ffffffff ffffffff 800188a0 00000000 00000001 00000000
[    6.696000] stack_dump: 0x80200900: 00000001 00000000 802096d0 00000000 00000000 00000000 80001fa6 00000000
[    6.696000] stack_dump: 0x80200920: c000004a 00000000 c0561000 00000000 80800000 00000000 000ffffe 00000000
[    6.696000] stack_dump: 0x80200940: 802096d0 00000000 8020af70 00000000 800180c0 00000000 80018170 00000000
[    6.696000] stack_dump: 0x80200960: 00000055 ffffffff 7474754e 00000058 802009f8 00000000 80018148 00000000
[    6.696000] stack_dump: 0x80200980: 0000000f 00000000 80016454 00000000 deadbeef deadbeef 00000006 2e323100
[    6.696000] stack_dump: 0x802009a0: 2d302e33 00304352 00000046 deadbeef 66366ce2 32663738 69642d36 20797472
[    6.696000] stack_dump: 0x802009c0: 2074634f 32203131 20333230 303a3331 36323a36 ffffff00 8020af70 00000000
[    6.696000] stack_dump: 0x802009e0: 00000007 73697200 00762d63 00000000 80200058 00000000 80200a30 00000000
[    6.696000] stack_dump: 0x80200a00: 80800000 00000000 000ffffe 00000000 7fefffff ffffffff 00100000 00000000
[    6.696000] stack_dump: 0x80200a20: 00400000 00000000 802099f0 00000000 7fc00000 ffffffff c0562000 00000000
[    6.696000] stack_dump: 0x80200a40: 7ff00000 ffffffff 8020af70 00000000 00000007 00000000 80001206 00000000
[    6.696000] stack_dump: 0x80200a60: c0100000 00000000 80201898 00000000 8020af70 00000000 80000cb0 00000000
[    6.696000] stack_dump: 0x80200a80: 8020aff0 00000000 00000001 00000000 00000007 00000000 800009c2 00000000
[    6.696000] dump_stack: Kernel Stack:
[    6.696000] dump_stack:   base: 0x802088e0
[    6.696000] dump_stack:   size: 00003072
[    6.696000] dump_stack: User Stack:
[    6.696000] dump_stack:   base: 0xc0202040
[    6.696000] dump_stack:   size: 00003008
[    6.696000] stack_dump: 0xc02028a0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc02028c0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc02028e0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202900: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202920: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202940: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202960: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202980: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc02029a0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc02029c0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc02029e0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202a00: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202a20: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202a40: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202a60: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202a80: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202aa0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202ac0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202ae0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202b00: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202b20: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202b40: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202b60: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202b80: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202ba0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202bc0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] stack_dump: 0xc0202be0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    6.696000] dump_tasks:    PID GROUP PRI POLICY   TYPE    NPX STATE   EVENT      SIGMASK          STACKBASE  STACKSIZE      USED   FILLED    COMMAND
[    6.696000] dump_tasks:   ----   --- --- -------- ------- --- ------- ---------- ---------------- 0x802002b0      2048      2040    99.6%!   irq
[    6.696000] dump_task:       0     0   0 FIFO     Kthread N-- Ready              0000000000000000 0x80206010      3056      1536    50.2%    Idle_Task
[    6.696000] dump_task:       1     1 100 RR       Kthread --- Waiting Semaphore  0000000000000000 0x8020a050      1968       720    36.5%    lpwork 0x802015f0 0x80201618
[    6.696000] dump_task:       2     2 100 RR       Task    --- Waiting Semaphore  0000000000000000 0xc0202040      3008       848    28.1%    /system/bin/init
[    6.696000] dump_task:       3     3 100 RR       Task    --- Running            0000000000000000 0xc0202030      2000       600    30.0%    hello

*/
