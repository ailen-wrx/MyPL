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
    // NFuncDef *f = context.functions[funcName];
    // if (f->args.size() != this->args.size())
    // {
    //     cout << "function unmatched." << endl;
    //     return nullptr;
    // }

    // vars_reserved = context.vars;
    // context.vars.clear();

    // int idx = 0;
    // for (auto i : this->args)
    //     context.vars[f->args[idx++]] = i;

    // Value *ret = f->body->codeGen(context);

    // context.vars = vars_reserved;

    // return ret;
    return nullptr;
}

Value *NBlock::codeGen(CodeGenContext &context)
{
    Value *ret;
    for (NStmt *i : statements)
    {
        ret = i->codeGen(context);
        if (i->stmt_type == STMT_TYPE_RET)
            return ret;
    }
    return nullptr;
}

Value *NIfStmt::codeGen(CodeGenContext &context)
{
    Log("If statement");
    Value *condVal = this->cond->codeGen(context);
    auto cond = ((ConstantInt *)condVal)->getLimitedValue();
    if (cond == 0)
    {
        this->el->codeGen(context);
    }
    else
    {
        this->then->codeGen(context);
    }
    return nullptr;
}

Value *NWhileStmt::codeGen(CodeGenContext &context)
{
    Value *condVal = this->Cond->codeGen(context);
    auto cond = ((ConstantInt *)condVal)->getLimitedValue();

    while (cond != 0)
    {
        this->body->codeGen(context);
        condVal = this->Cond->codeGen(context);
        cond = ((ConstantInt *)condVal)->getLimitedValue();
    }
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
