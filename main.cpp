#include "codeGen.h"
#include "objGen.h"
#include "parser.hpp"
#include "binop.h"

extern NBlock *programBlock;
extern int yyparse();

int main()
{
    yyparse();
    cout << "[LOG]  Parsing Successful" << endl;

    CodeGenContext context;
    initializeBinaryOperation();

    // Intermediate code generation.
    context.generateCode(*programBlock);

    // Object code generation.
    objGen(context);
    return 0;
}