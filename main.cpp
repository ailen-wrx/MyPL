#include "codeGen.h"
#include "parser.hpp"

extern Node *pro;



int main()
{
    yyparse();
    cout << "Succeed parsed" << endl;
    
    CodeGenContext context;
    std::vector<Type *> sysArgs;
    FunctionType *mainFuncType = FunctionType::get(Type::getVoidTy(context.llvmcontext), makeArrayRef(sysArgs), false);
    Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main");
    BasicBlock *block = BasicBlock::Create(context.llvmcontext, "entry", mainFunc);

    // context.builder.SetInsertPoint(block);



    pro->codeGen(context);
}