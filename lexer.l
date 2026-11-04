%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

%}

%option noyywrap

DIGIT   [0-9]
LETTER  [a-zA-Z_]
ID      [a-zA-Z]({LETTER}|{DIGIT})*

%%

[ \t]+          ;


"true"                 { yylval.boolean = 1; return BOOLEAN; }
"false"                { yylval.boolean = 0; return BOOLEAN; }

"typeconst"            { return TYPECONST; }
"const"                { return CONST; }

"exp" | "log" | "cos" | "sin" | "sqrt" | "sum" | "len"   { yylval.str = strdup(yytext); return SYS_FUNC; }
"list"                 { yylval.str = strdup(yytext); return LIST; }

"quit"                 { return QUIT; }
"symbols"              { return SYMBOLS; }
"delete"               { return DELETE; }


"+"                    { return PLUS; }
"-"                    { return MINUS; }
"/"                    { return DEVIDE; }
"*"                    { return MULT; }
"^"                    { return POW; }

"and"                  { return AND; }
"or"                   { return OR;  }
"not"                  { return NOT; }

"="                    { return ASSIGNE; }

"=="                   { return EQEQ; }
"!="                   { return NOTEQEQ; }
">="                   { return GE; }
"<="                   { return LE; }
">"                    { return GRATER; }
"<"                    { return LOWER; }


"("                    { return LPAREN; }
")"                    { return RPAREN; }

\n                     { return NEWLINE; }

{ID}                   { yylval.str = strdup(yytext);   return ID; }
{DIGIT}+               { yylval.num = atoi(yytext);     return NUM; }
{DIGIT}+\.{DIGIT}+     { yylval.decimal = atof(yytext); return DECIMAL; }

.                      { fprintf(stderr, "Erreur lexicale : caractere inattendu '%s'\n", yytext); }

%%