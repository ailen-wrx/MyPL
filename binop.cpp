#include "binop.h"

map<int, Value *(*)(CodeGenContext &context, NExp *left, NExp *right)> BinaryOperation;

Value *binaryAssign(CodeGenContext &context, NExp *left, NExp *right)
{
    switch (left->type)
    {
    case TYPE_VAR:
    {
        NVariable *lvar = static_cast<NVariable *>(left);
        auto a = context.getType(lvar->name);
        if (a == -1)
        {
            Type *type = context.typeToLLVMType(right->type);
            Value *inst = context.builder.CreateAlloca(type);
            context.blockStack.back()->localVars[lvar->name] = inst;
            context.blockStack.back()->localVarTypes[lvar->name] = right->type;
            Value *rvar = right->codeGen(context);
            context.builder.CreateStore(rvar, inst);
            // context.builder.CreateLoad(inst);
            return inst;
        }
        else
        {
            if (a != right->type)
            {
                cout << "Fail to match variables." << endl;
                break;
            }
            Value *dst = context.getSymbolValue(lvar->name);
            Value *rvar = right->codeGen(context);
            context.builder.CreateStore(rvar, dst);
            return dst;
        }
        break;
    }

    case TYPE_ARRIDX:
    {
        break;
    }

    default:
        break;
    }

    //TODO: right->type bug(BinOp).
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
