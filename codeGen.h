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

#include "node.h"
#include "parser.hpp"

using namespace std;
using namespace llvm;

class CodeGenBlock
{
public:
	BasicBlock *block;
	Value *returnValue;
	std::map<string, Value *> locals;
	std::map<string, int> types;
	std::map<string, bool> isFuncArg;
	std::map<string, vector<uint64_t>> arraySizes;
};

class CodeGenContext
{
public:
	vector<CodeGenBlock *> blockStack;

	LLVMContext llvmcontext;
	IRBuilder<> builder;
	unique_ptr<Module> module;
	map<string, NExp *> vars;
	map<string, NFuncDef *> functions;

	CodeGenContext() : builder(llvmcontext)
	{
		module = unique_ptr<Module>(new Module("main", this->llvmcontext));
	}

	void pushBlock(BasicBlock *block)
	{
		CodeGenBlock *codeGenBlock = new CodeGenBlock();
		codeGenBlock->block = block;
		codeGenBlock->returnValue = nullptr;
		blockStack.push_back(codeGenBlock);
	}

	void popBlock()
	{
		CodeGenBlock *codeGenBlock = blockStack.back();
		blockStack.pop_back();
		delete codeGenBlock;
	}

	Value *getSymbolValue(string name) const
	{
		for (auto it = blockStack.rbegin(); it != blockStack.rend(); it++)
		{
			if ((*it)->locals.find(name) != (*it)->locals.end())
			{
				return (*it)->locals[name];
			}
		}
		return nullptr;
	}

	void generateCode(NBlock &root)
	{
		cout << "[LOG]  Generating IR Code..." << endl;
		std::vector<Type *> sysArgs;

		FunctionType *functionType = FunctionType::get(Type::getDoubleTy(llvmcontext), NULL, false);
		Function *function = Function::Create(functionType, GlobalValue::ExternalLinkage, "main", module.get());
		BasicBlock *basicBlock = BasicBlock::Create(llvmcontext, "entry", function, nullptr);
		builder.SetInsertPoint(basicBlock);
		pushBlock(basicBlock);
		root.codeGen(*this);
		popBlock();

		cout << "[LOG]  Code Generation Success!" << endl;
	}

	Type *getType(int T)
	{
		Type *doubleTy = Type::getDoubleTy(llvmcontext);
		Type *stringTy = Type::getInt8PtrTy(llvmcontext);
		switch (T)
		{
		case (TYPE_NUM):
		{
			return doubleTy;
			break;
		}
		case (TYPE_STR):
		{
			return stringTy;
			break;
		}
		default:
		{
			return NULL;
			break;
		}
		}
	}
};

#endif