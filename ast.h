#ifndef AST_H
#define AST_H

typedef enum {
    DEFAULT_ID, CONST_ID, TYPECONST_ID
} IdFormat;

typedef enum {
    NODE_NUM, NODE_ID, NODE_DECIMAL, NODE_BOOLEAN,
    NODE_SYSFUNC, NDOE_BASICOP, NODE_BINOP, 
    NODE_ASSIGN
} NodeType;

typedef struct ASTNode {
    NodeType type;
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
            IdFormat    format;          
        } assign;
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
ASTNode *makeAssign(const char *id, ASTNode *e, IdFormat format);

void afficheAST(ASTNode *root);
void freeAST(ASTNode *root);

#endif