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

// In this include file: Define the Phone Number in International Format
// and in PDU Format (nibbles are flipped)
//   #define PHONE_NUMBER    "+1234567890"
//   #define PHONE_NUMBER_PDU "2143658709"
// If the number of nibbles (half-bytes) is odd, insert "F" into the PDU Phone Number like this:
//   #define PHONE_NUMBER    "+123456789"
//   #define PHONE_NUMBER_PDU "214365870F9"
#include "../../../phone_number.h"

// TODO: Handle CONFIG_A64_UART2 and /dev/ttyS2
#ifndef CONFIG_A64_UART3
#error UART3 should be enabled in menuconfig
#endif

static void send_sms_text(int fd);
static void send_sms_pdu(int fd);
static void dial_number(int fd);

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

  // Repeat 5 times: Write AT command and read response
  for (int i = 0; i < 5; i++)
    {
      // Write command
      const char cmd[] = "AT\r";
      ssize_t nbytes = write(fd, cmd, strlen(cmd));
      printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
      assert(nbytes == strlen(cmd));

      // Read response
      static char buf[1024];
      nbytes = read(fd, buf, sizeof(buf) - 1);
      if (nbytes >= 0) { buf[nbytes] = 0; }
      else { buf[0] = 0; }
      printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

      // Wait a while
      sleep(2);
    }

  // TODO: Enter the PIN for the SIM card:
  //  AT+CPIN=1234
  //  > OK

  // Check the connection status:
  //  AT+CREG?
  //  > CGREG: 0,1
  //  > OK
  // This means the modem is connected to the home network.
  {
    // Write command
    const char cmd[] = "AT+CREG?\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(2);
  }

  // Show operator:
  //  AT+COPS?
  {
    // Write command
    const char cmd[] = "AT+COPS?\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(2);
  }

  // Show SMS Center Address:
  //  AT+CSCA?
  {
    // Write command
    const char cmd[] = "AT+CSCA?\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(2);
  }

  // Check Service Domain, which should be 2
  //  AT+QCFG="servicedomain"
  {
    // Write command
    const char cmd[] = "AT+QCFG=\"servicedomain\"\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(2);
  }

  // Check Network Scan Mode, which should be 0
  //  AT+QCFG="nwscanmode"
  {
    // Write command
    const char cmd[] = "AT+QCFG=\"nwscanmode\"\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(2);
  }

  // Make an Outgoing Phone Call
  dial_number(fd);

  // Send an SMS Message in Text Mode
  send_sms_text(fd);

  // Send an SMS Message in PDU Mode
  send_sms_pdu(fd);

  // Close the device
  close(fd);

  UNUSED(send_sms_text);
  UNUSED(send_sms_pdu);
  UNUSED(dial_number);
  return 0;
}

// Send an SMS Message in Text Mode
static void send_sms_text(int fd)
{
  puts("send_sms_text");

  // Set Message Format to Text Mode
  // AT+CMGF=1
  {
    // Write command
    const char cmd[] = "AT+CMGF=1\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(1);
  }

  // Set Character Set to GSM
  // AT+CSCS="GSM"
  {
    // Write command
    const char cmd[] = "AT+CSCS=\"GSM\"\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(1);
  }

  // Send SMS Text Message, assuming Message Format is Text Mode
  // AT+CMGS="yourphonenumber"\r
  // text is entered
  // <Ctrl+Z>
  {
    // Write command
    const char cmd[] = 
      "AT+CMGS=\""
      PHONE_NUMBER
      "\"\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));
  }
  // Wait for ">"
  for (;;)
    {
      // Read response
      static char buf[1024];
      ssize_t nbytes = read(fd, buf, sizeof(buf) - 1);
      if (nbytes >= 0) { buf[nbytes] = 0; }
      else { buf[0] = 0; }
      printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

      // Stop if we find ">"
      if (strchr(buf, '>') != NULL) { break; }
    }
  {
    // Write message
    const char cmd[] = 
      "Hello from Apache NuttX RTOS on PinePhone! (SMS Text Mode)"
      "\x1A";  // End of Message (Ctrl-Z)
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(1);
  }

  // Clean up the Log: Repeat 5 times: Write AT command and read response
  for (int i = 0; i < 5; i++)
    {
      // Write command
      const char cmd[] = "AT\r";
      ssize_t nbytes = write(fd, cmd, strlen(cmd));
      printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
      assert(nbytes == strlen(cmd));

      // Read response
      static char buf[1024];
      nbytes = read(fd, buf, sizeof(buf) - 1);
      if (nbytes >= 0) { buf[nbytes] = 0; }
      else { buf[0] = 0; }
      printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

      // Wait a while
      sleep(1);
    }
}

