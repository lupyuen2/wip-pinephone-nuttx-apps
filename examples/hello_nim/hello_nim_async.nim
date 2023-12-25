import std/strformat  ## String Formatting

## Import Standard Functions from C.
## Based on /home/vscode/.choosenim/toolchains/nim-#devel/lib/std/syncio.nim
proc c_open(filename: cstring, mode: cint): cint {.
  importc: "open", nodecl.}
proc c_close(fd: cint): cint {.
  importc: "close", nodecl, discardable.}
proc c_ioctl(fd: cint, request: cint): cint {.
  importc: "ioctl", nodecl, varargs.}
proc c_usleep(usec: cuint): cint {.
  importc: "usleep", nodecl, discardable.}
var O_WRONLY {.importc: "O_WRONLY", header: "<fcntl.h>".}: cint
var ULEDIOC_SETALL {.importc: "ULEDIOC_SETALL", header: "<nuttx/leds/userled.h>".}: cint

## Blink the LED
proc blink_led() =

  ## Open the LED driver
  echo "Opening /dev/userleds"
  var fd = c_open("/dev/userleds", O_WRONLY)
  if fd < 0:
    echo "Failed to open /dev/userleds"
    return

  ## Turn on LED
  echo "Set LED 0 to 1"
  var ret = c_ioctl(fd, ULEDIOC_SETALL, 1)
  if ret < 0:
    echo "ioctl(ULEDIOC_SETALL) failed"
    return

  ## Wait a second (literally)
  echo "Waiting..."
  c_usleep(1000_000)

  ## Turn on LED
  echo "Set LED 0 to 0"
  ret = c_ioctl(fd, ULEDIOC_SETALL, 0)
  if ret < 0:
    echo "ioctl(ULEDIOC_SETALL) failed"
    return

  ## Close the LED Driver
  c_close(fd)

## Main Function in Nim
proc hello_nim() {.exportc, cdecl.} =

  ## Print something
  echo "Hello Nim!"

  ## Blink the LED
  blink_led()

  ## Finish
  ## waitFor launch()
  GC_runOrc()

## Compile on NuttX:
# export TOPDIR=/workspaces/bookworm/nuttx
# cd /workspaces/bookworm/apps/examples/hello_nim
# nim c --header hello_nim_async.nim 

## Previously:
import std/asyncdispatch

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
