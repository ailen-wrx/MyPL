#include "codeGen.h"
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
}