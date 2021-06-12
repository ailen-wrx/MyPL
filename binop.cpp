#include "binop.h"

map<int, Value *(*)(CodeGenContext &context, NExp *left, NExp *right)> BinaryOperation;

Value *binaryAssign(CodeGenContext &context, NExp *left, NExp *right)
{
    int targetType = right->type;
    switch (right->type)
    {
    case TYPE_BINOP:
    case TYPE_CALL:
    case TYPE_ARRIDX:
    {
        targetType = TYPE_NUM;
        break;
    }
    }
    Value *rval = right->codeGen(context);
    Value *dst;

    switch (left->type)
    {
    case TYPE_VAR:
    {
        NVariable *lvar = static_cast<NVariable *>(left);
        int a = context.getType(lvar->name);
        if (a == -1)
        {
            context.getCurrentBlock()->localVarTypes[lvar->name] = targetType;

            if (targetType == TYPE_ARR)
            {
                // context.getCurrentBlock()->localVars[lvar->name] = rval;
                context.arrays[lvar->name] = (NArray *)right;
            }
            // else
            // {
            Type *type = context.typeToLLVMType(right->type);
            dst = context.builder.CreateAlloca(type);
            context.getCurrentBlock()->localVars[lvar->name] = dst;
            context.builder.CreateStore(rval, dst);
            // }
        }
        else
        {
            if (a != targetType)
            {
                cout << "Fail to match variables." << endl;
                break;
            }
            dst = context.getSymbolValue(lvar->name);
            context.builder.CreateStore(rval, dst);
        }
        break;
    }

    case TYPE_ARRIDX:
    {
        NArrayIndex *lvar = static_cast<NArrayIndex *>(left);
        NArray *targetArray = context.arrays[lvar->arrName];
        if (targetArray->elementType == -1)
        {
            targetArray->type = targetType;
        }
        else if (targetArray->elementType != targetType)
        {
            cout << "Fail to match variables." << endl;
            break;
        }

        dst = lvar->codeGen(context);
        context.builder.CreateStore(rval, dst);
        break;
    }

    default:
        break;
    }

    //TODO: right->type bug(BinOp).
    return rval;
}

Value *binaryPlus(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFAdd(left->codeGen(context), right->codeGen(context), "addtmp");
}
Value *binaryMinus(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFSub(left->codeGen(context), right->codeGen(context), "subtmp");
}
Value *binaryMul(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFMul(left->codeGen(context), right->codeGen(context), "multmp");
}
Value *binaryDiv(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFDiv(left->codeGen(context), right->codeGen(context), "divtmp");
}
Value *binaryClt(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFCmpULT(left->codeGen(context), right->codeGen(context), "cmpftmp");
}
Value *binaryCle(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFCmpOLE(left->codeGen(context), right->codeGen(context), "cmpftmp");
}
Value *binaryCge(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFCmpOGE(left->codeGen(context), right->codeGen(context), "cmpftmp");
}
Value *binaryCgt(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFCmpOGT(left->codeGen(context), right->codeGen(context), "cmpftmp");
}
Value *binaryCeq(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFCmpOEQ(left->codeGen(context), right->codeGen(context), "cmpftmp");
}
Value *binaryCne(CodeGenContext &context, NExp *left, NExp *right)
{
    return context.builder.CreateFCmpONE(left->codeGen(context), right->codeGen(context), "cmpftmp");
}

void initializeBinaryOperation()
{
    BinaryOperation[BINOP_ASSIGN] = binaryAssign;
    BinaryOperation[BINOP_PLUS] = binaryPlus;
    BinaryOperation[BINOP_MINUS] = binaryMinus;
    BinaryOperation[BINOP_MUL] = binaryMul;
    BinaryOperation[BINOP_DIV] = binaryDiv;
    BinaryOperation[BINOP_CLT] = binaryClt;
    BinaryOperation[BINOP_CLE] = binaryCle;
    BinaryOperation[BINOP_CGE] = binaryCge;
    BinaryOperation[BINOP_CGT] = binaryCgt;
    BinaryOperation[BINOP_CEQ] = binaryCeq;
    BinaryOperation[BINOP_CNE] = binaryCne;
}
