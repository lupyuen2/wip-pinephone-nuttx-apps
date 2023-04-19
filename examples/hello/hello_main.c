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

  // Forever write and read
  for (;;)
    {
      // Write command
      const char cmd[] = "AT\nAT\nAT\r\nAT\r\n";
      ssize_t nbytes = write(fd, cmd, sizeof(cmd));
      printf("Write command: nbytes=%ld\n", nbytes);
      assert(nbytes == sizeof(cmd));

      // Read response
      static char buf[1024];
      nbytes = read(fd, buf, sizeof(buf) - 1);
      if (nbytes >= 0) { buf[nbytes] = 0; }
      printf("Response: nbytes=%ld, buf=%s\n", nbytes, buf);
      for (int i = 0; i < nbytes; i++)
        {
          char c = buf[i];
          printf("[%02x] %c\n", c, c);
        }
    }

  // Close the device
  close(fd);

  return 0;
}

/* Output Log

Script started on Thu Apr 20 07:20:35 2023
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
347109 bytes read in 20 ms (16.6 MiB/s)
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
up_setup: baud_rate=115200
up_setup: Clear fifos
up_serialout: addr=0x1c28008, before=0xc1, after=0x6
up_setup: Set trigger
up_serialout: addr=0x1c28008, before=0x1, after=0x81
up_setup: Set up the IER
up_setup: Enter DLAB=1
up_serialout: addr=0x1c2800c, before=0x3, after=0x83
up_setup: Set the BAUD divisor
up_serialout: addr=0x1c28004, before=0x0, after=0x0
up_serialout: addr=0x1c28000, before=0x0, after=0xd
up_setup: Clear DLAB
up_serialout: addr=0x1c2800c, before=0x3, after=0x3
up_setup: Configure the FIFOs
up_serialout: addr=0x1c28008, before=0xc7, after=0x87
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
a64_pio_config: port=7, pin=9, ext=2, cfgaddr=0x1c20900, value=0, shift=4
pinephone_modem_init: Status=0
pinephone_pmic_usb_init: Set DCDC1 Voltage to 3.3V
pmic_write: reg=0x20, val=0x11
a64_rsb_write: rt_addr=0x2d, reg_addr=0x20, value=0x11
pmic_clrsetbits: reg=0x10, clr_mask=0x0, set_mask=0x1
a64_rsb_read: rt_addr=0x2d, reg_addr=0x10
a64_rsb_write: rt_addr=0x2d, reg_addr=0x10, value=0x37
pinephone_modem_init: Status=0
pinephone_modem_init: Wait 1000 ms
pinephone_modem_init: Status=0
pinephone_modem_init: Configure PWR_BAT (PL7) for Output
a64_pio_config: port=8, pin=7, ext=-1, cfgaddr=0x1f02c00, value=1, shift=28
pinephone_modem_init: Set PWR_BAT (PL7) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 1000 ms
pinephone_modem_init: Status=1
pinephone_modem_init: Configure RESET_N (PC4) for Output
a64_pio_config: port=2, pin=4, ext=-1, cfgaddr=0x1c20848, value=1, shift=16
pinephone_modem_init: Set RESET_N (PC4) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Configure PWRKEY (PB3) for Output
a64_pio_config: port=1, pin=3, ext=0, cfgaddr=0x1c20824, value=1, shift=12
pinephone_modem_init: Set PWRKEY (PB3) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 30 ms for VBAT to be stable
pinephone_modem_init: Status=1
pinephone_modem_init: Set PWRKEY (PB3) to Low
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 500 ms
pinephone_modem_init: Status=1
pinephone_modem_init: Set PWRKEY (PB3) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Configure W_DISABLE (PH8) for Output
a64_pio_config: port=7, pin=8, ext=2, cfgaddr=0x1c20900, value=1, shift=0
pinephone_modem_init: Set W_DISABLE (PH8) to High
pinephone_modem_init: Status=1
a64_pio_config: port=3, pin=18, ext=-1, cfgaddr=0x1c20874, value=1, shift=8
a64_pio_config: port=3, pin=19, ext=-1, cfgaddr=0x1c20874, value=1, shift=12
a64_pio_config: port=3, pin=20, ext=-1, cfgaddr=0x1c20874, value=1, shift=16
pinephone_modem_init: Turn on Green LED
pinephone_modem_init: Status=1
pinephone_modem_init: Turn on Red LED
pinephone_modem_init: Status=1
pinephone_modem_init: Turn on Blue LED
pinephone_modem_init: Status=1
nsh: mkfatfs: command not found

NuttShell (NSH) NuttX-12.0.3
nsh> [Khello
up_setup: baud_rate=115200
up_setup: Clear fifos
up_serialout: addr=0x1c28c08, before=0x1, after=0x6
up_setup: Set trigger
up_serialout: addr=0x1c28c08, before=0x1, after=0x81
up_setup: Set up the IER
up_setup: Enter DLAB=1
up_serialout: addr=0x1c28c0c, before=0x0, after=0x83
up_setup: Set the BAUD divisor
up_serialout: addr=0x1c28c04, before=0x0, after=0x0
up_serialout: addr=0x1c28c00, before=0x0, after=0xd
up_setup: Clear DLAB
up_serialout: addr=0x1c28c0c, before=0x83, after=0x3
up_setup: Configure the FIFOs
up_serialout: addr=0x1c28c08, before=0xc1, after=0x87
Hello, World!!
Open /dev/ttyS1: fd=3
Write command: nbytes=15
Response: nbytes=7, buf=I
[7f] 
[49] I
[08] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�&
[9f] �
[26] &
[04] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�@AH
[b0] �
[40] @
[41] A
[48] H
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�z��
[be] �
[7a] z
[9d] �
[81] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[df] �
[d3] �
[01] 
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[d7] �
[18] 
[89] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�ƒ�
[f3] �
[c6] �
[92] �
[82] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�ކ
[b7] �
[de] �
[86] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=wZ 
[77] w
[5a] Z
[20]  
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=w�
[77] w
[92] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[b6] �
[8b] �
[11] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=o76!
[6f] o
[37] 7
[36] 6
[21] !
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[fe] �
[8c] �
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=[8C
[1b] 
[09]    
[04] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[bf] �
[86] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�@�
[b4] �
[16] 
[40] @
[80] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�Y(
[db] �
[59] Y
[28] (
[05] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=w;
[77] w
[3b] ;
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�e
[ff] �
[65] e
[05] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=ۊ
[db] �
[8a] �
[11] 
[08] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�&
[ff] �
[26] &
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�'@
[9f] �
[27] '
[40] @
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=���
[ff] �
[96] �
[80] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=f!
[66] f
[12] 
[21] !
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[af] �
[a2] �
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[96] �
[16] 
[05] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=-�
[7f] 
[2d] -
[c4] �
[00] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��[6C
[fb] �
[9b] �
[d9] �
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�-@
[ff] �
[2d] -
[40] @
[12] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�m�
[fb] �
[6d] m
[9a] �
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[ba] �
[93] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=7�
[37] 7
[03] 
[8a] �
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��F�
[ff] �
[f7] �
[46] F
[98] �
[12] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[df] �
[06] 
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=O�K�
[4f] O
[bb] �
[4b] K
[90] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�3�
[fb] �
[33] 3
[82] �
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[df] �
[f2] �
[9a] �
[81] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�SS
[ff] �
[53] S
[53] S
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=~ˀ�
[7e] ~
[cb] �
[80] �
[90] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�L`
[db] �
[4c] L
[60] `
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[db] �
[0b] 
[08] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=}��
[7d] }
[92] �
[91] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=;VL
[3b] ;
[56] V
[4c] L
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�6
[ee] �
[36] 6
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��J
[ff] �
[f2] �
[4a] J
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�$
[bf] �
[24] $
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�u��
[ef] �
[75] u
[9a] �
[82] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=}L�
[7d] }
[4c] L
[82] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��Z�
[f7] �
[d3] �
[5a] Z
[80] �
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�M�
[fb] �
[4d] M
[08] 
[80] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=;�"
[3b] ;
[93] �
[22] "
[04] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�%
[d3] �
[25] %
[01] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�H 
[bd] �
[48] H
[20]  
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=}-
[7d] }
[2d] -
[19] 
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�[7C�
[ff] �
[09]    
[81] �
[11] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[9f] �
[05] 
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=����
[ff] �
[fe] �
[86] �
[80] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�%��
[b6] �
[25] %
[82] �
[90] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�mH
[be] �
[6d] m
[48] H
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=ߓ@
[df] �
[93] �
[40] @
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=߲A
[df] �
[b2] �
[41] A
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�Z
[ff] �
[5a] Z
[19] 
[10] 
[00] 
[00] 
Write command: nbytes=15

Script done on Thu Apr 20 07:21:26 2023

*/
