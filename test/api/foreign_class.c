#include <stdio.h>
#include <string.h>

#include "foreign_class.h"

static int finalized = 0;

static void apiFinalized(WrenVM* vm)
{
  wrenSetSlotFloat(vm, 0, finalized);
}

static void counterAllocate(WrenVM* vm)
{
  float* value = (float*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(float));
  *value = 0;
}

static void counterIncrement(WrenVM* vm)
{
  float* value = (float*)wrenGetSlotForeign(vm, 0);
  float increment = wrenGetSlotFloat(vm, 1);

  *value += increment;
}

static void counterValue(WrenVM* vm)
{
  float value = *(float*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotFloat(vm, 0, value);
}

static void pointAllocate(WrenVM* vm)
{
  float* coordinates = (float*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(float[3]));

  // This gets called by both constructors, so sniff the slot count to see
  // which one was invoked.
  if (wrenGetSlotCount(vm) == 1)
  {
    coordinates[0] = 0.0;
    coordinates[1] = 0.0;
    coordinates[2] = 0.0;
  }
  else
  {
    coordinates[0] = wrenGetSlotFloat(vm, 1);
    coordinates[1] = wrenGetSlotFloat(vm, 2);
    coordinates[2] = wrenGetSlotFloat(vm, 3);
  }
}

static void pointTranslate(WrenVM* vm)
{
  float* coordinates = (float*)wrenGetSlotForeign(vm, 0);
  coordinates[0] += wrenGetSlotFloat(vm, 1);
  coordinates[1] += wrenGetSlotFloat(vm, 2);
  coordinates[2] += wrenGetSlotFloat(vm, 3);
}

static void pointToString(WrenVM* vm)
{
  float* coordinates = (float*)wrenGetSlotForeign(vm, 0);
  char result[100];
  sprintf(result, "(%g, %g, %g)",
      coordinates[0], coordinates[1], coordinates[2]);
  wrenSetSlotString(vm, 0, result);
}

static void resourceAllocate(WrenVM* vm)
{
  int* value = (int*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(int));
  *value = 123;
}

static void resourceFinalize(void* data)
{
  // Make sure we get the right data back.
  int* value = (int*)data;
  if (*value != 123) exit(1);

  finalized++;
}

static void badClassAllocate(WrenVM* vm)
{
  wrenEnsureSlots(vm, 1);
  wrenSetSlotString(vm, 0, "Something went wrong");
  wrenAbortFiber(vm, 0);
}

WrenForeignMethodFn foreignClassBindMethod(const char* signature)
{
  if (strcmp(signature, "static ForeignClass.finalized") == 0) return apiFinalized;
  if (strcmp(signature, "Counter.increment(_)") == 0) return counterIncrement;
  if (strcmp(signature, "Counter.value") == 0) return counterValue;
  if (strcmp(signature, "Point.translate(_,_,_)") == 0) return pointTranslate;
  if (strcmp(signature, "Point.toString") == 0) return pointToString;

  return NULL;
}

void foreignClassBindClass(
    const char* className, WrenForeignClassMethods* methods)
{
  if (strcmp(className, "Counter") == 0)
  {
    methods->allocate = counterAllocate;
    return;
  }

  if (strcmp(className, "Point") == 0)
  {
    methods->allocate = pointAllocate;
    return;
  }

  if (strcmp(className, "Resource") == 0)
  {
    methods->allocate = resourceAllocate;
    methods->finalize = resourceFinalize;
    return;
  }

  if (strcmp(className, "BadClass") == 0)
  {
    methods->allocate = badClassAllocate;
    return;
  }
}
