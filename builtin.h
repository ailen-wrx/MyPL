// builtin.h and builtin.cpp are for builtin functions implementations.
// Builtin functions include `printf()`, `scanf()` and `endline()` .

#ifndef _BUILTIN_H
#define _BUILTIN_H

#include "codeGen.h"

void initializeBuiltinFunction(CodeGenContext &context);

#endif