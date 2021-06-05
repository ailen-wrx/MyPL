#include "codeGen.h"

Value *NVariable::codeGen(CodeGenContext &context)
{
    cout << "var" << endl;
    Value *V = context.vars[name];
    if (!V)
    {
        cout << "Unknown variable name";
        V = context.vars[name] = (new NDouble(0))->codeGen(context);
    }
    return V;
}

Value *NDouble::codeGen(CodeGenContext &context)
{
    cout << "double" << endl;
    return ConstantFP::get(Type::getDoubleTy(context.llvmcontext), value);
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    cout << "====" << endl;
    if (op == '=')
    {
        cout << "enter" << endl;
        NVariable *l = static_cast<NVariable *>(left);
        Value *r = right->codeGen(context);
        Value *variable = context.vars[l->name];
        context.builder.CreateStore(r, variable);

        cout << l->name << " assigned " << (static_cast<ConstantInt *>(r)->getValue()).getLimitedValue();

        return r;
    }

    cout << "out" << endl;
    Value *L = left->codeGen(context);
    Value *R = right->codeGen(context);
    switch (op)
    {
    case '+':
        return context.builder.CreateAdd(L, R, "addtmp");
    case '-':
        return context.builder.CreateSub(L, R, "subtmp");
    case '*':
        return context.builder.CreateMul(L, R, "multmp");
    case '/':
        return context.builder.CreateSDiv(L, R, "divtmp");
    }
}