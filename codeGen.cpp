#include "node.h"

Value *NVariable::codeGen(CodeGenContext &context)
{
    cout << "var:" << name << endl;
    Value *V = context.vars[name]->codeGen(context);
    if (!V)
    {
        cout << "Unknown variable name";
    }
    return V;
}

Value *NNum::codeGen(CodeGenContext &context)
{
    cout << "double" << value << endl;
    return ConstantFP::get(Type::getDoubleTy(context.llvmcontext), value);
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    cout << "====" << endl;
    if (op == '=')
    {
        cout << "enter" << endl;
        NVariable *l = static_cast<NVariable *>(left);
        switch (right->type)
        {
        case TYPE_BINOP:
        case TYPE_STR:
            context.vars[l->name] =
                new NNum(((ConstantFP *)right->codeGen(context))->getValue().convertToDouble());
            break;
        case TYPE_VAR:
            context.vars[l->name] = context.vars[((NVariable *)right)->name];
            break;

        default:
            context.vars[l->name] = right;
        }

        cout << l->name << " assigned " << context.vars[l->name]->toString() << endl;

        return l->codeGen(context);
    }

    cout << "out" << endl;
    Value *L = left->codeGen(context);
    cout << " left is " << (((ConstantFP *)L)->getValue()).convertToDouble() << endl;
    Value *R = right->codeGen(context);
    cout << " right is " << (((ConstantFP *)R)->getValue()).convertToDouble() << endl;
    switch (op)
    {
    case '+':
        return context.builder.CreateFAdd(L, R, "addtmp");
    case '-':
        return context.builder.CreateFSub(L, R, "subtmp");
    case '*':
        return context.builder.CreateFMul(L, R, "multmp");
    case '/':
        return context.builder.CreateFDiv(L, R, "divtmp");
    }
}