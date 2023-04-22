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
#include <fcntl.h>
#include <assert.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * hello_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  printf("Hello, World!!\n");

  // Open /dev/ttyS1 (UART3)
  int fd = open("/dev/ttyS1", O_RDWR);
  printf("Open /dev/ttyS1: fd=%d\n", fd);
  assert(fd > 0);

  // Repeat 5 times: Write command and read response
  for (int i = 0; i < 5; i++)
    {
      // Write command
      const char cmd[] = "AT\r";
      ssize_t nbytes = write(fd, cmd, sizeof(cmd));
      printf("Write command: nbytes=%ld\n", nbytes);
      assert(nbytes == sizeof(cmd));

      // Read response
      static char buf[1024];
      nbytes = read(fd, buf, sizeof(buf) - 1);
      if (nbytes >= 0) { buf[nbytes] = 0; }
      printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

      // Wait a while
      sleep(2);
    }

  // Close the device
  close(fd);

  return 0;
}

/* Output Log

Script started on Sat Apr 22 23:18:50 2023
command: screen /dev/tty.usbserial-1410 115200
[?1049h[!p[?3;4l[4l>[4l[?1h=[0m(B[1;66r[H[2J[H[2JDRAM: 2048 MiB
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
346915 bytes read in 20 ms (16.5 MiB/s)
Uncompressed size: 10514432 = 0xA07000
36162 bytes read in 5 ms (6.9 MiB/s)
1078500 bytes read in 50 ms (20.6 MiB/s)
## Flattened Device Tree blob at 4fa00000
   Booting using the fdt blob at 0x4fa00000
   Loading Ramdisk to 49ef8000, end 49fff4e4 ... OK
   Loading Device Tree to 0000000049eec000, end 0000000049ef7d41 ... OK

Starting kernel ...

a64_pio_config: port=3, pin=18, ext=-1, cfgaddr=0x1c20874, value=1, shift=8
a64_pio_config: port=3, pin=19, ext=-1, cfgaddr=0x1c20874, value=1, shift=12
a64_pio_config: port=3, pin=20, ext=-1, cfgaddr=0x1c20874, value=1, shift=16
up_setup: Clear DLAB
up_setup: addr=0x1c28004, before=0x0, after=0x0
up_setup: addr=0x1c28000, before=0xd, after=0xd
up_setup: Configure the FIFOs
arm64_serialinit: Enable clocking to UART3: Set UART3_GATING to High (Pass): addr=0x1c2006c, before=0x0, after=0x80000
arm64_serialinit: Compare with UART0_GATING: addr=0x1c2006c, val=0x10000
arm64_serialinit: Deassert reset for UART3: Set UART3_RST to High: addr=0x1c202d8, before=0x0, after=0x80000
arm64_serialinit: Compare with UART0_RST: addr=0x1c202d8, val=0x10000
a64_pio_config: port=3, pin=0, ext=-1, cfgaddr=0x1c2086c, value=3, shift=0
a64_pio_config: port=3, pin=1, ext=-1, cfgaddr=0x1c2086c, value=3, shift=4
arm64_serialinit: Enable UART3 on PD0: PD0_SELECT: addr=0x1c2086c, before=0x7, after=0x3
arm64_serialinit: Enable UART3 on PD1: PD0_SELECT: addr=0x1c2086c, before=0x70, after=0x30
a64_pio_config: port=3, pin=18, ext=-1, cfgaddr=0x1c20874, value=1, shift=8
a64_pio_config: port=3, pin=19, ext=-1, cfgaddr=0x1c20874, value=1, shift=12
a64_pio_config: port=3, pin=20, ext=-1, cfgaddr=0x1c20874, value=1, shift=16
a64_pio_config: port=8, pin=10, ext=-1, cfgaddr=0x1f02c04, value=2, shift=8
a64_pio_config: port=7, pin=10, ext=2, cfgaddr=0x1c20900, value=1, shift=8
a64_pio_config: port=3, pin=23, ext=-1, cfgaddr=0x1c20874, value=1, shift=28
pinephone_pmic_init: Set DLDO1 Voltage to 3.3V
pmic_write: reg=0x15, val=0x1a
a64_rsb_write: rt_addr=0x2d, reg_addr=0x15, value=0x1a
pmic_clrsetbits: reg=0x12, clr_mask=0x0, set_mask=0x8
a64_rsb_read: rt_addr=0x2d, reg_addr=0x12
a64_rsb_write: rt_addr=0x2d, reg_addr=0x12, value=0xd9
pinephone_pmic_init: Set LDO Voltage to 3.3V
pmic_write: reg=0x91, val=0x1a
a64_rsb_write: rt_addr=0x2d, reg_addr=0x91, value=0x1a
pinephone_pmic_init: Enable LDO mode on GPIO0
pmic_write: reg=0x90, val=0x3
a64_rsb_write: rt_addr=0x2d, reg_addr=0x90, value=0x3
pinephone_pmic_init: Set DLDO2 Voltage to 1.8V
pmic_write: reg=0x16, val=0xb
a64_rsb_write: rt_addr=0x2d, reg_addr=0x16, value=0xb
pmic_clrsetbits: reg=0x12, clr_mask=0x0, set_mask=0x10
a64_rsb_read: rt_addr=0x2d, reg_addr=0x12
a64_rsb_write: rt_addr=0x2d, reg_addr=0x12, value=0xd9
a64_pio_config: port=3, pin=23, ext=-1, cfgaddr=0x1c20874, value=1, shift=28
a64_pio_config: port=7, pin=0, ext=2, cfgaddr=0x1c208fc, value=2, shift=0
a64_pio_config: port=7, pin=1, ext=2, cfgaddr=0x1c208fc, value=2, shift=4
pinephone_modem_init: Configure STATUS (PH9) for Input
a64_pio_config: port=7, pin=9, ext=2, cfgaddr=0x1c20900, value=0, shift=4
pinephone_modem_init: Status=0
pinephone_modem_init: Configure PWR_BAT (PL7) for Output
a64_pio_config: port=8, pin=7, ext=-1, cfgaddr=0x1f02c00, value=1, shift=28
pinephone_modem_init: Set PWR_BAT (PL7) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Configure RESET_N (PC4) for Output
a64_pio_config: port=2, pin=4, ext=-1, cfgaddr=0x1c20848, value=1, shift=16
pinephone_modem_init: Set RESET_N (PC4) to Low
pinephone_modem_init: Status=1
pinephone_modem_init: Configure AP-READY (PH7) for Output
a64_pio_config: port=7, pin=7, ext=2, cfgaddr=0x1c208fc, value=1, shift=28
pinephone_modem_init: Set AP-READY (PH7) to Low to wake up modem
pinephone_modem_init: Status=1
pinephone_modem_init: Configure DTR (PB2) for Output
a64_pio_config: port=1, pin=2, ext=0, cfgaddr=0x1c20824, value=1, shift=8
pinephone_modem_init: Set DTR (PB2) to Low to wake up modem
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 30 ms
pinephone_modem_init: Status=1
pinephone_modem_init: Configure PWRKEY (PB3) for Output
a64_pio_config: port=1, pin=3, ext=0, cfgaddr=0x1c20824, value=1, shift=12
pinephone_modem_init: Set PWRKEY (PB3) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 600 ms
pinephone_modem_init: Status=1
pinephone_modem_init: Set PWRKEY (PB3) to Low
pinephone_modem_init: Status=1
pinephone_modem_init: Configure W_DISABLE (PH8) for Output
a64_pio_config: port=7, pin=8, ext=2, cfgaddr=0x1c20900, value=1, shift=0
pinephone_modem_init: Set W_DISABLE (PH8) to High
pinephone_modem_init: Status=1
a64_pio_config: port=3, pin=5, ext=-1, cfgaddr=0x1c2086c, value=0, shift=20
pinephone_modem_init: CTS=1
pinephone_modem_init: Configure RTS (PD4) for Output
a64_pio_config: port=3, pin=4, ext=-1, cfgaddr=0x1c2086c, value=1, shift=16
pinephone_modem_init: Set RTS (PD4) to Low
pinephone_modem_init: CTS=1
pinephone_modem_init: Status=1
nsh: mkfatfs: command not found

NuttShell (NSH) NuttX-12.0.3
nsh> [Khello
up_setup: Clear DLAB
up_setup: addr=0x1c28c04, before=0x0, after=0x0
up_setup: addr=0x1c28c00, before=0x0, after=0xd
up_setup: Configure the FIFOs
Hello, World!!
Open /dev/ttyS1: fd=3
Write command: nbytes=4
Response: nbytes=7

RDY

Write command: nbytes=4
Response: nbytes=35

+CFUN: 1

+CPIN: NOT INSERTED

Write command: nbytes=4
Response: nbytes=9
AT
OK

Write command: nbytes=4
Response: nbytes=9
AT
OK

Write command: nbytes=4
Response: nbytes=9
AT
OK

nsh> [Khello
up_setup: Clear DLAB
up_setup: addr=0x1c28c04, before=0x0, after=0x0
up_setup: addr=0x1c28c00, before=0xd, after=0xd
up_setup: Configure the FIFOs
Hello, World!!
Open /dev/ttyS1: fd=3
Write command: nbytes=4
Response: nbytes=9
AT
OK

Write command: nbytes=4
Response: nbytes=9
AT
OK

Write command: nbytes=4
Response: nbytes=9
AT
OK

Write command: nbytes=4
Response: nbytes=9
AT
OK

Write command: nbytes=4
Response: nbytes=9
AT
OK

nsh> [K
Script done on Sat Apr 22 23:19:41 2023

*/
