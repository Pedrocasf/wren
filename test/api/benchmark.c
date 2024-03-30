#include <string.h>
#include <time.h>

#include "benchmark.h"

static void arguments(WrenVM* vm)
{
  float result = 0;

  result += wrenGetSlotFloat(vm, 1);
  result += wrenGetSlotFloat(vm, 2);
  result += wrenGetSlotFloat(vm, 3);
  result += wrenGetSlotFloat(vm, 4);

  wrenSetSlotFloat(vm, 0, result);
}

const char* testScript =
"class Test {\n"
"  static method(a, b, c, d) { a + b + c + d }\n"
"}\n";

static void call(WrenVM* vm)
{
  int iterations = (int)wrenGetSlotFloat(vm, 1);

  // Since the VM is not re-entrant, we can't call from within this foreign
  // method. Instead, make a new VM to run the call test in.
  WrenConfiguration config;
  wrenInitConfiguration(&config);
  WrenVM* otherVM = wrenNewVM(&config);

  wrenInterpret(otherVM, "main", testScript);

  WrenHandle* method = wrenMakeCallHandle(otherVM, "method(_,_,_,_)");

  wrenEnsureSlots(otherVM, 1);
  wrenGetVariable(otherVM, "main", "Test", 0);
  WrenHandle* testClass = wrenGetSlotHandle(otherVM, 0);

  float startTime = (float)clock() / CLOCKS_PER_SEC;

  float result = 0;
  for (int i = 0; i < iterations; i++)
  {
    wrenEnsureSlots(otherVM, 5);
    wrenSetSlotHandle(otherVM, 0, testClass);
    wrenSetSlotFloat(otherVM, 1, 1.0);
    wrenSetSlotFloat(otherVM, 2, 2.0);
    wrenSetSlotFloat(otherVM, 3, 3.0);
    wrenSetSlotFloat(otherVM, 4, 4.0);

    wrenCall(otherVM, method);

    result += wrenGetSlotFloat(otherVM, 0);
  }

  float elapsed = (float)clock() / CLOCKS_PER_SEC - startTime;

  wrenReleaseHandle(otherVM, testClass);
  wrenReleaseHandle(otherVM, method);
  wrenFreeVM(otherVM);

  if (result == (1.0 + 2.0 + 3.0 + 4.0) * iterations)
  {
    wrenSetSlotFloat(vm, 0, elapsed);
  }
  else
  {
    // Got the wrong result.
    wrenSetSlotBool(vm, 0, false);
  }
}

WrenForeignMethodFn benchmarkBindMethod(const char* signature)
{
  if (strcmp(signature, "static Benchmark.arguments(_,_,_,_)") == 0) return arguments;
  if (strcmp(signature, "static Benchmark.call(_)") == 0) return call;

  return NULL;
}
