#include "codeGen.h"
#include "builtin.h"

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
    // Check all blocks in the stack to find the value.
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

Type *CodeGenContext::typeToLLVMType(int type)
{
    Type *ret = nullptr;
    switch (type)
    {
    case (TYPE_INT):
    {
        ret = Type::getInt32Ty(llvmcontext);
        break;
    }
    case (TYPE_DOUBLE):
    {
        ret = Type::getDoubleTy(llvmcontext);
        break;
    }
    case (TYPE_CHAR):
    {
        ret = Type::getInt8Ty(llvmcontext);
        break;
    }
    case (TYPE_STR):
    {
        ret = Type::getInt8PtrTy(llvmcontext);
        break;
    }
    case (TYPE_ARR):
    {
        ret = Type::getInt32PtrTy(llvmcontext);
        break;
    }
    }
    return ret;
}

void CodeGenContext::generateCode(NBlock &root)
{
    Log("Generating IR Code...");

    // Create main function wrapper.
    std::vector<Type *> mainArgs;
    FunctionType *mainFuncType = FunctionType::get(Type::getInt32Ty(llvmcontext), makeArrayRef(mainArgs), false);
    Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main", module);
    BasicBlock *basicBlock = BasicBlock::Create(llvmcontext, "entry", mainFunc, nullptr);
    builder.SetInsertPoint(basicBlock);

    initializeBuiltinFunction(*this);

    // Code generation.
    pushBlock(basicBlock);
    root.codeGen(*this);
    builder.CreateRet(ConstantInt::get(Type::getInt32Ty(llvmcontext), 0));
    popBlock();

    Log("Code Generation Success!");
    legacy::PassManager passManager;
    passManager.add(createPrintModulePass(outs()));
    passManager.run(module);

    return;
}