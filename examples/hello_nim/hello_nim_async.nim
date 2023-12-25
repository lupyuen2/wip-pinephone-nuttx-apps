import std/strformat  ## String Formatting

## Import ioctl() from C
## Based on /home/vscode/.choosenim/toolchains/nim-#devel/lib/std/syncio.nim
proc c_ioctl(fd: cint, request: cint): cint {.
  importc: "ioctl", header: "<sys/ioctl.h>", varargs.}

## Main Function in Nim
proc hello_nim() {.exportc, cdecl.} =

  ## Print something
  echo "Hello Nim!"

  ## Test ioctl
  var ret = c_ioctl(0, 0)
  echo &"ret={ret}"

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
