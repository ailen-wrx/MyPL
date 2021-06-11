#include "codeGen.h"
#include "objGen.h"
#include "parser.hpp"
#include "binop.h"
#include "builtin.h"

extern NBlock *programBlock;
extern int yyparse();

int main()
{
    yyparse();
    cout << "[LOG]  Parsing Successful" << endl;

    CodeGenContext context;
    initializeBinaryOperation();
    initializeBuiltinFunction();
    context.generateCode(*programBlock);
    objGen(context);
    return 0;
}