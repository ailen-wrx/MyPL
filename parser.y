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
	vector<string>* stringVec;
	vector<NExp *>* NExpVec;
	NArray* array;
	NArrayIndex* index;
	NCallFunc* call;
	NStmt* stmt;
	NExp* exp;
	std::string* str;
    double number;
	int token;
}

%token <str> TVAR TSTRING
%token <token> TEQUAL TPLUS TMINUS TMUL TDIV TMOD
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE 
%token <token> TIF TELSE TFOR TRETURN TDEF TWHILE
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET TCOMMA TCOLON
%token <number> TNUMBER

%type <block> program stmts blk
%type <exp> expr 
%type <stmt> stmt ifstmt whilestmt funcdef
%type <stringVec> funcargs
%type <NExpVec> funcvars
%type <array> arrayelements
%type <index> arrayindex
%type <call> callfunc
%type <token> comparison


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
	| TRETURN expr { $$ = new NRetStmt($2); }
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
	%empty { $$=new vector<string>();}
	| TVAR { $$=new vector<string>(); $$->push_back(*$1); }
	| funcargs TCOMMA TVAR { $$=$1; $$->push_back(*$3); }
	;

ifstmt: 
	TIF TLPAREN expr TRPAREN blk TELSE blk { $$=new NIfStmt($3,$5,$7); }
	;

whilestmt: 
	TWHILE TLPAREN expr TRPAREN blk {$$=new NWhileStmt($3,$5);} 
	;

expr: 
	TVAR TEQUAL expr { $$ = new NBinOp($2, new NVariable(*$1),$3);}
	| expr TPLUS expr { $$ = new NBinOp($2, $1, $3); }
	| expr TMINUS expr { $$ = new NBinOp($2, $1, $3);}
	| expr TMUL expr { $$ = new NBinOp($2, $1, $3); }
	| expr TDIV expr { $$ = new NBinOp($2, $1, $3); }
	| expr comparison expr { $$ = new NBinOp($2, $1, $3); }
	| TNUMBER { $$ = new NNum($1); }
	| TSTRING { $$ = new NStr(*$1); }
	| TVAR   { $$ = new NVariable(*$1);}
	| TLBRACKET arrayelements TRBRACKET { $$ = $2; }
	| arrayindex { $$ = $1; }
	| callfunc { $$ = $1; }
	;

comparison:
	TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
	;

arrayelements: 
	%empty { $$ = new NArray(); }
	| TNUMBER { $$ = new NArray(); $$->elements.push_back(new NNum($1)); }
	| TSTRING { $$ = new NArray(); $$->elements.push_back(new NStr(*$1)); }
	| TLBRACKET arrayelements TRBRACKET { $$ = new NArray(); $$->elements.push_back($2); }
	| arrayelements TCOMMA TNUMBER { $$ = $1; $$->elements.push_back(new NNum($3)); }
	| arrayelements TCOMMA TSTRING { $$ = $1; $$->elements.push_back(new NStr(*$3)); }
	| arrayelements TCOMMA TLBRACKET arrayelements TRBRACKET { $$ = $1; $$->elements.push_back($4); }
	;

arrayindex:
	TVAR TLBRACKET expr TRBRACKET { $$ = new NArrayIndex(*$1, $3); }
	| arrayindex TLBRACKET expr TRBRACKET { $$ = new NArrayIndex($1, $3); }
	;

callfunc:
	TVAR TLPAREN funcvars TRPAREN { $$ = new NCallFunc(*$1, *$3); }
	;

funcvars:
	%empty { $$ = new vector<NExp *>(); }
	| expr { $$ = new vector<NExp *>(); $$->push_back($1); }
	| funcvars TCOMMA expr { $$ = $1; $$->push_back($3); }
	;

%%