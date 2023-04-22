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

Script started on Sat Apr 22 16:57:40 2023
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
347012 bytes read in 20 ms (16.5 MiB/s)
Uncompressed size: 10514432 = 0xA07000
36162 bytes read in 4 ms (8.6 MiB/s)
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
pinephone_pmic_usb_init: Set DCDC1 Voltage to 3.3V
pmic_write: reg=0x20, val=0x11
a64_rsb_write: rt_addr=0x2d, reg_addr=0x20, value=0x11
pmic_clrsetbits: reg=0x10, clr_mask=0x0, set_mask=0x1
a64_rsb_read: rt_addr=0x2d, reg_addr=0x10
a64_rsb_write: rt_addr=0x2d, reg_addr=0x10, value=0x37
pinephone_modem_init: Status=0
pinephone_modem_init: Wait 2000 ms
pinephone_modem_init: Status=0
pinephone_modem_init: Configure PWR_BAT (PL7) for Output
a64_pio_config: port=8, pin=7, ext=-1, cfgaddr=0x1f02c00, value=1, shift=28
pinephone_modem_init: Set PWR_BAT (PL7) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 2000 ms
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
pinephone_modem_init: Wait 2000 ms for VBAT to be stable
pinephone_modem_init: Status=1
pinephone_modem_init: Set PWRKEY (PB3) to Low
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 2000 ms
pinephone_modem_init: Status=1
pinephone_modem_init: Set PWRKEY (PB3) to High
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
Write command: nbytes=15
Response: nbytes=6, buf==֕�
[d6] �
[95] �
[95] �
[00] 
[12] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�6Q
[bb] �
[36] 6
[51] Q
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=﫫
[ef] �
[ab] �
[ab] �
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[ae] �
[14] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��S
[df] �
[d7] �
[53] S
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=5
[7f] 
[35] 5
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[cb] �
[0b] 
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=����
[ff] �
[9a] �
[92] �
[82] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�UE
[b6] �
[55] U
[45] E
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[e9] �
[1a] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=WQ
[57] W
[51] Q
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�V
[df] �
[56] V
[08] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=ߔ
[df] �
[94] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[fb] �
[16] 
[08] 
[08] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=/)I@
[2f] /
[29] )
[49] I
[40] @
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[ab] �
[84] �
[04] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=o"
[6f] o
[22] "
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=k� 
[6b] k
[a6] �
[20]  
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=7[7C
[37] 7
[15] 
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=� 
[fb] �
[b6] �
[20]  
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[fb] �
[9b] �
[95] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�+
[ff] �
[2b] +
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=_)
[5f] _
[29] )
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[b6] �
[12] 
[12] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[bf] �
[9a] �
[12] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=ߋ
[df] �
[8b] �
[12] 
[00] 
[02] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=$A
[24] $
[41] A
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=]�
[5d] ]
[94] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�
[bf] �
[0b] 
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[af] �
[94] �
[04] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
�
[b2] �
[0a] 

[92] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�ב
[df] �
[d7] �
[91] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�n
[db] �
[6e] n
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��

[bf] �
[81] �
[0a] 

