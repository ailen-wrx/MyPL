#include "codeGen.h"
#include "parser.hpp"

extern Node *pro;

CodeGenContext context;

int main()
{
    yyparse();
    cout << "Succeed parsed" << endl;
    pro->codeGen(context);
}