#include "codeGen.h"
extern map<int, Value *(*)(CodeGenContext &context, NExp *left, NExp *right)> BinaryOperation;
extern map<string, Value *(*)(CodeGenContext &context, vector<NExp *> &args)> BuiltinFunction;

Value *NVariable::codeGen(CodeGenContext &context)
{
    Log("Var", name);
    Value *V = context.getSymbolValue(name);
    if (!V)
    {
        cout << "Unknown variable name" << endl;
        return nullptr;
    }
    if (context.getType(name) == TYPE_ARR)
    {
        // TODO: Deal with array
    }
    return context.builder.CreateLoad(V, false, "");
}

Value *NNum::codeGen(CodeGenContext &context)
{
    Log("Double", value);
    return ConstantFP::get(Type::getDoubleTy(context.llvmcontext), value);
}

Value *NStr::codeGen(CodeGenContext &context)
{
    Log("String", value);
    return context.builder.CreateGlobalString(value, "string");
}

Value *NArray::codeGen(CodeGenContext &context)
{
    cout << "Invalid use of array" << endl;
    return nullptr;
}

NArray *NArrayIndex::getArrayNode(CodeGenContext &context)
{
    return context.getArrayNode(arrName);
}

Value *NArrayIndex::codeGen(CodeGenContext &context)
{
    // TODO: to check

    Value *arrPtr = context.getSymbolValue(arrName);
    if (!arrPtr)
    {
        cout << "Unknown variable name" << endl;
        return nullptr;
    }

    Value *indexValue = index->codeGen(context);

    arrPtr = context.builder.CreateLoad(arrPtr, "actualArrayPtr");
    Value *ptr = context.builder.CreateInBoundsGEP(arrPtr, indexValue, "elementPtr");

    return context.builder.CreateAlignedLoad(ptr, MaybeAlign(4));
}

Value *NArrayIndex::modify(CodeGenContext &context, NExp *newVal)
{
    // TODO: to check

    Value *arrPtr = context.getSymbolValue(arrName);
    if (!arrPtr)
    {
        cout << "Unknown variable name" << endl;
        return nullptr;
    }

    Value *indexValue = index->codeGen(context);

    arrPtr = context.builder.CreateLoad(arrPtr, "actualArrayPtr");
    Value *ptr = context.builder.CreateInBoundsGEP(arrPtr, indexValue, "elementPtr");

    return context.builder.CreateAlignedStore(newVal->codeGen(context), ptr, MaybeAlign(4));
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    Log("Binary");
    return BinaryOperation[op](context, left, right);
}

Value *NCallFunc::codeGen(CodeGenContext &context)
{
    Log("Function Call: ", this->funcName);
    Function *calleeFunc = context.module.getFunction(this->funcName);
    if (!calleeFunc)
    {
        // TODO: search for BuiltIn methods
        return nullptr;
    }
    if (calleeFunc->arg_size() != this->args.size())
    {
        cout << "Arguments fail to match function " << this->funcName << endl;
    }
    vector<Value *> argsVec;
    for (NExp *arg : args)
    {
        argsVec.push_back(arg->codeGen(context));
        if (!argsVec.back())
        {
            return nullptr;
        }
    }
    return context.builder.CreateCall(calleeFunc, argsVec, "calltmp");
}

Value *NBlock::codeGen(CodeGenContext &context)
{
    Log("Block");
    Value *ret = nullptr;
    for (NStmt *i : statements)
    {
        ret = i->codeGen(context);
        if (i->stmt_type == STMT_TYPE_RET)
            return ret;
    }
    return ret;
}

Value *NIfStmt::codeGen(CodeGenContext &context)
{
    Log("If statement");
    Value *condVal = this->cond->codeGen(context);
    if (!condVal)
        return nullptr;
    condVal = context.builder.CreateIntCast(condVal, Type::getInt1Ty(context.llvmcontext), true);
    Function *parentFunc = context.builder.GetInsertBlock()->getParent();
    BasicBlock *thenBlk = BasicBlock::Create(context.llvmcontext, "then", parentFunc);
    BasicBlock *elseBlk = BasicBlock::Create(context.llvmcontext, "else");
    BasicBlock *contBlk = BasicBlock::Create(context.llvmcontext, "ifcont");
    context.builder.CreateCondBr(condVal, thenBlk, elseBlk);
    context.builder.SetInsertPoint(thenBlk);
    context.pushBlock(thenBlk);
    this->then->codeGen(context);
    context.popBlock();
    thenBlk = context.builder.GetInsertBlock();
    if (thenBlk->getTerminator() == nullptr)
    {
        context.builder.CreateBr(contBlk);
    }
    parentFunc->getBasicBlockList().push_back(elseBlk);
    context.builder.SetInsertPoint(elseBlk);
    context.pushBlock(thenBlk);
    this->el->codeGen(context);
    context.popBlock();
    context.builder.CreateBr(contBlk);
    parentFunc->getBasicBlockList().push_back(contBlk);
    context.builder.SetInsertPoint(contBlk);
    return nullptr;
}

Value *NWhileStmt::codeGen(CodeGenContext &context)
{
    Log("While statement");
    Value *condVal = this->cond->codeGen(context);
    if (!condVal)
        return nullptr;
    condVal = context.builder.CreateIntCast(condVal, Type::getInt1Ty(context.llvmcontext), true);
    Function *parentFunc = context.builder.GetInsertBlock()->getParent();
    BasicBlock *loopBlk = BasicBlock::Create(context.llvmcontext, "whileloop", parentFunc);
    BasicBlock *contBlk = BasicBlock::Create(context.llvmcontext, "whilecont");
    context.builder.CreateCondBr(condVal, loopBlk, contBlk);
    context.builder.SetInsertPoint(loopBlk);
    context.pushBlock(loopBlk);
    this->body->codeGen(context);
    context.popBlock();
    condVal = this->cond->codeGen(context);
    condVal = context.builder.CreateIntCast(condVal, Type::getInt1Ty(context.llvmcontext), true);
    context.builder.CreateCondBr(condVal, loopBlk, contBlk);
    parentFunc->getBasicBlockList().push_back(contBlk);
    context.builder.SetInsertPoint(contBlk);
    return nullptr;
}

Value *NFuncDef::codeGen(CodeGenContext &context)
{
    vector<Type *> argTypes;
    context.functions[name] = this;
    return nullptr;
}

Value *NRetStmt::codeGen(CodeGenContext &context)
{
    Value *returnVal = retVal->codeGen(context);
    context.getCurrentBlock()->returnValue = returnVal;
    return returnVal;
}
