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
    if (op == TEQUAL)
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
            break;
        default:
            cout << "Invalid assignment" << endl;
        }
    }

    Log("out");
    Value *L = left->codeGen(context);
    Log("Left", (((ConstantFP *)L)->getValue()).convertToDouble());
    Value *R = right->codeGen(context);
    Log("Right", (((ConstantFP *)R)->getValue()).convertToDouble());
    switch (op)
    {
    case TPLUS:
        return context.builder.CreateFAdd(L, R, "addtmp");
    case TMINUS:
        return context.builder.CreateFSub(L, R, "subtmp");
    case TMUL:
        return context.builder.CreateFMul(L, R, "multmp");
    case TDIV:
        return context.builder.CreateFDiv(L, R, "divtmp");
    case TCLT:
        return context.builder.CreateFCmpULT(L, R, "cmpftmp");
    case TCLE:
        return context.builder.CreateFCmpOLE(L, R, "cmpftmp");
    case TCGE:
        return context.builder.CreateFCmpOGE(L, R, "cmpftmp");
    case TCGT:
        return context.builder.CreateFCmpOGT(L, R, "cmpftmp");
    case TCEQ:
        return context.builder.CreateFCmpOEQ(L, R, "cmpftmp");
    case TCNE:
        return context.builder.CreateFCmpONE(L, R, "cmpftmp");
    }
}

Value *NCallFunc::codeGen(CodeGenContext &context)
{
    NFuncDef *f = context.functions[funcName];
    if (f->args.size() != this->args.size())
    {
        cout << "function unmatched." << endl;
        return NULL;
    }

    vars_reserved = context.vars;
    context.vars.clear();

    int idx = 0;
    for (auto i : this->args)
        context.vars[f->args[idx]] = i;

    Value *ret = f->body->codeGen(context);

    context.vars = vars_reserved;

    return ret;
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
}

Value *NIfStmt::codeGen(CodeGenContext &context)
{
    Log("If statement");
    Value* condVal = this->cond->codeGen(context);
    int cond = static_cast<int>((((ConstantFP *)condVal)->getValue()).convertToDouble());
    if (cond == 0) {
        this->el->codeGen(context);
    } else {
        this->then->codeGen(context);
    }
}

Value *NWhileStmt::codeGen(CodeGenContext &context)
{
    Value* condVal = this->Cond->codeGen(context);
    int cond = static_cast<int>((((ConstantFP *)condVal)->getValue()).convertToDouble());
    while (cond != 0) {
        this->body->codeGen(context);
        condVal = this->Cond->codeGen(context);
        cond = static_cast<int>((((ConstantFP *)condVal)->getValue()).convertToDouble());
    }
}

Value *NFuncDef::codeGen(CodeGenContext &context)
{
    context.functions[name] = this;
}

Value *NRetStmt::codeGen(CodeGenContext &context)
{
    return retVal->codeGen(context);
}
