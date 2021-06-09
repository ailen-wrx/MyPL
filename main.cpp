#include "node.h"
#include "codeGen.h"
#include "parser.hpp"

extern NBlock *programBlock;
extern int yyparse();

int main()
{
    yyparse();
    cout << "[LOG]  Parsing Successful" << endl;

    CodeGenContext context;
    context.generateCode(*programBlock);
}