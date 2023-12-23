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

proc hello_nim() {.exportc, cdecl.} =
  waitFor launch()
  GC_runOrc()

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
