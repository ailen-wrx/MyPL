%{
#include "codeGen.h"
#include <memory>

Node* pro;

void yyerror(const char* s)
{
	printf("Error: %s\n", s);
}

extern int yylex();
%}

%union
{
    Node* node;
	std::string* string;
    double number;
	int token;
}

%token <string> TVAR
%token <token> TEQUAL TPLUS TMINUS TMUL TDIV
%token <number> TNUMBER
%type <node> program stmt rstmt

%left TEQUAL
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%
program: stmt {pro = $1;}

stmt: TVAR TEQUAL rstmt { $$ = new NBinOp('=',new NVariable(*$1),$3);}

rstmt: rstmt TPLUS rstmt { $$ = new NBinOp('+',$1,$3); }
| rstmt TMINUS rstmt { $$ = new NBinOp('-',$1,$3);}
| rstmt TMUL rstmt { $$ = new NBinOp('*',$1,$3); }
| rstmt TDIV rstmt { $$ = new NBinOp('/',$1,$3); }
| TNUMBER          { $$ = new NDouble($1); }
%%