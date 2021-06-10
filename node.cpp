#include "codeGen.h"

Value *NVariable::codeGen(CodeGenContext &context)
{
    Log("Var", name);
    Value *V = context.getSymbolValue(name);
    if (!V)
    {
        cout << "Unknown variable name" << endl;
    }
    return V;
}

Value *NNum::codeGen(CodeGenContext &context)
{
    Log("Double", value);
    return ConstantFP::get(Type::getDoubleTy(context.llvmcontext), value);
}

Value *NStr::codeGen(CodeGenContext &context)
{
    return context.builder.CreateGlobalString(value, "string");
}

Value *NArray::codeGen(CodeGenContext &context)
{
    cout << "Invalid use of array" << endl;
    return NULL;
}

NArray *NArrayIndex::getArrayNode(CodeGenContext &context)
{
    return context.getArrayNode(arrName);
}

Value *NArrayIndex::codeGen(CodeGenContext &context)
{
    int intIndex = int(((ConstantFP *)index->codeGen(context))->getValue().convertToDouble());

    return getArrayNode(context)->elements[intIndex]->codeGen(context);
}

void NArrayIndex::modify(CodeGenContext &context, NExp *newVal)
{
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    Log("====");
    if (op == BINOP_EQUAL)
    {
        Log("enter");
        Value *newVal = right->codeGen(context);

        NVariable *lvar = static_cast<NVariable *>(left);

        /* Valid Code */
        Type *type = context.typeToLLVMType(right->type);
        Value *inst = context.builder.CreateAlloca(type);
        context.getCurrentBlock()->localVars[lvar->name] = inst;
        context.getCurrentBlock()->localVarTypes[lvar->name] = right->type;
        Value *rvar = right->codeGen(context);
        Value *dst = context.getSymbolValue(lvar->name);
        context.builder.CreateStore(rvar, dst);
        return dst;
        /* End of valid code */

        return NULL;
    }

    Log("out");
    Value *L = left->codeGen(context);
    Log("Left", (((ConstantFP *)L)->getValue()).convertToDouble());
    Value *R = right->codeGen(context);
    Log("Right", (((ConstantFP *)R)->getValue()).convertToDouble());
    switch (op)
    {
    case BINOP_PLUS:
        return context.builder.CreateFAdd(L, R, "addtmp");
    case BINOP_MINUS:
        return context.builder.CreateFSub(L, R, "subtmp");
    case BINOP_MUL:
        return context.builder.CreateFMul(L, R, "multmp");
    case BINOP_DIV:
        return context.builder.CreateFDiv(L, R, "divtmp");
    case BINOP_CLT:
        return context.builder.CreateFCmpULT(L, R, "cmpftmp");
    case BINOP_CLE:
        return context.builder.CreateFCmpOLE(L, R, "cmpftmp");
    case BINOP_CGE:
        return context.builder.CreateFCmpOGE(L, R, "cmpftmp");
    case BINOP_CGT:
        return context.builder.CreateFCmpOGT(L, R, "cmpftmp");
    case BINOP_CEQ:
        return context.builder.CreateFCmpOEQ(L, R, "cmpftmp");
    case BINOP_CNE:
        return context.builder.CreateFCmpONE(L, R, "cmpftmp");
    }
    return NULL;
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
    //     return NULL;
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
    //     return NULL;
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
    return NULL;
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
    return NULL;
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
    return NULL;
}

Value *NFuncDef::codeGen(CodeGenContext &context)
{
    context.functions[name] = this;
    return NULL;
}

Value *NRetStmt::codeGen(CodeGenContext &context)
{
    return retVal->codeGen(context);
}
