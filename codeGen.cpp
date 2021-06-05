#include "codeGen.h"

Value *NVariable::codeGen(CodeGenContext &context)
{
    cout << "var:" << name << endl;
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
        Value *r = right->codeGen(context);
        context.vars[l->name] = r;
        // context.builder.CreateStore(r, context.vars[l->name]);

        cout << l->name << " assigned " << (((ConstantFP *)r)->getValue()).convertToDouble() << endl;

        return r;
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