#include "codeGen.h"
extern map<int, Value *(*)(CodeGenContext &context, NExp *left, NExp *right)> BinaryOperation;

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
    // if (funcName == "print")
    // {
    //     for (NExp *i : this->args)
    //     {
    //         switch (i->type)
    //         {
    //         case TYPE_NUM:
    //         {
    //             cout << ((NNum *)i)->value;
    //             break;
    //         }

    //         case TYPE_STR:
    //         {
    //             cout << ((NStr *)i)->value;
    //             break;
    //         }

    //         case TYPE_VAR:
    //         {
    //             NExp *val = ((NVariable *)i)->getTarget(context);
    //             switch (val->type)
    //             {
    //             case TYPE_NUM:
    //             {
    //                 cout << ((NNum *)val)->value;
    //                 break;
    //             }
    //             case TYPE_STR:
    //             {
    //                 cout << ((NStr *)val)->value;
    //                 break;
    //             }
    //             default:
    //             {
    //                 Value *v = i->codeGen(context);
    //                 cout << (((ConstantFP *)v)->getValue()).convertToDouble();
    //             }
    //             }
    //             break;
    //         }

    //         default:
    //             Value *v = i->codeGen(context);
    //             cout << (((ConstantFP *)v)->getValue()).convertToDouble();
    //         }
    //     }
    //     cout << endl;
    //     return nullptr;
    // }
    // else if (funcName == "readn")
    // {
    //     double in;
    //     cin >> in;
    //     NExp *target = this->args[0];
    //     switch (target->type)
    //     {
    //     case TYPE_VAR:
    //         context.vars[((NVariable *)target)->name] = new NNum(in);
    //         break;
    //     case TYPE_ARRIDX:
    //         ((NArrayIndex *)target)->modify(context, new NNum(in));
    //         break;
    //     default:
    //         cout << "Invalid assignment" << endl;
    //     }
    // }
    // else if (funcName == "reads")
    // {
    //     string in;
    //     cin >> in;
    //     NExp *target = this->args[0];
    //     switch (target->type)
    //     {
    //     case TYPE_VAR:
    //         context.vars[((NVariable *)target)->name] = new NStr(in);
    //         break;
    //     case TYPE_ARRIDX:
    //         ((NArrayIndex *)target)->modify(context, new NStr(in));
    //         break;
    //     default:
    //         cout << "Invalid assignment" << endl;
    //     }
    // }
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
    context.functions[name] = this;
    return nullptr;
}

Value *NRetStmt::codeGen(CodeGenContext &context)
{
    return retVal->codeGen(context);
}
