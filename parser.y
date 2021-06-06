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
	NBlock* block;
	NStmt* stmt;
	NExp* exp;
	std::string* string;
    double number;
	int token;
}

%token <string> TVAR TSTRING
%token <token> TEQUAL TPLUS TMINUS TMUL TDIV TMOD
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE 
%token <token> TIF TELSE TFOR TRETURN TDEF
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET TCOMMA TCOLON
%token <number> TNUMBER

%type <block> program stmts
%type <exp> expr
%type <stmt> stmt ifstmt whilestmt funcdef

%left TEQUAL
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%
program: 
	stmts {programBlock = $1;};

stmts: 
	stmt {}
	| stmts stmt {}
	;

stmt: 
	expr {}
	| TRETURN expr {}
	| ifstmt
	| whilestmt
	| funcdef
	;

block: 
	TLBRACE stmts TRBRACE {}
	| TLBRACE TRBRACE {}
	;

funcdef: 
	TDEF TVAR TLPAREN funcargs TRPAREN block
		{};

funcargs: 
	{}
	| TVAR {}
	| funcargs TCOMMA TVAR {}
	;

ifstmt: 
	TIF expr block {}
	| TIF expr block TELSE block {}
	| TIF expr block TELSE ifstmt {}
	;

whilestmt: 
	{} 
	;

expr: 
	TVAR TEQUAL expr {}
	| expr TPLUS expr {}
	| expr TMINUS expr {}
	| expr TMUL expr {}
	| expr TDIV expr {}
	| TNUMBER {}
	| TSTRING {}
	| /* Array */
	;

%%