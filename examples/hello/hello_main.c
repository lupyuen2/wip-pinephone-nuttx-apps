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

Script started on Fri Apr 21 16:14:50 2023
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
346993 bytes read in 20 ms (16.5 MiB/s)
Uncompressed size: 10514432 = 0xA07000
36162 bytes read in 4 ms (8.6 MiB/s)
1078500 bytes read in 51 ms (20.2 MiB/s)
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
pinephone_modem_init: Configure DTR (PB2) for Output
a64_pio_config: port=1, pin=2, ext=0, cfgaddr=0x1c20824, value=1, shift=8
pinephone_modem_init: Set DTR (PB2) to Low to wake up modem
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
a64_pio_config: port=3, pin=5, ext=-1, cfgaddr=0x1c2086c, value=0, shift=20
pinephone_modem_init: CTS=0
pinephone_modem_init: Configure RTS (PD4) for Output
a64_pio_config: port=3, pin=4, ext=-1, cfgaddr=0x1c2086c, value=1, shift=16
pinephone_modem_init: Set RTS (PD4) to Low
pinephone_modem_init: CTS=0
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
Write command: nbytes=15
Response: nbytes=7, buf=�
[f7] �
[8b] �
[8b] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[9d] �
[8b] �
[01] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�Q�
[df] �
[51] Q
[92] �
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=�׻�
[ff] �
[d7] �
[bb] �
[83] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�ב
[df] �
[d7] �
[91] �
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=WV
[57] W
[56] V
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[9f] �
[aa] �
[02] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�Q
[d5] �
[51] Q
[11] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=���
[df] �
[d7] �
[d1] �
[10] 
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=���
[ff] �
[bb] �
[9b] �
[10] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf==ۛT
[db] �
[9b] �
[54] T
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=}w�
[7d] }
[77] w
[95] �
[00] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�/�
[fb] �
[2f] /
[8b] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�
[ef] �
[0d] 
[05] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=�VS
[ff] �
[56] V
[53] S
[02] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=]I
[5d] ]
[49] I
[04] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��*
[bd] �
[ad] �
[2a] *
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=[U
[5b] [
[55] U
[05] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=щ
[d1] �
[89] �
[13] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=7
[37] 7
[0d] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=V!
[7f] 
[56] V
[21] !
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=���
[96] �
[df] �
[0b] 
[82] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=]UA
[5d] ]
[55] U
[41] A
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=��Y
[d7] �
[df] �
[59] Y
[10] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�

[bf] �
[0a] 

[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[ff] �
[af] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=[�
[5b] [
[83] �
[01] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�]i
[ff] �
[5d] ]
[69] i
[0d] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[bb] �
[d1] �
[03] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�K
[af] �
[4b] K
[02] 
[12] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf='+�
[27] '
[2b] +
[89] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[ff] �
[af] �
[15] 
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=W�
[57] W
[92] �
[01] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=���
[bb] �
[97] �
[83] �
[11] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��[6C
[9f] �
[8b] �
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf==۪��
[db] �
[aa] �
[8b] �
[80] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=W�
[57] W
[11] 
[8a] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=_]
[5f] _
[5d] ]
[13] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=WE
[57] W
[45] E
[05] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�2
[d7] �
[32] 2
[04] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[ff] �
[95] �
[10] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�[
[df] �
[5b] [
[12] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�I
[b7] �
[49] I
[11] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=׿
�
[d7] �
[bf] �
[0a] 

[82] �
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=_R
[5f] _
[52] R
[04] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[ff] �
[f7] �
[89] �
[11] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=Օ

[d5] �
[95] �
[0a] 

[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��Q
[df] �
[d7] �
[51] Q
[11] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[bd] �
[89] �
[08] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[bf] �
[d5] �
[11] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=׵
[d7] �
[b5] �
[05] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=߿U�
[df] �
[bf] �
[55] U
[82] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�UE
[ff] �
[55] U
[45] E
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=���
[9f] �
[9f] �
[8a] �
[00] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=���
[bb] �
[92] �
[81] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=[8C
[16] 
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�[.
[ff] �
[5b] [
[2e] .
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[fb] �
[bb] �
[8b] �
[12] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�k[6C
[7f] 
[ad] �
[6b] k
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�E�
[ad] �
[45] E
[82] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�[R

[df] �
[5b] [
[52] R
[0a] 

[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�S
[fb] �
[53] S
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�t[6C
[f7] �
[74] t
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=ۿ*
[db] �
[bf] �
[2a] *
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[af] �
[0b] 
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�S6
[df] �
[53] S
[36] 6
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�oI
[ff] �
[6f] o
[49] I
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[bf] �
[91] �
[13] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�U
[bf] �
[55] U
[05] 
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[b6] �
[a2] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�[
[db] �
[5b] [
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=}�
[7d] }
[89] �
[02] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��[6C
[bb] �
[99] �
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=_J�
[5f] _
[4a] J
[90] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=QM
[51] Q
[4d] M
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�U
[df] �
[55] U
[05] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[df] �
[d7] �
[11] 
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�W
[db] �
[57] W
[01] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�ED
[95] �
[45] E
[44] D
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�Ր
[da] �
[d5] �
[90] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=_q@
[5f] _
[71] q
[40] @
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[df] �
[d3] �
[10] 
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�S
[d7] �
[53] S
[12] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=߿
[df] �
[bf] �
[02] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=[
[5b] [
[12] 
[04] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[af] �
[15] 
[04] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�
[fb] �
[bb] �
[81] �
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=_
[5f] _
[0b] 
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�S
[fb] �
[53] S
[11] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf==۫
[db] �
[ab] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[bb] �
[91] �
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=Z
[5a] Z
[10] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=���
[ff] �
[8b] �
[92] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�

[f7] �
[0d] 
[0a] 

[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��[6C
[bf] �
[d3] �
[09]    
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[db] �
[13] 
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�ߑ
[fb] �
[df] �
[91] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�"
[d7] �
[22] "
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=_�
[5f] _
[95] �
[1a] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�[i
[ff] �
[5b] [
[69] i
[05] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=_E�
[5f] _
[45] E
[11] 
[80] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�Q
[df] �
[51] Q
[11] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=��
[ff] �
[b5] �
[04] 
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��"
[bf] �
[d7] �
[22] "
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�

[fb] �
[02] 
[0a] 

[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=ߕ�
[df] �
[95] �
[81] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�A
[8b] �
[41] A
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=oM
[6f] o
[4d] M
[08] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=�[)@
[ff] �
[5b] [
[29] )
[40] @
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=}W

[7d] }
[57] W
[0a] 

[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=8, buf=[�+
[5b] [
[ac] �
[2b] +
[08] 
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=ە�
[db] �
[95] �
[90] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=[�
[5b] [
[91] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�A
[fb] �
[0b] 
[41] A
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=[

[5b] [
[0a] 

[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�$
[96] �
[24] $
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�[7C
[d3] �
[09]    
[12] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=+�
[2b] +
[8b] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=k�
[6b] k
[a2] �
[04] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=���
[bf] �
[af] �
[91] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[bb] �
[8a] �
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[9f] �
[93] �
[12] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�ߑ
[ff] �
[df] �
[91] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=7, buf=_W

[5f] _
[57] W
[0a] 

[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�


[db] �
[0a] 

[0a] 

[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[af] �
[a5] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=[
A
[5b] [
[0d] 
[41] A
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[7f] 
[95] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�WR
[ff] �
[57] W
[52] R
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15

Script done on Fri Apr 21 16:16:00 2023

*/
