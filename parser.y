%{
#include "codeGen.h"
#include <memory>
int yydebug = 1;

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
	int intval;
	char ch;
	int token;
}

%token <str> TVAR TSTRING
%token <token> TEQUAL TPLUS TMINUS TMUL TDIV TMOD
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TAND TOR
%token <token> TIF TELSE TFOR TRETURN TDEF TWHILE TEXTERN
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET TCOMMA TCOLON TSEMICOLON
%token <number> TDOUBLE
%token <intval> TINT
%token <ch> TCHAR

%type <block> program stmts blk
%type <exp> expr boolexpr
%type <stmt> stmt ifstmt whilestmt funcdef
%type <stringVec> funcargs
%type <NExpVec> funcvars
%type <array> arrayelements arraydecl
%type <index> arrayindex
%type <call> callfunc
%type <token> comparison


%left TEQUAL
%left TPLUS TMINUS
%left TMUL TDIV
%left TCEQ TCNE TCLT TCLE TCGT TCGE 
%left TAND TOR

%start program

%%
program: 
	stmts {programBlock = $1;};

stmts: 
	stmt { $$ = new NBlock(); $$->statements.push_back($1); }
	| stmts stmt { $$=$1; $$->statements.push_back($2); }
	;

stmt: 
	expr TSEMICOLON { $$=$1;}
	| TRETURN expr TSEMICOLON { $$ = new NRetStmt($2); }
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
	| TEXTERN TVAR TLPAREN funcargs TRPAREN TSEMICOLON { $$ = new NFuncDef(*$2,$4); }
	;

funcargs: 
	%empty { $$=new vector<string>();}
	| TVAR { $$=new vector<string>(); $$->push_back(*$1); }
	| funcargs TCOMMA TVAR { $$=$1; $$->push_back(*$3); }
	;

ifstmt: 
	TIF TLPAREN boolexpr TRPAREN blk TELSE blk { $$=new NIfStmt($3,$5,$7); }
	;

whilestmt: 
	TWHILE TLPAREN boolexpr TRPAREN blk {$$=new NWhileStmt($3,$5);} 
	;

expr: 
	expr TEQUAL expr { $$ = new NBinOp($2, $1, $3);}
	| expr TPLUS expr { $$ = new NBinOp($2, $1, $3); }
	| expr TMINUS expr { $$ = new NBinOp($2, $1, $3);}
	| expr TMUL expr { $$ = new NBinOp($2, $1, $3); }
	| expr TDIV expr { $$ = new NBinOp($2, $1, $3); }
	| TDOUBLE { $$ = new NDouble($1); }
	| TINT { $$ = new NInt($1); }
	| TSTRING { $$ = new NStr(*$1); }
	| TVAR   { $$ = new NVariable(*$1);}
	| TLBRACE arrayelements TRBRACE { $$ = $2; }
	| TLBRACKET arraydecl TRBRACKET { $$ = $2; }
	| arrayindex { $$ = $1; }
	| callfunc { $$ = $1; }
	;

boolexpr:
	expr comparison expr { $$ = new NBinOp($2, $1, $3); }
	| boolexpr TAND boolexpr { $$ = new NBinOp($2, $1, $3); }
	| boolexpr TOR boolexpr { $$ = new NBinOp($2, $1, $3); }
	;

comparison:
	TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE | TAND | TOR
	;

arrayelements: 
	%empty { }
	| TDOUBLE { }
	| TSTRING {  }
	| TINT { }
	| TLBRACKET arrayelements TRBRACKET {  }
	| arrayelements TCOMMA TDOUBLE {  }
	| arrayelements TCOMMA TSTRING {  }
	| arrayelements TCOMMA TLBRACKET arrayelements TRBRACKET {  }
	;

arraydecl: 
	%empty { }
	| TINT { $$ = new NArray($1); }
	;

arrayindex:
	TVAR TLBRACKET expr TRBRACKET { $$ = new NArrayIndex(*$1, $3); }
	| arrayindex TLBRACKET expr TRBRACKET { }
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