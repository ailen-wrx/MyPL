%{
#include "codeGen.h"
#include  <iostream>
#include "parser.hpp"

using namespace std;
%}

%option noyywrap

%%
[a-zA-Z_][a-zA-Z0-9_]*  {yylval.string = new string(yytext); return TVAR; }
[0-9]+\.[0-9]*          {sscanf(yytext,"%lf",&(yylval.number)); return TNUMBER; }
[0-9]+                  {sscanf(yytext,"%lf",&(yylval.number)); return TNUMBER; }
"+"                     {return TPLUS; }
"-"                     {return TMINUS; }
"*"                     {return TMUL; }
"/"                     {return TDIV; }
"="                     {return TEQUAL; }
.                        ;
%%