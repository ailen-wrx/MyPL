%{
#include "codeGen.h"
#include "parser.hpp"
#include  <iostream>
#define TOKEN(t) ( yylval.token = t)

static FILE* yyparse_file_ptr;
void yyparse_init(const char* filename)
{
    yyparse_file_ptr = fopen(filename, "r");
    yyin = yyparse_file_ptr;
}

void yyparse_cleanup()
{
    fclose(yyparse_file_ptr);
}
using namespace std;
%}

%option noyywrap

%%
"#".*                   ;
[ \t\r\n]				;
"if"                    puts("TIF"); return TOKEN(TIF);
"else"                  puts("TELSE"); return TOKEN(TELSE);
"return"                puts("TRETURN"); return TOKEN(TRETURN);
"while"                 puts("TWHILE"); return TOKEN(TWHILE);
"def"                   puts("TDEF"); return TOKEN(TDEF);
"extern"                puts("TEXTERN"); return TOKEN(TEXTERN);
[a-zA-Z_][a-zA-Z0-9_]*  puts("TVAR"); {yylval.str = new string(yytext); return TVAR; }
[0-9]+\.[0-9]*          puts("TDOUBLE"); {sscanf(yytext,"%lf",&(yylval.number)); return TDOUBLE; }
[0-9]+                  puts("TINT"); {sscanf(yytext,"%d",&(yylval.intval)); return TINT; }
\"(\\.|[^"])*\"         puts("TSTRING"); {yylval.str = new string(yytext); return TSTRING; }
\'(\\.|[^"])\'          puts("TCHAR"); {yylval.ch = yytext[1]; return TCHAR; }
"+"                     puts("TPLUS"); return TOKEN(TPLUS);
"-"                     puts("TMINUS"); return TOKEN(TMINUS);
"*"                     puts("TMUL"); return TOKEN(TMUL);
"/"                     puts("TDIV"); return TOKEN(TDIV);
"%"                     puts("TMOD"); return TOKEN(TMOD);
"="                     puts("TEQUAL"); return TOKEN(TEQUAL);
"=="					puts("TCEQ"); return TOKEN(TCEQ);
"!="                    puts("TCNE"); return TOKEN(TCNE);
"<"                     puts("TCLT"); return TOKEN(TCLT);
"<="                    puts("TCLE"); return TOKEN(TCLE);
">"                     puts("TCGT"); return TOKEN(TCGT);
">="                    puts("TCGE"); return TOKEN(TCGE);
"("                     puts("TLPAREN"); return TOKEN(TLPAREN);
")"                     puts("TRPAREN"); return TOKEN(TRPAREN);
"{"                     puts("TLBRACE"); return TOKEN(TLBRACE);
"}"                     puts("TRBRACE"); return TOKEN(TRBRACE);
"["                     puts("TLBRACKET"); return TOKEN(TLBRACKET);
"]"                     puts("TRBRACKET"); return TOKEN(TRBRACKET);
","                     puts("TCOMMA"); return TOKEN(TCOMMA);
":"                     puts("TCOLON"); return TOKEN(TCOLON);
";"                     puts("TSEMICOLON"); return TOKEN(TSEMICOLON);

.                        ;
%%