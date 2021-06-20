#include "builtin.h"

map<string, Value *(*)(CodeGenContext &context, vector<NExp *> &args)> BuiltinFunction;

Value *endlineValue;

Value *BuiltinPrintf(CodeGenContext &context, vector<NExp *> &args)
{
    Function *calleeFunc = context.module.getFunction("printf");
    vector<Value *> argsVec;
    for (NExp *arg : args)
    {
        argsVec.push_back(arg->codeGen(context));
        if (!argsVec.back())
            return nullptr;
    }
    return context.builder.CreateCall(calleeFunc, argsVec, "callprint");
}

Value *BuiltinScanf(CodeGenContext &context, vector<NExp *> &args)
{
    Function *calleeFunc = context.module.getFunction("scanf");

    // Create space for the incoming value.
    AllocaInst *container =
        context.builder.CreateAlloca(Type::getInt32Ty(context.llvmcontext), nullptr, "temp");
    vector<Value *> argsVec;
    argsVec.push_back(args[0]->codeGen(context));
    argsVec.push_back(container);

    Value *ret = context.builder.CreateCall(calleeFunc, argsVec);

    Value *val = context.builder.CreateLoad(container);
    Value *dst;

    // Implement assignment to the value in the function arguments.
    switch (args[1]->type)
    {

    case TYPE_VAR:
    {
        NVariable *targetVar = static_cast<NVariable *>(args[1]);
        int targetVarType = context.getType(targetVar->name);
        if (targetVarType == -1)
        {
            // Not found. `targetVar` undefined.
            context.getCurrentBlock()->localVarTypes[targetVar->name] = TYPE_INT;

            Type *type = context.typeToLLVMType(TYPE_INT);
            dst = context.builder.CreateAlloca(type);
            context.getCurrentBlock()->localVars[targetVar->name] = dst;
            context.builder.CreateStore(val, dst);
        }
        else
        {
            dst = context.getSymbolValue(targetVar->name);
            context.builder.CreateStore(val, dst);
        }
        break;
    }

    case TYPE_ARRIDX:
    {
        NArrayIndex *targetArridx = static_cast<NArrayIndex *>(args[1]);
        NArray *targetArray = context.arrays[targetArridx->arrName];

        targetArridx->modify(context, val);
        break;
    }

    default:
        break;
    }

    return ret;
}

Value *BuiltinEndline(CodeGenContext &context, vector<NExp *> &args)
{
    // thie function works the same way as `printf("\n")` .
    Function *calleeFunc = context.module.getFunction("printf");
    vector<Value *> argsVec{endlineValue};
    return context.builder.CreateCall(calleeFunc, argsVec);
}

void initializeBuiltinFunction(CodeGenContext &context)
{
    // Function declaration for printf and scanf.
    std::vector<Type *> inoutFuncArgs = {Type::getInt8PtrTy(context.llvmcontext)};
    FunctionType *inoutFuncType = FunctionType::get(context.builder.getInt32Ty(), inoutFuncArgs, true);

    Function::Create(inoutFuncType, Function::ExternalLinkage, "printf", context.module);
    Function::Create(inoutFuncType, Function::ExternalLinkage, "scanf", context.module);

    endlineValue = context.builder.CreateGlobalString("\n", "endline");

    // Add function pointer to the `map`.
    BuiltinFunction["printf"] = BuiltinPrintf;
    BuiltinFunction["scanf"] = BuiltinScanf;
    BuiltinFunction["endline"] = BuiltinEndline;
}