#include "codeGen.h"

void CodeGenContext::pushBlock(BasicBlock *block)
{
    CodeGenBlock *codeGenBlock = new CodeGenBlock(block, nullptr);
    blockStack.push_back(codeGenBlock);
}

void CodeGenContext::popBlock()
{
    CodeGenBlock *codeGenBlock = getCurrentBlock();
    blockStack.pop_back();
    delete codeGenBlock;
}

CodeGenBlock *CodeGenContext::getCurrentBlock()
{
    return blockStack.back();
}

Value *CodeGenContext::getSymbolValue(string name) const
{
    for (auto it = blockStack.rbegin(); it != blockStack.rend(); it++)
        if ((*it)->localVars.find(name) != (*it)->localVars.end())
            return (*it)->localVars[name];

    return nullptr;
}

int CodeGenContext::getType(string name) const
{
    for (auto it = blockStack.rbegin(); it != blockStack.rend(); it++)
        if ((*it)->localVarTypes.find(name) != (*it)->localVarTypes.end())
            return (*it)->localVarTypes[name];

    return -1;
}

NArray *CodeGenContext::getArrayNode(string name) const
{
    for (auto it = blockStack.rbegin(); it != blockStack.rend(); it++)
        if ((*it)->localArrayNodes.find(name) != (*it)->localArrayNodes.end())
            return (*it)->localArrayNodes[name];

    return nullptr;
}

Type *CodeGenContext::typeToLLVMType(int T)
{
    Type *ret = nullptr;
    switch (T)
    {
    case (TYPE_NUM):
    {
        ret = Type::getDoubleTy(llvmcontext);
        break;
    }
    case (TYPE_STR):
    {
        ret = Type::getInt8PtrTy(llvmcontext);
        break;
    }
    }
    return ret;
}

void CodeGenContext::generateCode(NBlock &root)
{
    Log("Generating IR Code...");

    // std::vector<Type *> sysArgs;
    // FunctionType *FuncType = FunctionType::get(Type::getVoidTy(this->llvmcontext), makeArrayRef(sysArgs), false);
    // BasicBlock *block = BasicBlock::Create(this->llvmcontext, "entry");
    // pushBlock(block);

    std::vector<Type *> mainArgs;
    FunctionType *mainFuncType = FunctionType::get(Type::getInt32Ty(llvmcontext), makeArrayRef(mainArgs), false);
    Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main", module.get());
    BasicBlock *basicBlock = BasicBlock::Create(llvmcontext, "entry", mainFunc, nullptr);
    builder.SetInsertPoint(basicBlock);
    pushBlock(basicBlock);
    root.codeGen(*this);
    builder.CreateRet(ConstantInt::get(Type::getInt32Ty(llvmcontext), 0));
    popBlock();

    // popBlock();
    Log("Code Generation Success!");
    legacy::PassManager passManager;
    passManager.add(createPrintModulePass(outs()));
    passManager.run(*(module.get()));

    return;
}