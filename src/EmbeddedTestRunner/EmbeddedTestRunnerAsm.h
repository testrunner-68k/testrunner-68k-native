#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "EmbeddedTestRunner.h"

bool EmbeddedTestRunner_runTestAtAddress(register TestEntryPoint testEntryPoint __asm("a0"));

#ifdef __cplusplus
}
#endif