// Send an SMS Message in PDU Mode. Based on
// Quectel GSM AT Commands Application Note, Section 9.3.2 "Send SMS in PDU mode", Page 26:
// https://www.cika.com/soporte/Information/GSMmodules/Quectel/AppNotes/Quectel_GSM_ATC_Application_Note.pdf
// ETSI GSM 07.05 Spec (AT Commands): https://www.etsi.org/deliver/etsi_gts/07/0705/05.01.00_60/gsmts_0705v050100p.pdf
// ETSI GSM 03.40 Spec (PDU Format): https://en.wikipedia.org/wiki/GSM_03.40
static void send_sms_pdu(int fd)
{
  puts("send_sms_pdu");

  // Set Message Format to PDU Mode
  // AT+CMGF=0
  {
    // Write command
    const char cmd[] = "AT+CMGF=0\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(1);
  }

  // Send SMS Text Message, assuming Message Format is PDU Mode
  // AT+CMGS="yourphonenumber"\r
  // text is entered
  // <Ctrl+Z>
  {
    // Write command
    const char cmd[] = 
      "AT+CMGS="
      "41"  // TODO: PDU Length in bytes, excluding the Length of SMSC
      "\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));
  }
  // Wait for ">"
  for (;;)
    {
      // Read response
      static char buf[1024];
      ssize_t nbytes = read(fd, buf, sizeof(buf) - 1);
      if (nbytes >= 0) { buf[nbytes] = 0; }
      else { buf[0] = 0; }
      printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

      // Stop if we find ">"
      if (strchr(buf, '>') != NULL) { break; }
    }
  {
    // Write message
    const char cmd[] = 
      "00"  // Length of SMSC information (None)
      "11"  // SMS-SUBMIT message
      "00"  // TP-Message-Reference: 00 to let the phone set the message reference number itself
      "0A"  // TODO: Address-Length: Length of phone number (Number of Decimal Digits in Phone Number)
      "91"  // Type-of-Address: 91 for International Format of phone number
      PHONE_NUMBER_PDU  // TODO: Phone Number in PDU Format
      "00"  // TP-PID: Protocol identifier
      "08"  // TP-DCS: Data coding scheme
      "01"  // TP-Validity-Period
      "1C"  // TP-User-Data-Length: Length of Encoded Message Text in bytes
      // TP-User-Data: Encoded Message Text "Hello,Quectel!"
      "00480065006C006C006F002C005100750065006300740065006C0021"
      "\x1A";  // End of Message (Ctrl-Z)
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(1);
  }

  // Clean up the Log: Repeat 5 times: Write AT command and read response
  for (int i = 0; i < 5; i++)
    {
      // Write command
      const char cmd[] = "AT\r";
      ssize_t nbytes = write(fd, cmd, strlen(cmd));
      printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
      assert(nbytes == strlen(cmd));

      // Read response
      static char buf[1024];
      nbytes = read(fd, buf, sizeof(buf) - 1);
      if (nbytes >= 0) { buf[nbytes] = 0; }
      else { buf[0] = 0; }
      printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

      // Wait a while
      sleep(1);
    }
}

