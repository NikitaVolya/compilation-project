%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"
#include "errors.h"


/** @brief Current column in the input line. */
int current_column = 1;

/** @brief Starting column of the matched token. */
int token_column = 1;



#define YY_USER_ACTION \
    do { \
        token_column = current_column; \
        current_column += yyleng; \
    } while (0);

%}

%option noyywrap yylineno

DIGIT   [0-9]
LETTER  [a-zA-Z_]
ID      [a-zA-Z]({LETTER}|{DIGIT})*

%%

[ \t]+          ;


"true"                 { yylval.boolean = 1; return BOOLEAN; }
"false"                { yylval.boolean = 0; return BOOLEAN; }

"typeconst"            { return TYPECONST; }
"const"                { return CONST; }

("exp"|"log"|"cos"|"sin"|"sqrt"|"len"|"not") { yylval.str = strdup(yytext); return SYS_FUNC; }
"list"                                       { yylval.str = strdup(yytext); return LIST; }
"sum"                                        { return SUM; }

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

"="                    { return ASSIGNE; }

"=="                   { return EQEQ; }
"!="                   { return NOTEQEQ; }
">="                   { return GE; }
"<="                   { return LE; }
">"                    { return GRATER; }
"<"                    { return LOWER; }


"("                    { return LPAREN; }
")"                    { return RPAREN; }

\n                     {
                            current_column = 1;
                            return NEWLINE;
                       }

{ID}                   { yylval.str = strdup(yytext);   return ID; }
{DIGIT}+               { yylval.num = atoi(yytext);     return NUM; }
{DIGIT}+\.{DIGIT}+     { yylval.decimal = atof(yytext); return DECIMAL; }

                       . {
                        lexical_error_occurred = 1;

                        report_error(ERROR_LEXICAL, yylineno, token_column,
                                    "Caractère inattendu '%s'", yytext);

                        return LEX_ERROR;
                       }

%%