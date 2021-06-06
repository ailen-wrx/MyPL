#include "codeGen.h"
using namespace std;

void Log(string str) 
{
    cout << "[LOG]  " << str << endl;
}
void Log(string str1, double str2) 
{
    cout << "[LOG]  " << str1 << str2 << endl;
}

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
    Log("Double: ", value);
    return ConstantFP::get(Type::getDoubleTy(context.llvmcontext), value);
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    Log("====");
    if (op == '=')
    {
        Log("enter");
        NVariable *l = static_cast<NVariable *>(left);
        Value *lC = lC = l->codeGen(context);
        Value *r = r = right->codeGen(context);

        /* Declare variable according to the type of r */
        Type* type = llvm::Type::getFloatTy(context.llvmcontext);
        context.builder.CreateLoad(lC, false, "");

        
        Value* initial = nullptr;
        Value* inst = context.builder.CreateAlloca(type);
        context.vars.insert(pair<string, Value *> (l->name, inst));

        /* dst -> 0x00: No variable declared yet. */
        Value *dst = context.vars.find(l->name)->second;
        // context.vars[l->name] = r;
        context.builder.CreateStore(r, dst);
        Log("Assigned: ", (((ConstantFP *)r)->getValue()).convertToDouble());

        return r;
    }

    Log("out");
    Value *L = left->codeGen(context);
    Log("Left: ", (((ConstantFP *)L)->getValue()).convertToDouble());
    Value *R = right->codeGen(context);
    Log("Right: ", (((ConstantFP *)R)->getValue()).convertToDouble());
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