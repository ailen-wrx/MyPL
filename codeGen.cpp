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
    int intIndex = int(((ConstantFP *)index->codeGen(context))->getValue().convertToDouble());

    if (arrName != string(""))
    {
        NExp *target = context.vars[arrName];
        if (target->type != TYPE_ARR)
        {
            cout << "Not an array" << endl;
            return NULL;
        }
        NArray *arr = static_cast<NArray *>(target);

        return arr->elements[intIndex];
    }
    else
    {
        NArray *thisArray = (NArray *)super->getTarget(context);
        return thisArray->elements[intIndex];
    }
}

Value *NArrayIndex::codeGen(CodeGenContext &context)
{
    return getTarget(context)->codeGen(context);
}

void NArrayIndex::modify(CodeGenContext &context, NExp *newVal)
{
    int intIndex = int(((ConstantFP *)index->codeGen(context))->getValue().convertToDouble());
    if (arrName != string(""))
    {
        NExp *target = context.vars[arrName];
        if (target->type != TYPE_ARR)
            cout << "Not an array" << endl;

        NArray *arr = static_cast<NArray *>(target);

        arr->elements[intIndex] = newVal;
    }
    else
    {
        NArray *thisArray = (NArray *)super->getTarget(context);
        thisArray->elements[intIndex] = newVal;
    }
}

Value *NBinOp::codeGen(CodeGenContext &context)
{
    Log("====");
    if (op == '=')
    {
        Log("enter");
        NExp *newVal;
        switch (right->type)
        {
        case TYPE_BINOP:
        case TYPE_CALL:
            newVal = new NNum(((ConstantFP *)right->codeGen(context))->getValue().convertToDouble());
            break;
        case TYPE_ARRIDX:
            NArrayIndex *r = static_cast<NArrayIndex *>(right);
            newVal = r->getTarget(context);
            break;
        case TYPE_VAR:
            newVal = context.vars[((NVariable *)right)->name];
            break;
        default:
            newVal = right;
        }
        switch (left->type)
        {
        case TYPE_VAR:
            NVariable *lvar = static_cast<NVariable *>(left);
            context.vars[lvar->name] = newVal;
            cout << left->toString() << " assigned " << context.vars[lvar->name]->toString() << endl;
            break;
        case TYPE_ARRIDX:
            NArrayIndex *larridx = static_cast<NArrayIndex *>(left);
            larridx->modify(context, newVal);
        }
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

    PointerType::get(args.size()
}

Value *NRetStmt::codeGen(CodeGenContext &context)
{
    return retVal->codeGen(context);
}
