#include "codeGen.h"
#include "node.h"
using namespace std;

void Log(string str) 
{
    cout << "[LOG]  " << str << endl;
}
void Log(string str1, double str2) 
{
    cout << "[LOG]  " << str1 << " : " << str2 << endl;
}
void Log(string str1, string str2) 
{
    cout << "[LOG]  " << str1 << " : " << str2 << endl;
}


Value *NVariable::codeGen(CodeGenContext &context)
{
    Log("Var", name);
    Value *V = context.vars[name]->codeGen(context);
    if (!V)
    {
        cout << "Unknown variable name";
    }
    return V;
}

Value *NNum::codeGen(CodeGenContext &context)
{
    Log("Double", value);
    return ConstantFP::get(Type::getDoubleTy(context.llvmcontext), value);
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    Log("====");
    if (op == '=')
    {
        Log("enter");
        NVariable *l = static_cast<NVariable *>(left);
        switch (right->type)
        {
        case TYPE_BINOP:
        case TYPE_CALL:
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

    Log("out");
    Value *L = left->codeGen(context);
    Log("Left", (((ConstantFP *)L)->getValue()).convertToDouble());
    Value *R = right->codeGen(context);
    Log("Right", (((ConstantFP *)R)->getValue()).convertToDouble());
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