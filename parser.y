%{
#include <stdio.h>
#include <stdlib.h>

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

int  yylex(void);
void yyerror(const char *s);
extern FILE *yyin;

int quit_flag = 0;

%}

%union {
    int num;
    char* str;
    double decimal;
}


%token <num> NUM
%token <str> ID
%token <decimal> DECIMAL

%token TRUE FALSE
%token TYPECONST CONST
%token <str> SINGTON_OP

%token QUIT SYMBOLS DELETE

%token PLUS MINUS DEVIDE MULT POW LIST
%token AND OR NOT
%token ASSIGNE
%token EQEQ NOTEQEQ GE LE GRATER LOWER
%token LPAREN RPAREN


%type program instr assigne_t
%type expr math_basic_op factor arg_list

%start program
%define parse.error verbose

%%

program
    : instr                                 { }
    | QUIT                                  { 
        quit_flag = 1;
        printf("good night world!");
    }
    ;

instr 
    : assigne_t                             { }
    | expr                                  { }
    ;

assigne_t
    : TYPECONST ID ASSIGNE expr             { }
    | CONST ID ASSIGNE expr                 { }
    | ID ASSIGNE expr                       { printf("ASSIGNE\n"); }
    ;

expr
    : LPAREN SINGTON_OP arg_list RPAREN     { }
    | LPAREN LIST arg_list RPAREN           { }
    | LPAREN math_basic_op arg_list RPAREN  { }
    | LPAREN POW arg_list RPAREN            { }
    | factor                                { }
    ;

math_basic_op
    : PLUS                                  { }
    | MINUS                                 { }
    | DEVIDE                                { }
    | MULT                                  { }
    ;

factor
    : LPAREN expr RPAREN                    { }
    | ID                                    { }
    | NUM                                   { }
    | DECIMAL                               { }
    ;

arg_list
    : expr                                  { }
    | expr arg_list                         { }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erreur syntaxique : %s\n", s);
}

int main() {
    char input[4096];
    int r = 0;

    while (!quit_flag) {
        printf(">>> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        YY_BUFFER_STATE buffer = yy_scan_string(input);

        r = yyparse();

        yy_delete_buffer(buffer);

        if (r != 0) {
            fprintf(stderr,
                "Analyse interrompue (code %d).\n", r);
        }
    }

    return r;
}