[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�S�
[d3] �
[53] S
[82] �
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[fa] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�"
[d7] �
[22] "
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=���
[bf] �
[bb] �
[85] �
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[a2] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�"�
[bf] �
[22] "
[82] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=_�
[5f] _
[83] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�R@
[bf] �
[52] R
[04] 
[40] @
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��S
[df] �
[d7] �
[53] S
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�[7C
[b6] �
[04] 
[09]    
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��K[5C
[ff] �
[d9] �
[4b] K
[09]    
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=���
[be] �
[8a] �
[80] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��[6C
[ff] �
[ab] �
[09]    
[01] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=���
[9a] �
[93] �
[82] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[94] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�:�
[df] �
[3a] :
[01] 
[80] �
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�ߒ
[db] �
[df] �
[92] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[96] �
[08] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[cb] �
[12] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=v2
[76] v
[32] 2
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[da] �
[12] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�(
[d3] �
[28] (
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[7f] 
[fb] �
[da] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�"
[bb] �
[22] "
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=)I
[29] )
[49] I
[02] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=K 
[7f] 
[4b] K
[20]  
[04] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�K
[d7] �
[4b] K
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=A
[16] 
[41] A
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=R
[52] R
[04] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=� 
[b2] �
[20]  
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[7f] 
[bb] �
[82] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=��
[b6] �
[92] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=��
[b6] �
[13] 
[81] �
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[b7] �
[00] 
[20]  
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf==ۚ
[db] �
[9a] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=nA
[6e] n
[41] A
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=k�
[6b] k
[b4] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[ef] �
[aa] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�[H
[db] �
[5b] [
[48] H
[02] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=כ�
[d7] �
[9b] �
[11] 
[82] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[b2] �
[05] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�

[fa] �
[8a] �
[0a] 

[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[96] �
[84] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[a9] �
[11] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[95] �
[14] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[8C
[bb] �
[0a] 

[09]    
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=K)
[4b] K
[29] )
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=Z)
[5a] Z
[29] )
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=߉
[df] �
[89] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[fb] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�o[6C
[db] �
[6f] o
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�$
[de] �
[24] $
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=7[7C
[37] 7
[09]    
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[9a] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�G
[ff] �
[47] G
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[af] �
[fb] �
[04] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=���
[b3] �
[92] �
[92] �
[02] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�+
[9f] �
[2b] +
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=ךJ
[d7] �
[9a] �
[4a] J
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=
[05] 
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�Z
[ff] �
[5a] Z
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=Β
[ce] �
[92] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[d7] �
[08] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�@
[ff] �
[12] 
[40] @
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[92] �
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[96] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[b6] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=Vi
[56] V
[69] i
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�(
[d6] �
[28] (
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[d3] �
[0b] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�B
[d6] �
[42] B
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[92] �
[a5] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�I�
[df] �
[49] I
[80] �
[02] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=s
[73] s
[12] 
[01] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=&
[26] &
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=U
[55] U
[14] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�K�
[df] �
[4b] K
[90] �
[02] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[96] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[bf] �
[9b] �
[10] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=T
[54] T
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=c�
[63] c
[8b] �
[12] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[96] �
[0b] 
[01] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=
[04] 
[08] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[b7] �
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=���
[ff] �
[d7] �
[f2] �
[82] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf="
[22] "
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��

[ff] �
[ab] �
[0a] 

[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�$
[bf] �
[24] $
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=OA
[4f] O
[41] A
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��S
[fb] �
[de] �
[53] S
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=IO
[49] I
[4f] O
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�V[6C
[df] �
[56] V
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=
[16] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=wI[6C
[77] w
[49] I
[09]    
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=��
[fb] �
[ca] �
[12] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[9b] �
[00] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�{)
[ef] �
[7b] {
[29] )
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[92] �
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�(@
[b6] �
[28] (
[40] @
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[cb] �
[06] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[9b] �
[08] 
[01] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=v
[76] v
[00] 
[02] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[93] �
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�(
[fb] �
[b6] �
[28] (
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=}�
[7d] }
[dd] �
[13] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=��
[b2] �
[80] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[aa] �
[12] 
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=[�

[5b] [
[99] �
[0a] 

[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[9f] �
[08] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=%
[25] %
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=I
[49] I
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[b2] �
[88] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[ff] �
[0e] 
[06] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=?
[3f] ?
[11] 
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=
[7f] 
[0d] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[e7] �
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=~�
[7e] ~
[92] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[fe] �
[f0] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=e[7C
[65] e
[09]    
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[9b] �
[97] �
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�2
[96] �
[32] 2
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=2
[32] 2
[04] 
[02] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[ae] �
[06] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[ff] �
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[1b] 
[92] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[ff] �
[96] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[be] �
[97] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[81] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[b6] �
[0e] 
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=
[05] 
[18] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�R
[fd] �
[a3] �
[52] R
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[fb] �
[89] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=w��
[77] w
[aa] �
[82] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=k�
[6b] k
[ca] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[fe] �
[93] �
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�2
[fb] �
[32] 2
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
Response: nbytes=5, buf=�
[df] �
[13] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=
[12] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=7-A
[37] 7
[2d] -
[41] A
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=
[7f] 
[02] 
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf={

[7b] {
[0a] 

[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[df] �
[12] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=��
[9b] �
[92] �
[02] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=ߛ�
[df] �
[9b] �
[88] �
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�7�
[97] �
[37] 7
[9a] �
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[9a] �
[16] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�
[9b] �
[19] 
[02] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=4, buf=�
[b7] �
[05] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[9f] �
[c6] �
[08] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��  
[fb] �
[db] �
[20]  
[20]  
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=�\
[ff] �
[5c] \
[00] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[b7] �
[92] �
[10] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=5, buf=��
[ff] �
[9b] �
[01] 
[00] 
[00] 
Write command: nbytes=15
Response: nbytes=6, buf=�+
[ff] �
[2b] +
[01] 
[00] 
[00] 
[00] 
Write command: nbytes=15

Script done on Sat Apr 22 16:59:10 2023

*/
