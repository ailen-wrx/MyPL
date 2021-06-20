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
    Value *ret;
    if (args[1]->type == TYPE_CHARARR)
    {
        vector<Value *> argsVec;
        argsVec.push_back(args[0]->codeGen(context));
        argsVec.push_back(args[1]->codeGen(context));

        ret = context.builder.CreateCall(calleeFunc, argsVec, "callscanf");
    }
    else
    {
        // Create space for the incoming value.
        AllocaInst *container =
            context.builder.CreateAlloca(Type::getInt32Ty(context.llvmcontext), nullptr, "temp");
        vector<Value *> argsVec;
        argsVec.push_back(args[0]->codeGen(context));
        argsVec.push_back(container);

        ret = context.builder.CreateCall(calleeFunc, argsVec, "callscanf");

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
        }
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

Value *BuiltinGets(CodeGenContext &context, vector<NExp *> &args)
{
    Function *calleeFunc = context.module.getFunction("gets");
    vector<Value *> argsVec;
    for (NExp *arg : args)
    {
        argsVec.push_back(arg->codeGen(context));
        if (!argsVec.back())
            return nullptr;
    }
    return context.builder.CreateCall(calleeFunc, argsVec, "callgets");
}

Value *BuiltinStrchr(CodeGenContext &context, vector<NExp *> &args)
{
    Function *calleeFunc = context.module.getFunction("strchr");
    vector<Value *> argsVec;
    for (NExp *arg : args)
    {
        argsVec.push_back(arg->codeGen(context));
        if (!argsVec.back())
            return nullptr;
    }
    return context.builder.CreateCall(calleeFunc, argsVec, "callstrchr");
}

Value *BuiltinMemset(CodeGenContext &context, vector<NExp *> &args)
{
    Function *calleeFunc = context.module.getFunction("memset");
    vector<Value *> argsVec;
    for (NExp *arg : args)
    {
        argsVec.push_back(arg->codeGen(context));
        if (!argsVec.back())
            return nullptr;
    }
    return context.builder.CreateCall(calleeFunc, argsVec, "callmemset");
}

Value *BuiltinStrncpy(CodeGenContext &context, vector<NExp *> &args)
{
    Function *calleeFunc = context.module.getFunction("strncpy");
    vector<Value *> argsVec;
    for (NExp *arg : args)
    {
        argsVec.push_back(arg->codeGen(context));
        if (!argsVec.back())
            return nullptr;
    }
    return context.builder.CreateCall(calleeFunc, argsVec, "callstrncpy");
}

Value *BuiltinSscanf(CodeGenContext &context, vector<NExp *> &args)
{
    Function *calleeFunc = context.module.getFunction("sscanf");
    Value *ret;
    if (args[2]->type == TYPE_CHARARR)
    {
        vector<Value *> argsVec;
        argsVec.push_back(args[0]->codeGen(context));
        argsVec.push_back(args[1]->codeGen(context));
        argsVec.push_back(args[2]->codeGen(context));

        ret = context.builder.CreateCall(calleeFunc, argsVec, "callsscanf");
    }
    else
    {
        // Create space for the incoming value.
        AllocaInst *container =
            context.builder.CreateAlloca(Type::getInt32Ty(context.llvmcontext), nullptr, "temp");
        vector<Value *> argsVec;
        argsVec.push_back(args[0]->codeGen(context));
        argsVec.push_back(args[1]->codeGen(context));
        argsVec.push_back(container);

        ret = context.builder.CreateCall(calleeFunc, argsVec, "callsscanf");

        Value *val = context.builder.CreateLoad(container);
        Value *dst;

        // Implement assignment to the value in the function arguments.
        switch (args[2]->type)
        {

        case TYPE_VAR:
        {
            NVariable *targetVar = static_cast<NVariable *>(args[2]);
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
            NArrayIndex *targetArridx = static_cast<NArrayIndex *>(args[2]);
            NArray *targetArray = context.arrays[targetArridx->arrName];

            targetArridx->modify(context, val);
            break;
        }
        }
    }
    return ret;
}

Value *BuiltinStrcmp(CodeGenContext &context, vector<NExp *> &args)
{
    Function *calleeFunc = context.module.getFunction("strcmp");
    vector<Value *> argsVec;
    for (NExp *arg : args)
    {
        argsVec.push_back(arg->codeGen(context));
        if (!argsVec.back())
            return nullptr;
    }
    return context.builder.CreateCall(calleeFunc, argsVec, "callstrcmp");
}

void initializeBuiltinFunction(CodeGenContext &context)
{
    // Function declaration for printf and scanf.
    std::vector<Type *> inoutFuncArgs = {Type::getInt8PtrTy(context.llvmcontext)};
    FunctionType *inoutFuncType = FunctionType::get(context.builder.getInt32Ty(), inoutFuncArgs, true);

    Function::Create(inoutFuncType, Function::ExternalLinkage, "printf", context.module);
    Function::Create(inoutFuncType, Function::ExternalLinkage, "scanf", context.module);
    Function::Create(inoutFuncType, Function::ExternalLinkage, "gets", context.module);
    Function::Create(inoutFuncType, Function::ExternalLinkage, "memset", context.module);
    Function::Create(inoutFuncType, Function::ExternalLinkage, "sscanf", context.module);
    Function::Create(inoutFuncType, Function::ExternalLinkage, "strcmp", context.module);

    endlineValue = context.builder.CreateGlobalString("\n", "endline");

    std::vector<Type *> strchrFuncArgs = {Type::getInt8PtrTy(context.llvmcontext), Type::getInt8Ty(context.llvmcontext)};
    FunctionType *strchrFuncType = FunctionType::get(context.builder.getInt32Ty(), strchrFuncArgs, true);
    Function::Create(strchrFuncType, Function::ExternalLinkage, "strchr", context.module);

    std::vector<Type *> strncpyFuncArgs = {Type::getInt8PtrTy(context.llvmcontext), Type::getInt8PtrTy(context.llvmcontext),
                                           Type::getInt32Ty(context.llvmcontext)};
    FunctionType *strncpyFuncType = FunctionType::get(context.builder.getInt32Ty(), strchrFuncArgs, true);
    Function::Create(strchrFuncType, Function::ExternalLinkage, "strncpy", context.module);

    // Add function pointer to the `map`.
    BuiltinFunction["printf"] = BuiltinPrintf;
    BuiltinFunction["scanf"] = BuiltinScanf;
    BuiltinFunction["endline"] = BuiltinEndline;
    BuiltinFunction["gets"] = BuiltinGets;
    BuiltinFunction["strchr"] = BuiltinStrchr;
    BuiltinFunction["memset"] = BuiltinMemset;
    BuiltinFunction["strncpy"] = BuiltinStrncpy;
    BuiltinFunction["sscanf"] = BuiltinSscanf;
    BuiltinFunction["strcmp"] = BuiltinStrcmp;
}