// Make an Outgoing Phone Call
static void dial_number(int fd)
{
  puts("dial_number");

  // Digital Audio Interface Configuration: Query the range
  //  AT+QDAI=?
  {
    // Write command
    const char cmd[] = "AT+QDAI=?\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(1);
  }

  // Digital Audio Interface Configuration: Query the current interface configuration
  //  AT+QDAI?
  {
    // Write command
    const char cmd[] = "AT+QDAI?\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(1);
  }

  // Issue a call:
  //  ATD1711;
  //  > OK
  {
    // Write command
    const char cmd[] = 
      "ATD"
      PHONE_NUMBER
      ";\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(20);
  }

  // Hang up:
  //  ATH
  //  > OK
  {
    // Write command
    const char cmd[] = "ATH\r";
    ssize_t nbytes = write(fd, cmd, strlen(cmd));
    printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
    assert(nbytes == strlen(cmd));

    // Read response
    static char buf[1024];
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes >= 0) { buf[nbytes] = 0; }
    else { buf[0] = 0; }
    printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

    // Wait a while
    sleep(1);
  }
  // Clean up the Log: Repeat 5 times: Write AT command and read response
  for (int i = 0; i < 5; i++)
    {
      // Write command
      const char cmd[] = "AT\r";
      ssize_t nbytes = write(fd, cmd, strlen(cmd));
      printf("Write command: nbytes=%ld\n%s\n", nbytes, cmd);
      assert(nbytes == strlen(cmd));

      // Read response
      static char buf[1024];
      nbytes = read(fd, buf, sizeof(buf) - 1);
      if (nbytes >= 0) { buf[nbytes] = 0; }
      else { buf[0] = 0; }
      printf("Response: nbytes=%ld\n%s\n", nbytes, buf);

      // Wait a while
      sleep(1);
    }
}

