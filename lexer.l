%{
#include "node.h"
#include "parser.hpp"
#include  <iostream>
#define TOKEN(t) ( yylval.token = t)

using namespace std;
%}

%option noyywrap

%%
"#".*                   ;
[ \t\r\n]				;
"if"                    return TOKEN(TIF);
"else"                  return TOKEN(TELSE);
"return"                return TOKEN(TRETURN);
"while"                 return TOKEN(TWHILE);
"def"                   return TOKEN(TDEF);
[a-zA-Z_][a-zA-Z0-9_]*  {yylval.str = new string(yytext); return TVAR; }
[0-9]+\.[0-9]*          {sscanf(yytext,"%lf",&(yylval.number)); return TNUMBER; }
[0-9]+                  {sscanf(yytext,"%lf",&(yylval.number)); return TNUMBER; }
\"(\\.|[^"])*\"         {yylval.str = new string(yytext); return TSTRING; }
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);
"%"                     return TOKEN(TMOD);
"="                     return TOKEN(TEQUAL);
"=="					return TOKEN(TCEQ);
"!="                    return TOKEN(TCNE);
"<"                     return TOKEN(TCLT);
"<="                    return TOKEN(TCLE);
">"                     return TOKEN(TCGT);
">="                    return TOKEN(TCGE);
"("                     return TOKEN(TLPAREN);
")"                     return TOKEN(TRPAREN);
"{"                     return TOKEN(TLBRACE);
"}"                     return TOKEN(TRBRACE);
"["                     return TOKEN(TLBRACKET);
"]"                     return TOKEN(TRBRACKET);
","                     return TOKEN(TCOMMA);
":"                     return TOKEN(TCOLON);

.                        ;
%%