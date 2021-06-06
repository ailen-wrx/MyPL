%{
#include "node.h"
#include  <iostream>
#include "parser.hpp"

using namespace std;
%}

%option noyywrap

%%
"#".*                   ;
[ \t\r\n]				;
"if"                    return TIF;
"else"                  return TELSE;
"return"                return TRETURN;
"while"                 return TWHILE;
"def"                   return TDEF;
[a-zA-Z_][a-zA-Z0-9_]*  {yylval.string = new string(yytext); return TVAR; }
[0-9]+\.[0-9]*          {sscanf(yytext,"%lf",&(yylval.number)); return TNUMBER; }
[0-9]+                  {sscanf(yytext,"%lf",&(yylval.number)); return TNUMBER; }
\"(\\.|[^"])*\"         {yylval.string = new string(yytext); return TSTRING; }
"+"                     return TPLUS;
"-"                     return TMINUS;
"*"                     return TMUL;
"/"                     return TDIV;
"%"                     return TMOD;
"="                     return TEQUAL;
"=="					return TCEQ;
"!="                    return TCNE;
"<"                     return TCLT;
"<="                    return TCLE;
">"                     return TCGT;
">="                    return TCGE;
"("                     return TLPAREN;
")"                     return TRPAREN;
"{"                     return TLBRACE;
"}"                     return TRBRACE;
"["                     return TLBRACKET;
"]"                     return TRBRACKET;
","                     return TCOMMA;
":"                     return TCOLON;

.                        ;
%%