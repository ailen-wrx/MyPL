%{
#include "node.h"
#include <memory>

Node* programBlock;

void yyerror(const char* s)
{
	printf("Error: %s\n", s);
}

extern int yylex();
%}

%union
{
    Node* node;
	NExp* exp;
	std::string* string;
    double number;
	int token;
}

%token <string> TVAR TSTRING
%token <token> TEQUAL TPLUS TMINUS TMUL TDIV TMOD
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE 
%token <token> TIF TELSE TFOR TRETURN
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET TCOMMA
%token <number> TNUMBER

c <node> program
%type <exp> stmt rstmt

%left TEQUAL
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%
program: stmt {programBlock = $1;}

stmt: TVAR TEQUAL rstmt { $$ = new NBinOp('=',new NVariable(*$1),$3);}

rstmt: rstmt TPLUS rstmt { $$ = new NBinOp('+',$1,$3); }
| rstmt TMINUS rstmt { $$ = new NBinOp('-',$1,$3);}
| rstmt TMUL rstmt { $$ = new NBinOp('*',$1,$3); }
| rstmt TDIV rstmt { $$ = new NBinOp('/',$1,$3); }
| TNUMBER          { $$ = new NNum($1); }
%%