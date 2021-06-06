#ifndef _CODEGEN_H
#define _CODEGEN_H

#include <iostream>
#include <map>
#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

using namespace std;
using namespace llvm;

class NExp;

class CodeGenContext
{
public:
	LLVMContext llvmcontext;
	IRBuilder<> builder;
	Module llvmmodule;
	map<string, NExp *> vars;

	CodeGenContext() : llvmcontext(), builder(llvmcontext), llvmmodule("module", llvmcontext) {}
};

#endif