%{
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "sym_table.h"
#include "typecheck.h"
#include "errors.h"

typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

int  yylex(void);
void yyerror(const char *s);
extern FILE *yyin;

int quit_flag = 0;

static SymTable *root_table;
static SymTable *cur_table; 

extern int yylineno;
extern int token_column;

%}

%union {
    int num;
    char* str;
    double decimal;
    int boolean;
    struct ASTNode *ast;
}


%token <num> NUM
%token <str> ID
%token <decimal> DECIMAL

%token <boolean> BOOLEAN
%token TYPECONST CONST
%token <str> SYS_FUNC

%token QUIT SYMBOLS DELETE

%token PLUS MINUS DEVIDE MULT POW LIST SUM
%token AND OR NOT
%token ASSIGNE
%token EQEQ NOTEQEQ GE LE GRATER LOWER
%token LPAREN RPAREN
%token NEWLINE LEX_ERROR


%type <ast> program instr assigne_t
%type <ast> expr factor arg_list
%type <str> basic_op bin_op

%start program
%define parse.error verbose

%%

program
    : instr NEWLINE                         { 
        ASTNode *root = $1;
        typecheck(root, root_table);
        if (!type_equal(root->type, type_error)) {
            afficheAST(root);
        }
        freeAST(root);
    }
    | SYMBOLS NEWLINE                       {
        print_sym_table(root_table);
    }
    | DELETE ID NEWLINE                     {
        const char *id_name = $2;
        if (remove_symbol(root_table, id_name)) {
            printf("Error while removing \"%s\"\n", id_name);
        } else {
            printf("Removed %s\n", id_name);
        }
        
    }
    | QUIT NEWLINE                          { 
        quit_flag = 1;
        printf("good night world!\n");
    }
    | error NEWLINE {
        yyerrok;
    }
    ;

instr 
    : assigne_t                             { $$ = $1; }
    | expr                                  { $$ = $1; }
    ;

assigne_t
    : TYPECONST ID ASSIGNE expr             {
        $$ = makeAssign($2, $4, TYPECONST_SYM);
        $$->row = yylineno;
        $$->col = token_column;
    }
    | CONST ID ASSIGNE expr                 { 
        $$ = makeAssign($2, $4, CONST_SYM);
        $$->row = yylineno;
        $$->col = token_column;
    }
    | ID ASSIGNE expr                       { 
        $$ = makeAssign($1, $3, DEFAULT_SYM);
        $$->row = yylineno;
        $$->col = token_column;
    }
    | ID ID ASSIGNE expr {
        token_column = 1;
        report_error(ERROR_SYNTAX, yylineno, token_column,
                     "Mot-clé inconnu '%s'. Vouliez-vous écrire 'const' ou 'typeconst' ?",
                     $1);
        free($1);
        free($2);
        freeAST($4);
        YYERROR;
    }
    ;

expr
    : SYS_FUNC expr                         { 
                                              $$ = makeSysFunc($1, $2);
                                              $$->row = yylineno;
                                              $$->col = token_column;
                                            }
    | LIST arg_list                         { 
                                              $$ = makeList($2); 
                                              $$->row = yylineno;
                                              $$->col = token_column;
                                            }
    | basic_op arg_list                     { 
                                              $$ = makeBasicOp($1, $2); 
                                              $$->row = yylineno;
                                              $$->col = token_column;
                                            }
    | bin_op expr expr                      { 
                                              $$ = makeBinOp($1, $2, $3);
                                              $$->row = yylineno;
                                              $$->col = token_column;
                                            }
    | factor                                { $$ = $1; }
    ;   

basic_op
    : PLUS                                  { $$ = "+"; }
    | MINUS                                 { $$ = "-"; }
    | MULT                                  { $$ = "*"; }
    | SUM                                   { $$ = "sum"; }
    | AND                                   { $$ = "and"; }
    | OR                                    { $$ = "or"; }
    ;

bin_op
    : POW                                   { $$ = "^"; }
    | DEVIDE                                { $$ = "/"; }
    | EQEQ                                  { $$ = "=="; }
    | NOTEQEQ                               { $$ = "!="; }
    | GE                                    { $$ = ">="; }
    | LE                                    { $$ = "<="; }
    | GRATER                                { $$ = ">"; }
    | LOWER                                 { $$ = "<"; }
    ;

factor
    : LPAREN expr RPAREN                    { $$ = $2; }
    | ID                                    { 
                                              $$ = makeId($1);
                                              $$->row = yylineno;
                                              $$->col = token_column;
                                            }
    | NUM                                   { $$ = makeNum($1);}
    | DECIMAL                               { $$ = makeDecimal($1); }
    | BOOLEAN                               { $$ = makeBoolean($1); }
    ;

arg_list
    : expr                                  { 
        $$ = $1; 
        $$->row = yylineno;
        $$->col = token_column;
    }
    | expr arg_list                         { 
        $$ = $1;
        $$->next = $2;
        $$->row = yylineno;
        $$->col = token_column;
    }
    ;

%%


void yyerror(const char *s) {
    if (lexical_error_occurred) {
        return;
    }

    if (strstr(s, "expecting RPAREN") != NULL) {
        report_error(ERROR_SYNTAX, yylineno, token_column,
                     "Parenthèse fermante attendue ')'");
    } else if (strstr(s, "expecting ASSIGNE") != NULL) {
        report_error(ERROR_SYNTAX, yylineno, token_column,
                     "Opérateur d'affectation attendu '='");
    } else if (strstr(s, "expecting ID") != NULL) {
        report_error(ERROR_SYNTAX, yylineno, token_column,
                     "Identifiant attendu");
    } else {
        report_error(ERROR_SYNTAX, yylineno, token_column,
                     "%s", s);
    }
}


int main() {
    char input[4096];
    int r = 0;

    root_table = create_table(NULL);
    cur_table = root_table;

    while (!quit_flag) {
        printf(">>> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        lexical_error_occurred = 0;

        YY_BUFFER_STATE buffer = yy_scan_string(input);
        r = yyparse();
        yy_delete_buffer(buffer);

        if (r != 0) {
            fprintf(stderr,
                "Analyse interrompue (code %d).\n", r);
        }
    }

    free_table(root_table);

    return r;
}