#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// dupliquer une chaine de caracteres
static char *dupstr(const char *s) {
    char *r = (char *)malloc(strlen(s) + 1);
    if (!r) { perror("malloc"); exit(1); }
    strcpy(r, s);
    return r;
}

// Une fonction helper qui permet de créer n'importe quel type de noeud
static ASTNode *newNode(NodeType t) {
    ASTNode *n = (ASTNode *)calloc(1, sizeof(ASTNode));
    if (!n) { perror("calloc"); exit(1); }
    n->type = t;
    n->next = NULL;
    return n;
}
// Creer un noeud AST de type Num
ASTNode *makeNum(int n) { 
    ASTNode *r = newNode(NODE_NUM);    
    r->data.num = n;             
    return r; 
}

// Noeud AST ID
ASTNode *makeId(const char *x) { 
    ASTNode *r = newNode(NODE_ID); 
    r->data.id = dupstr(x);
    return r;
 }

// Noeud AST Decimal 
ASTNode *makeDecimal(double d) {
    ASTNode *r = newNode(NODE_DECIMAL); 
    r->data.decimal = d;
    return r;
}

ASTNode *makeBoolean(int b) {
    ASTNode *r = newNode(NODE_BOOLEAN);
    r->data.boolean = b;
    return r;
}

// Noeud AST SysFunc
ASTNode *makeSysFunc(const char *name, ASTNode *arg) {
    ASTNode *r = newNode(NODE_SYSFUNC); 
    r->data.sysfunc.name = dupstr(name);
    r->data.sysfunc.arg = arg;
    return r;
}

// Noeud AST BasicOp
ASTNode *makeBasicOp(const char *op, ASTNode *args) {
    ASTNode *r = newNode(NDOE_BASICOP); 
    r->data.basicop.op = dupstr(op);
    r->data.basicop.args = args;
    return r;
}

// Noeud AST Binop
ASTNode *makeBinOp(const char *op, ASTNode *g, ASTNode *d) {
    ASTNode *r = newNode(NODE_BINOP);
    r->data.binop.op = dupstr(op);
    r->data.binop.g  = g;
    r->data.binop.d  = d;
    return r;
}

// Noeud AST Assign
ASTNode *makeAssign(const char *id, ASTNode *e, IdFormat format) {
    ASTNode *r = newNode(NODE_ASSIGN);
    r->data.assign.id = dupstr(id);
    r->data.assign.e = e;
    r->data.assign.format  = format;
    return r;
}


/* ---------------- Affichage ------------- */
static void indent(int n) { while (n-- > 0) printf("  "); }

static void printNode(ASTNode *n, int d) {
    while (n) {
        indent(d);
        switch (n->type) {
        case NODE_NUM:     printf("Num(%d)\n", n->data.num);         break;
        case NODE_ID:      printf("Id(%s)\n", n->data.id);           break;
        case NODE_DECIMAL: printf("Decimal(%f)\n", n->data.decimal); break;
        case NODE_BOOLEAN: printf("Boolean(%s)\n", (n->data.boolean ? "TRUE" : "FALSE") ); break;
        case NODE_SYSFUNC:
            printf("SysFunc(%s)\n", n->data.sysfunc.name);
            printNode(n->data.sysfunc.arg, d + 1);
            break;
        case NDOE_BASICOP:
            printf("BasicOp(%s)\n", n->data.basicop.op);
            printNode(n->data.basicop.args, d + 1);
            break;
        case NODE_BINOP:
            printf("BinOp(%s)\n", n->data.binop.op);
            printNode(n->data.binop.g, d + 1);
            printNode(n->data.binop.d, d + 1);
            break;
        case NODE_ASSIGN:
            printf("Assign(%s)\n", n->data.assign.id);
            printNode(n->data.assign.e, d + 1);
            break;
        }
        n = n->next;
    }
}

void afficheAST(ASTNode *root) { printNode(root, 0); }

/* ---------------- Libération ------------- */
void freeAST(ASTNode *n) {
    while (n) {
        ASTNode *nx = n->next;
        switch (n->type) {
        case NODE_ID:     free(n->data.id); break;
        case NODE_SYSFUNC:
            free(n->data.sysfunc.name);
            freeAST(n->data.sysfunc.arg);
            break;
        case NDOE_BASICOP:
            free(n->data.basicop.op);
            freeAST(n->data.basicop.args);
            break;
        case NODE_BINOP:
            free(n->data.binop.op);
            freeAST(n->data.binop.g);
            freeAST(n->data.binop.d); break;
        case NODE_ASSIGN:
            free(n->data.assign.id);
            freeAST(n->data.assign.e); break;
        default: break;
        }
        free(n);
        n = nx;
    }
}