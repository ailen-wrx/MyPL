#include "codeGen.h"
extern map<int, Value *(*)(CodeGenContext &context, NExp *left, NExp *right)> BinaryOperation;
extern map<string, Value *(*)(CodeGenContext &context, vector<NExp *> &args)> BuiltinFunction;

bool NVariable::isDouble(CodeGenContext &context)
{
    return context.getCurrentBlock()->localVarTypes[name] == TYPE_DOUBLE;
}

Value *NVariable::codeGen(CodeGenContext &context)
{
    Log("Var", name);

    // Get pointer to the value.
    Value *val = context.getSymbolValue(name);
    if (!val)
    {
        cout << "Unknown variable name" << endl;
        return nullptr;
    }
    int type = context.getType(name);
    if (type == TYPE_INTARR || type == TYPE_DOUBLEARR || type == TYPE_STRARR || type == TYPE_CHARARR)
    {
        // Return pointer to the array.
        ArrayRef<Value *> indices{ConstantInt::get(Type::getInt32Ty(context.llvmcontext), 0, false)};
        auto ptr = context.builder.CreateInBoundsGEP(val, indices, "arrayPtr");
        return ptr;
    }

    // Load value and return.
    return context.builder.CreateLoad(val, false, "");
}

Value *NDouble::codeGen(CodeGenContext &context)
{
    Log("Double", value);
    return ConstantFP::get(Type::getDoubleTy(context.llvmcontext), value);
}

Value *NInt::codeGen(CodeGenContext &context)
{
    Log("Int", value);
    return ConstantInt::get(Type::getInt32Ty(context.llvmcontext), value, true);
}

Value *NChar::codeGen(CodeGenContext &context)
{
    Log("Char", char(value));
    return ConstantInt::get(Type::getInt8Ty(context.llvmcontext), value, true);
}

Value *NStr::codeGen(CodeGenContext &context)
{
    Log("String", value);
    return context.builder.CreateGlobalString(value, "string");
}

Value *NArray::codeGen(CodeGenContext &context)
{
    Value *arraySizeValue;
    ArrayType *arrayType;
    switch (type)
    {
    case (TYPE_INTARR):
    {
        arraySizeValue = ConstantInt::get(Type::getInt32Ty(context.llvmcontext), size);
        arrayType = ArrayType::get(Type::getInt32Ty(context.llvmcontext), size);
        break;
    }
    case (TYPE_DOUBLEARR):
    {
        arraySizeValue = ConstantFP::get(Type::getInt32Ty(context.llvmcontext), size);
        arrayType = ArrayType::get(Type::getDoubleTy(context.llvmcontext), size);
        break;
    }
    case (TYPE_CHARARR):
    {
        arraySizeValue = ConstantInt::get(Type::getInt32Ty(context.llvmcontext), size);
        arrayType = ArrayType::get(Type::getInt8Ty(context.llvmcontext), size);
        break;
    }
    case (TYPE_STRARR):
    {
        arraySizeValue = ConstantInt::get(Type::getInt32Ty(context.llvmcontext), size);
        arrayType = ArrayType::get(Type::getInt8PtrTy(context.llvmcontext), size);
        break;
    }
    }

    ConstantAggregateZero *constArray = ConstantAggregateZero::get(arrayType);

    Value *dst;
    if (isGlobal)
    {
        // Create global array.
        dst = new GlobalVariable(context.module, arrayType, false, GlobalValue::ExternalLinkage,
                                 constArray, "globalArray");
    }
    else
    {
        // Create local array.
        dst = context.builder.CreateAlloca(arrayType, arraySizeValue, "arraytmp");
    }
    return dst;
}

NArray *NArrayIndex::getArrayNode(CodeGenContext &context)
{
    return context.getArrayNode(arrName);
}

Value *NArrayIndex::codeGen(CodeGenContext &context)
{
    NArray *arrayNode = context.getArrayNode(arrName);
    Value *arrPtr = context.getSymbolValue(arrName);
    if (!arrPtr)
    {
        cout << "Unknown variable name" << endl;
        return nullptr;
    }

    Value *indexValue = index->codeGen(context);

    vector<Value *> normalIndicesVec{ConstantInt::get(Type::getInt32Ty(context.llvmcontext), 0), indexValue};
    ArrayRef<Value *> normalIndices(normalIndicesVec);
    ArrayRef<Value *> argIndices{indexValue};
    Value *ptr;

    if (context.getCurrentBlock()->isFuncArgs[arrName])
    {
        // When the array is argument, we have a pointer rather than the array itself.
        arrPtr = context.builder.CreateLoad(arrPtr, "actualArrayPtr");
        ptr = context.builder.CreateInBoundsGEP(arrPtr, argIndices, "elementPtr");
    }
    else
    {
        ptr = context.builder.CreateInBoundsGEP(arrPtr, normalIndices, "elementPtr");
    }

    return context.builder.CreateLoad(ptr);
}

