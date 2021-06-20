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
	vector<pair<int, string>>* intstrVec;
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
%token <token> TYPEINT TYPEDOUBLE TYPECHAR TYPESTRING
%token <token> TEQUAL TPLUS TMINUS TMUL TDIV TMOD
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TAND TOR
%token <token> TIF TELSE TFOR TRETURN TDEF TWHILE TEXTERN TGLOBAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET TCOMMA TCOLON TSEMICOLON
%token <number> TDOUBLE
%token <intval> TINT
%token <ch> TCHAR

%type <block> program stmts blk
%type <exp> expr boolexpr
%type <stmt> stmt ifstmt whilestmt funcdef
%type <intstrVec> funcargs
%type <NExpVec> funcvars
%type <array> arraydecl
%type <index> arrayindex
%type <call> callfunc
%type <token> comparison types


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
	%empty { $$=new vector<pair<int, string>>();}
	| types TVAR { $$=new vector<pair<int, string>>(); $$->push_back(make_pair($1, *$2)); }
	| funcargs TCOMMA types TVAR { $$=$1; $$->push_back(make_pair($3, *$4)); }
	;

ifstmt: 
	TIF TLPAREN boolexpr TRPAREN blk TELSE blk { $$=new NIfStmt($3,$5,$7); }
	;

whilestmt: 
	TWHILE TLPAREN boolexpr TRPAREN blk {$$=new NWhileStmt($3,$5);} 
	;

expr: 
	expr TEQUAL expr { $$ = new NBinOp(BINOP_ASSIGN, $1, $3);}
	| expr TPLUS expr { $$ = new NBinOp(BINOP_PLUS, $1, $3); }
	| expr TMINUS expr { $$ = new NBinOp(BINOP_MINUS, $1, $3);}
	| expr TMUL expr { $$ = new NBinOp(BINOP_MUL, $1, $3); }
	| expr TDIV expr { $$ = new NBinOp(BINOP_DIV, $1, $3); }
	| TDOUBLE { $$ = new NDouble($1); }
	| TINT { $$ = new NInt($1); }
	| TMINUS TINT { $$ = new NInt(-$1); }
	| TSTRING { $$ = new NStr(*$1); }
	| TVAR   { $$ = new NVariable(*$1);}
	| arraydecl { $$ = $1; }
	| arrayindex { $$ = $1; }
	| callfunc { $$ = $1; }
	;

boolexpr:
	expr comparison expr { $$ = new NBinOp($2, $1, $3); }
	| boolexpr TAND boolexpr { $$ = new NBinOp(BINOP_AND, $1, $3); }
	| boolexpr TOR boolexpr { $$ = new NBinOp(BINOP_OR, $1, $3); }
	;

comparison:
	TCEQ { $$=BINOP_CEQ; }
	| TCNE { $$=BINOP_CNE; }
	| TCLT { $$=BINOP_CLT; }
	| TCLE { $$=BINOP_CLE; }
	| TCGT { $$=BINOP_CGT; }
	| TCGE { $$=BINOP_CGE; }
	| TAND { $$=BINOP_AND; }
	| TOR  { $$=BINOP_OR; }
	;

arraydecl: 
	TGLOBAL types TLBRACKET TINT TRBRACKET { $$ = new NArray($2, $4, true); }
	| types TLBRACKET TINT TRBRACKET { $$ = new NArray($1, $3, false); }
	;

types:
	TYPEINT { $$ = TYPE_INT; }
	| TYPEDOUBLE { $$ = TYPE_DOUBLE; }
	| TYPECHAR { $$ = TYPE_CHAR; }
	| TYPESTRING { $$ = TYPE_STR; }
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