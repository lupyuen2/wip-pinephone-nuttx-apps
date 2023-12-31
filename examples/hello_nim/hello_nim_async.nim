## Import NuttX Functions and Macros from C.
## Based on https://github.com/nim-lang/Nim/blob/devel/lib/std/syncio.nim
proc c_open(filename: cstring, mode: cint): cint {.
  importc: "open", header: "<fcntl.h>".}
proc c_close(fd: cint): cint {.
  importc: "close", header: "<fcntl.h>",
  discardable.}
proc c_ioctl(fd: cint, request: cint): cint {.
  importc: "ioctl", header: "<sys/ioctl.h>",
  varargs.}
proc c_usleep(usec: cuint): cint {.
  importc: "usleep", header: "<unistd.h>",
  discardable.}
var O_WRONLY {.
  importc: "O_WRONLY", header: "<fcntl.h>".}: cint
var ULEDIOC_SETALL {.
  importc: "ULEDIOC_SETALL", header: "<nuttx/leds/userled.h>".}: cint

## Blink the LED
proc blink_led() =

  ## Open the LED Driver
  echo "Opening /dev/userleds"
  let fd = c_open("/dev/userleds", O_WRONLY)
  if fd < 0:
    echo "Failed to open /dev/userleds"
    return

  ## On Return: Close the LED Driver
  defer: c_close(fd)

  ## Turn on LED
  echo "Set LED 0 to 1"
  var ret = c_ioctl(fd, ULEDIOC_SETALL, 1)
  if ret < 0:
    echo "ioctl(ULEDIOC_SETALL) failed"
    return

  ## Wait a second (literally)
  echo "Waiting..."
  c_usleep(1000_000)

  ## Turn off LED
  echo "Set LED 0 to 0"
  ret = c_ioctl(fd, ULEDIOC_SETALL, 0)
  if ret < 0:
    echo "ioctl(ULEDIOC_SETALL) failed"
    return

  ## Wait again
  echo "Waiting..."
  c_usleep(1000_000)

## Main Function in Nim
proc hello_nim() {.exportc, cdecl.} =

  ## On Return: Force the Garbage Collection
  defer: GC_runOrc()

  ## Print something
  echo "Hello Nim!"

  ## Blink the LED 20 times
  for loop in 0..19:
    blink_led()

## To test the compilation for NuttX:
# export TOPDIR=/workspaces/bookworm/nuttx
# cd /workspaces/bookworm/apps/examples/hello_nim
# nim c --header hello_nim_async.nim 

## Output Log:
# nsh> hello_nim
# Hello Nim!
# Opening /dev/userleds
# Set LED 0 to 1
# board_userled_all: ledset=0x1
# board_userled_all: led=0, val=1
# board_userled_all: led=1, val=0
# board_userled_all: led=2, val=0
# Waiting...
# Set LED 0 to 0
# board_userled_all: ledset=0x0
# board_userled_all: led=0, val=0
# board_userled_all: led=1, val=0
# board_userled_all: led=2, val=0

## Previously: Test for Async Tasks
import std/asyncdispatch
import std/strformat

proc task(id: int): Future[void] {.async.} =
  for loop in 0..2:
    echo &"Hello from task {id}! loops: {loop}"
    if loop < 2:
      await sleepAsync(1000)

proc launch() {.async.} =
  for id in 1..2:
    asyncCheck task(id)
    await sleepAsync(200)
  await task(3)

# Test in Linux:
# hello_nim() ####
# Output:
# $ ./hello_nim_async 
# Hello from task 1! loops: 0
# Hello from task 2! loops: 0
# Hello from task 3! loops: 0
# Hello from task 1! loops: 1
# Hello from task 2! loops: 1
# Hello from task 3! loops: 1
# Hello from task 1! loops: 2
# Hello from task 2! loops: 2
# Hello from task 3! loops: 2
