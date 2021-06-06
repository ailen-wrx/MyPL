%{
#include "node.h"
#include <memory>

NBlock* programBlock;

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
	vector<string>* funcDefArgs
	NArray* array
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

%type <block> program stmts blk
%type <exp> expr
%type <stmt> stmt ifstmt whilestmt funcdef
%type <funcDefArgs> funcargs
%type <array> arr

%left TEQUAL
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%
program: 
	stmts {programBlock = $1;};

stmts: 
	stmt { $$ = new NBlock(); $$->statements.push_back($1); }
	| stmts stmt { $$=$1; $$->statements.push_back($2); }
	;

stmt: 
	expr { $$=$1;}
	| TRETURN expr { $$ = new NRetStmt($1); }
	| ifstmt { $$ = $1;}
	| whilestmt { $$ = $1;}
	| funcdef { $$ = $1;}
	;

blk: 
	TLBRACE stmts TRBRACE { $$ = $2;}
	| TLBRACE TRBRACE { $$ = new NBlock();}
	;

funcdef: 
	TDEF TVAR TLPAREN funcargs TRPAREN blk	{ $$ = new NFuncDef(*$2,$4,$6);}
	;

funcargs: 
	%empty { $$=new vector<string>;}
	| TVAR { $$=new vector<string>; $$->push_back(*$1); }
	| funcargs TCOMMA TVAR { $$=$1; $$->push_back(*$3); }
	;

ifstmt: 
	TIF TLPAREN expr TRPAREN blk TELSE blk { $$=new NIfStmt($3,$5,$7); }
	;

whilestmt: 
	TWHILE TLPAREN expr TRPAREN blk {$$=new NWhileStmt($3,$5);} 
	;

expr: 
	TVAR TEQUAL expr { $$ = new NBinOp('=', new NVariable(*$1),$3);}
	| expr TPLUS expr { $$ = new NBinOp('+', $1, $3); }
	| expr TMINUS expr { $$ = new NBinOp('-', $1, $3);}
	| expr TMUL expr { $$ = new NBinOp('*', $1, $3); }
	| expr TDIV expr { $$ = new BinOp('/', $1, $3); }
	| TNUMBER { $$ = new NNum($1); }
	| TSTRING { $$ = new NStr(*$1); }
	| TVAR   { $$ = new Variable(*$1);}
	| /* Array */
	| /* Array Index */
	| /* call function  */
	;

%%