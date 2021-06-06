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

// #include "llvm/ADT/STLExtras.h"
// #include "llvm/IR/BasicBlock.h"
// #include "llvm/IR/DerivedTypes.h"
// #include "llvm/IR/Function.h"
// #include "llvm/IR/Verifier.h"

using namespace std;
using namespace llvm;

class CodeGenContext
{
public:
	LLVMContext llvmcontext;
	IRBuilder<> builder;
	Module llvmmodule;
	map<string, Value *> vars;

	CodeGenContext() : builder(llvmcontext), llvmmodule("module", llvmcontext) {}
};

class Node
{
public:
	virtual Value *codeGen(CodeGenContext &context) = 0;
};

class NVariable : public Node
{
public:
	string name;
	NVariable(string s) : name(s) {}

	Value *codeGen(CodeGenContext &context);
};

class NInt : public Node
{
public:
	long long value;
	NInt(long long v) : value(v) {}
};

class NDouble : public Node
{
public:
	double value;
	NDouble(double v) : value(v) {}

	Value *codeGen(CodeGenContext &context);
};

class NBinOp : public Node
{
public:
	char op;
	Node *left, *right;
	NBinOp(char c, Node *l, Node *r) : op(c), left(l), right(r) {}

	Value *codeGen(CodeGenContext &context);
};

#endif