/* Output Log

Script started on Thu May  4 19:04:27 2023
command: screen /dev/tty.usbserial-1410 115200
[?1049h[!p[?3;4l[4l>[4l[?1h=[0m(B[1;64r[H[2J[H[2JDRAM: 2048 MiB
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
350545 bytes read in 20 ms (16.7 MiB/s)
Uncompressed size: 10539008 = 0xA0D000
36162 bytes read in 4 ms (8.6 MiB/s)
1078500 bytes read in 51 ms (20.2 MiB/s)
## Flattened Device Tree blob at 4fa00000
   Booting using the fdt blob at 0x4fa00000
   Loading Ramdisk to 49ef8000, end 49fff4e4 ... OK
   Loading Device Tree to 0000000049eec000, end 0000000049ef7d41 ... OK

Starting kernel ...

up_setup: Clear DLAB
up_setup: addr=0x1c28004, before=0x0, after=0x0
up_setup: addr=0x1c28000, before=0xd, after=0xd
up_setup: Configure the FIFOs
arm64_serialinit: Enable clocking to UART3: Set UART3_GATING to High (Pass): addr=0x1c2006c, before=0x0, after=0x80000
arm64_serialinit: Compare with UART0_GATING: addr=0x1c2006c, val=0x10000
arm64_serialinit: Deassert reset for UART3: Set UART3_RST to High: addr=0x1c202d8, before=0x0, after=0x80000
arm64_serialinit: Compare with UART0_RST: addr=0x1c202d8, val=0x10000
arm64_serialinit: Enable UART3 on PD0: PD0_SELECT: addr=0x1c2086c, before=0x7, after=0x3
arm64_serialinit: Enable UART3 on PD1: PD0_SELECT: addr=0x1c2086c, before=0x70, after=0x30
pinephone_modem_init: Configure STATUS (PH9) for Input
pinephone_modem_init: Status=0
pinephone_modem_init: Configure PWR_BAT (PL7) for Output
pinephone_modem_init: Set PWR_BAT (PL7) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Configure RESET_N (PC4) for Output
pinephone_modem_init: Set RESET_N (PC4) to Low
pinephone_modem_init: Status=1
pinephone_modem_init: Configure AP-READY (PH7) for Output
pinephone_modem_init: Set AP-READY (PH7) to Low to wake up modem
pinephone_modem_init: Status=1
pinephone_modem_init: Configure DTR (PB2) for Output
pinephone_modem_init: Set DTR (PB2) to Low to wake up modem
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 30 ms
pinephone_modem_init: Status=1
pinephone_modem_init: Configure PWRKEY (PB3) for Output
pinephone_modem_init: Set PWRKEY (PB3) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Wait 600 ms
pinephone_modem_init: Status=1
pinephone_modem_init: Set PWRKEY (PB3) to Low
pinephone_modem_init: Status=1
pinephone_modem_init: Configure W_DISABLE (PH8) for Output
pinephone_modem_init: Set W_DISABLE (PH8) to High
pinephone_modem_init: Status=1
pinephone_modem_init: Status=1
pinephone_modem_init: Status=1
pinephone_modem_init: Status=0
pinephone_modem_init: CTS=1
pinephone_modem_init: Configure RTS (PD4) for Output
pinephone_modem_init: Set RTS (PD4) to Low
pinephone_modem_init: CTS=1
pinephone_modem_init: Status=0
nsh: mkfatfs: command not found

NuttShell (NSH) NuttX-12.0.3
nsh> [Khello
up_setup: Clear DLAB
up_setup: addr=0x1c28c04, before=0x0, after=0x0
up_setup: addr=0x1c28c00, before=0x0, after=0xd
up_setup: Configure the FIFOs
Hello, World!!
Open /dev/ttyS1: fd=3
Write command: nbytes=3
AT
Response: nbytes=7

RDY

Write command: nbytes=3
AT
Response: nbytes=12

+CFUN: 1

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=38
AT
OK

+CPIN: READY

+QUSIM: 1

Write command: nbytes=9
AT+CREG?
Response: nbytes=28
AT
OK

+QIND: SMS DONE

Write command: nbytes=9
AT+COPS?
Response: nbytes=29
AT+CREG?
+CREG: 0,2

OK

Write command: nbytes=9
AT+CSCA?
Response: nbytes=58
AT+COPS?
+COPS: 0,0,"SGP-M1",2

OK

+QIND: PB DONE

Write command: nbytes=24
AT+QCFG="servicedomain"
Response: nbytes=43
AT+CSCA?
+CSCA: "+6596845999",145

OK

Write command: nbytes=21
AT+QCFG="nwscanmode"
Response: nbytes=58
AT+QCFG="servicedomain"
+QCFG: "servicedomain",2

OK

dial_number
Write command: nbytes=10
AT+QDAI=?
Response: nbytes=52
AT+QCFG="nwscanmode"
+QCFG: "nwscanmode",0

OK

Write command: nbytes=9
AT+QDAI?
Response: nbytes=71
AT+QDAI=?
+QDAI: (1-4),(0,1),(0,1),(0-5),(0-2),(0,1)(1)(1-16)

OK

Write command: nbytes=16
ATDyourphonenumber;
Response: nbytes=41
AT+QDAI?
+QDAI: 1,1,0,1,0,0,1,1

OK

Write command: nbytes=4
ATH
Response: nbytes=36
ATDyourphonenumber;
OK

NO CARRIER

Write command: nbytes=3
AT
Response: nbytes=10
ATH
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

send_sms_text
Write command: nbytes=10
AT+CMGF=1
Response: nbytes=9
AT
OK

Write command: nbytes=14
AT+CSCS="GSM"
Response: nbytes=16
AT+CMGF=1
OK

Write command: nbytes=22
AT+CMGS="yourphonenumber"
Response: nbytes=20
AT+CSCS="GSM"
OK

Response: nbytes=26
AT+CMGS="yourphonenumber"
> 
Write command: nbytes=59
Hello from Apache NuttX RTOS on PinePhone! (SMS Text Mode)
Response: nbytes=32
Hello from Apache NuttX RTOS on 
Write command: nbytes=3
AT
Response: nbytes=45
PinePhone! (SMS Text Mode)
+CMGS: 37

OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

send_sms_pdu
Write command: nbytes=10
AT+CMGF=0
Response: nbytes=9
AT
OK

Write command: nbytes=11
AT+CMGS=41
Response: nbytes=16
AT+CMGF=0
OK

Response: nbytes=15
AT+CMGS=41
> 
Write command: nbytes=85
0011000A91yourphonenumberpdu0008011C00480065006C006C006F002C005100750065006300740065006C0021
Response: nbytes=32
0011000A91yourphonenumberpdu0008011C0048
Write command: nbytes=3
AT
Response: nbytes=71
0065006C006C006F002C005100750065006300740065006C0021
+CMGS: 38

OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
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
Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=9
AT+CREG?
Response: nbytes=9
AT
OK

Write command: nbytes=9
AT+COPS?
Response: nbytes=29
AT+CREG?
+CREG: 0,1

OK

Write command: nbytes=9
AT+CSCA?
Response: nbytes=40
AT+COPS?
+COPS: 0,0,"SGP-M1",7

OK

Write command: nbytes=24
AT+QCFG="servicedomain"
Response: nbytes=43
AT+CSCA?
+CSCA: "+6596845999",145

OK

Write command: nbytes=21
AT+QCFG="nwscanmode"
Response: nbytes=58
AT+QCFG="servicedomain"
+QCFG: "servicedomain",2

OK

dial_number
Write command: nbytes=10
AT+QDAI=?
Response: nbytes=52
AT+QCFG="nwscanmode"
+QCFG: "nwscanmode",0

OK

Write command: nbytes=9
AT+QDAI?
Response: nbytes=71
AT+QDAI=?
+QDAI: (1-4),(0,1),(0,1),(0-5),(0-2),(0,1)(1)(1-16)

OK

Write command: nbytes=16
ATDyourphonenumber;
Response: nbytes=41
AT+QDAI?
+QDAI: 1,1,0,1,0,0,1,1

OK

Write command: nbytes=4
ATH
Response: nbytes=36
ATDyourphonenumber;
OK

NO CARRIER

Write command: nbytes=3
AT
Response: nbytes=10
ATH
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

send_sms_text
Write command: nbytes=10
AT+CMGF=1
Response: nbytes=9
AT
OK

Write command: nbytes=14
AT+CSCS="GSM"
Response: nbytes=16
AT+CMGF=1
OK

Write command: nbytes=22
AT+CMGS="yourphonenumber"
Response: nbytes=20
AT+CSCS="GSM"
OK

Response: nbytes=26
AT+CMGS="yourphonenumber"
> 
Write command: nbytes=59
Hello from Apache NuttX RTOS on PinePhone! (SMS Text Mode)
Response: nbytes=32
Hello from Apache NuttX RTOS on 
Write command: nbytes=3
AT
Response: nbytes=45
PinePhone! (SMS Text Mode)
+CMGS: 39

OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

send_sms_pdu
Write command: nbytes=10
AT+CMGF=0
Response: nbytes=9
AT
OK

Write command: nbytes=11
AT+CMGS=41
Response: nbytes=16
AT+CMGF=0
OK

Response: nbytes=15
AT+CMGS=41
> 
Write command: nbytes=85
0011000A91yourphonenumberpdu0008011C00480065006C006C006F002C005100750065006300740065006C0021
Response: nbytes=32
0011000A91yourphonenumberpdu0008011C0048
Write command: nbytes=3
AT
Response: nbytes=71
0065006C006C006F002C005100750065006300740065006C0021
+CMGS: 40

OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

Write command: nbytes=3
AT
Response: nbytes=9
AT
OK

nsh> [K
Script done on Thu May  4 19:07:50 2023

*/
