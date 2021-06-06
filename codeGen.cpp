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
        cout << "Unknown variable name" << endl;
        ;
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

NExp *NArrayIndex::getTarget(CodeGenContext &context)
{
    NExp *target = context.vars[arrName];
    if (target->type != TYPE_ARR)
    {
        cout << "Not an array" << endl;
        return NULL;
    }
    NArray *arr = static_cast<NArray *>(target);

    int intIndex = int(((ConstantFP *)index->codeGen(context))->getValue().convertToDouble());
    return arr->elements[intIndex];
}

Value *NArrayIndex::codeGen(CodeGenContext &context)
{
    return getTarget(context)->codeGen(context);
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    Log("====");
    if (op == '=')
    {
        Log("enter");
        if (left->type == TYPE_VAR)
        {
            NVariable *l = static_cast<NVariable *>(left);
            switch (right->type)
            {
            case TYPE_BINOP:
            case TYPE_CALL:
                context.vars[l->name] =
                    new NNum(((ConstantFP *)right->codeGen(context))->getValue().convertToDouble());
                break;
            case TYPE_ARRIDX:
                NArrayIndex *r = static_cast<NArrayIndex *>(right);
                context.vars[l->name] = r->getTarget(context);
                break;
            case TYPE_VAR:
                context.vars[l->name] = context.vars[((NVariable *)right)->name];
                break;

            default:
                context.vars[l->name] = right;
            }
        }
        else if (left->type = TYPE_ARRIDX)
        {
                }

        cout << left->toString() << " assigned " << context.vars[l->name]->toString() << endl;

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

Value *NCallFunc::codeGen(CodeGenContext &context)
{
}

Value *NBlock::codeGen(CodeGenContext &context)
{
}

Value *NIfStmt::codeGen(CodeGenContext &context)
{
}

Value *NWhileStmt::codeGen(CodeGenContext &context)
{
}

Value *NFuncDef::codeGen(CodeGenContext &context)
{
}

Value *NRetStmt::codeGen(CodeGenContext &context)
{
    return retVal->codeGen(context);
}
