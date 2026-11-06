#ifndef AST_H
#define AST_H

#include "sym_table.h"



typedef enum {
    NODE_NUM, NODE_ID, NODE_DECIMAL, NODE_BOOLEAN,
    NODE_SYSFUNC, NDOE_BASICOP, NODE_BINOP, 
    NODE_ASSIGN, NODE_LIST
} NodeKind;

typedef struct ASTNode {
    NodeKind kind;
    union {
        int   num;                                   /* Num(n)                      */
        char *id;                                    /* Id(x)                       */
        double decimal;                              /* Decimal(d)                  */
        int boolean;
        struct {
            char           *name;                    
            struct ASTNode *arg;
        } sysfunc;                                   /* SysFunc(name, arg)   */
        struct {
            char           *op;                      
            struct ASTNode *args;                    /* BasicOp(op, arg1, arg2, ...)*/
        } basicop;
        struct {                                     /* BinOp(op,g,d)               */
            char           *op;                      /*   <- char* (fix)            */
            struct ASTNode *g, *d;
        } binop;
        struct {                                     /* Assign(id,e)                */
            char           *id;
            struct ASTNode  *e;
            SymFormat   format;          
        } assign;
        struct {
            struct ASTNode *elements;               /* LIST(e1, e2, ..)*/
        } list;
    } data;
    struct ASTNode *next;                            /* linked list                 */
} ASTNode;

ASTNode *makeNum(int n);
ASTNode *makeId(const char *x);
ASTNode *makeDecimal(double d);
ASTNode *makeBoolean(int b);
ASTNode *makeSysFunc(const char *name, ASTNode *arg);
ASTNode *makeBasicOp(const char *op, ASTNode *args);
ASTNode *makeBinOp(const char *op, ASTNode *g, ASTNode *d);
ASTNode *makeAssign(const char *id, ASTNode *e);
ASTNode *makeList(ASTNode *elements);

void afficheAST(ASTNode *root);
void freeAST(ASTNode *root);

#endif /* AST_H */