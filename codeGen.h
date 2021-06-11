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

#include "node.h"

class CodeGenBlock
{

public:
	BasicBlock *block;
	Value *returnValue;

	map<string, Value *> localVars;
	map<string, int> localVarTypes;
	map<string, NArray *> localArrayNodes;

	CodeGenBlock(BasicBlock *b, Value *v) : block(b), returnValue(v) {}

private:
	CodeGenBlock(const CodeGenBlock &b);
};

class CodeGenContext
{

public:
	LLVMContext llvmcontext;
	IRBuilder<> builder;
	Module module;

	vector<CodeGenBlock *> blockStack;
	map<string, NFuncDef *> functions;

	CodeGenContext() : llvmcontext(), builder(llvmcontext), module("main", llvmcontext) {}

	void pushBlock(BasicBlock *block);
	void popBlock();
	CodeGenBlock *getCurrentBlock();

	Value *getSymbolValue(string name) const;
	int getType(string name) const;
	Type *typeToLLVMType(int T);
	NArray *getArrayNode(string name) const;

	void generateCode(NBlock &root);

private:
	CodeGenContext(const CodeGenContext &c);
};

#endif