Value *NArrayIndex::modify(CodeGenContext &context, Value *newVal)
{
    NArray *arrayNode = context.getArrayNode(arrName);
    Value *arrPtr = context.getSymbolValue(arrName);
    if (!arrPtr)
    {
        cout << "Unknown variable name" << endl;
        return nullptr;
    }

    Value *indexValue = index->codeGen(context);

    vector<Value *> normalIndicesVec{ConstantInt::get(Type::getInt32Ty(context.llvmcontext), 0), indexValue};
    ArrayRef<Value *> normalIndices(normalIndicesVec);
    ArrayRef<Value *> argIndices{indexValue};
    Value *ptr;

    if (context.getCurrentBlock()->isFuncArgs[arrName])
    {
        // When the array is argument, we have a pointer rather than the array itself.
        arrPtr = context.builder.CreateLoad(arrPtr, "actualArrayPtr");
        ptr = context.builder.CreateInBoundsGEP(arrPtr, argIndices, "elementPtr");
    }
    else
    {
        ptr = context.builder.CreateInBoundsGEP(arrPtr, normalIndices, "elementPtr");
    }

    return context.builder.CreateStore(newVal, ptr);
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    Log("Binary");
    // Dealt by functions in binop.cpp.
    return BinaryOperation[op](context, left, right);
}

Value *NCallFunc::codeGen(CodeGenContext &context)
{
    Log("Function Call: ", this->funcName);
    if (BuiltinFunction.find(funcName) != BuiltinFunction.end())
    {
        // Dealt by function in builtin.cpp
        return BuiltinFunction[funcName](context, args);
    }
    Function *calleeFunc = context.module.getFunction(this->funcName);

    if (!calleeFunc)
        return nullptr;

    vector<Value *> argsVec;
    for (NExp *arg : args)
    {
        argsVec.push_back(arg->codeGen(context));
        if (!argsVec.back())
            return nullptr;
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
        // Stop execution when coming across a return statement.
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

    // `then` is the block executed when the condition is true.
    BasicBlock *thenBlk = BasicBlock::Create(context.llvmcontext, "then", parentFunc);

    // `else` is the block executed when the condition is false.
    BasicBlock *elseBlk = BasicBlock::Create(context.llvmcontext, "else");

    // `ifcont` is the block executed after the if statement.
    BasicBlock *contBlk = BasicBlock::Create(context.llvmcontext, "ifcont");

    context.builder.CreateCondBr(condVal, thenBlk, elseBlk);
    context.builder.SetInsertPoint(thenBlk);
    context.pushBlock(thenBlk);
    // Code generation for `then` block.
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
    // Code generation for `else` block
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

    // `whileloop` is the block executed when the loop condition is true.
    BasicBlock *loopBlk = BasicBlock::Create(context.llvmcontext, "whileloop", parentFunc);

    // `whilecont` is the block executed after the while loop.
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
    context.functions[name] = this;

    vector<Type *> argTypes;
    for (auto i : args)
    {
        argTypes.push_back(context.typeToLLVMType(i.first));
    }

    // Function declaration.
    FunctionType *funcType = FunctionType::get(context.typeToLLVMType(TYPE_INT), argTypes, false);
    Function *f = Function::Create(funcType, GlobalValue::ExternalLinkage, name.c_str(), context.module);

    if (!isExternal)
    {
        // Function definition.
        BasicBlock *block = BasicBlock::Create(context.llvmcontext, "entry", f, nullptr);
        context.builder.SetInsertPoint(block);
        context.pushBlock(block);

        int index = 0;
        for (auto &a : f->args())
        {
            a.setName(args[index].second);

            Value *argAlloc;
            int argType = args[index].first;
            if (argType == TYPE_INTARR || argType == TYPE_DOUBLEARR || argType == TYPE_STRARR || argType == TYPE_CHARARR)
            {
                // Allocate pointer for array argument.
                argAlloc = context.builder.CreateAlloca(context.typeToLLVMType(argType));
            }
            else
                argAlloc = context.builder.CreateAlloca(a.getType());

            // Add arguments to local variables.
            context.builder.CreateStore(&a, argAlloc);
            context.getCurrentBlock()->localVars[args[index].second] = argAlloc;
            context.getCurrentBlock()->localVarTypes[args[index].second] =
                a.getType()->getTypeID() == Type::PointerTyID ? TYPE_INTARR : TYPE_INT;
            context.getCurrentBlock()->isFuncArgs[args[index].second] = true;

            index++;
        }

        // Code generation for function body.
        body->codeGen(context);
        context.builder.CreateRet(context.getCurrentBlock()->returnValue);

        context.popBlock();
    }

    // Get out of function definition.
    context.builder.SetInsertPoint(context.getCurrentBlock()->block);

    return f;
}

Value *NRetStmt::codeGen(CodeGenContext &context)
{
    Value *returnVal = retVal->codeGen(context);
    context.getCurrentBlock()->returnValue = returnVal;
    return returnVal;
}
