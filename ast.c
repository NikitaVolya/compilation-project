
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

/**
 * @brief Duplicates a string into newly allocated memory.
 * @return Pointer to the duplicated string.
 */
static char *dupstr(const char *s) {
    char *r = (char *)malloc(strlen(s) + 1);

    if (!r) { 
        perror("malloc"); exit(1); 
    }
    
    strcpy(r, s);
    
    return r;
}

/**
 * @brief Allocates and initializes an AST node.
 * @param t Node type.
 * @return Pointer to the new node.
 */
static ASTNode *newNode(NodeKind t) {
    ASTNode *n = (ASTNode *) calloc(1, sizeof(ASTNode));
    
    if (!n) { 
        perror("calloc"); 
        exit(1); 
    }

    n->kind = t;
    n->next = NULL;
    
    return n;
}

/** @brief Creates an integer literal node. */
ASTNode *makeNum(int n) {
    ASTNode *r = newNode(NODE_NUM);
    r->data.num = n;
    return r;
}

/** @brief Creates an identifier node. */
ASTNode *makeId(const char *x) {
    ASTNode *r = newNode(NODE_ID);
    r->data.id = dupstr(x);
    return r;
}

/** @brief Creates a decimal literal node. */
ASTNode *makeDecimal(double d) {
    ASTNode *r = newNode(NODE_DECIMAL);
    r->data.decimal = d;
    return r;
}

/** @brief Creates a boolean literal node. */
ASTNode *makeBoolean(int b) {
    ASTNode *r = newNode(NODE_BOOLEAN);
    r->data.boolean = b;
    return r;
}

/** @brief Creates a system function call node. */
ASTNode *makeSysFunc(const char *name, ASTNode *arg) {
    ASTNode *r = newNode(NODE_SYSFUNC);
    r->data.sysfunc.name = dupstr(name);
    r->data.sysfunc.arg = arg;
    return r;
}

/** @brief Creates an operation node with multiple arguments. */
ASTNode *makeBasicOp(const char *op, ASTNode *args) {
    ASTNode *r = newNode(NODE_BASICOP);
    r->data.basicop.op = dupstr(op);
    r->data.basicop.args = args;
    return r;
}

/** @brief Creates a binary operation node. */
ASTNode *makeBinOp(const char *op, ASTNode *g, ASTNode *d) {
    ASTNode *r = newNode(NODE_BINOP);
    r->data.binop.op = dupstr(op);
    r->data.binop.g = g;
    r->data.binop.d = d;
    return r;
}

/** @brief Creates an assignment node. */
ASTNode *makeAssign(const char *id, ASTNode *e) {
    ASTNode *r = newNode(NODE_ASSIGN);
    r->data.assign.id = dupstr(id);
    r->data.assign.e = e;
    return r;
}

/** @brief Creates a list node. */
ASTNode *makeList(ASTNode *elements) {
    ASTNode *r = newNode(NODE_LIST);
    r->data.list.elements = elements;
    return r;
}

/**
 * @brief Prints indentation based on tree depth.
 * @param n Indentation level.
 */
static void indent(int n) {
    while (n-- > 0) printf("  ");
}

/**
 * @brief Recursively prints AST nodes and their children.
 * @param n Current node.
 * @param d Current tree depth.
 */
static void printNode(ASTNode *n, int d) {
    while (n) {
        indent(d);

        switch (n->kind) {
        case NODE_NUM:
            printf("Num(%d)\n", n->data.num);
            break;

        case NODE_ID:
            printf("Id(%s)\n", n->data.id);
            break;

        case NODE_DECIMAL:
            printf("Decimal(%f)\n", n->data.decimal);
            break;

        case NODE_BOOLEAN:
            printf("Boolean(%s)\n",
                   n->data.boolean ? "TRUE" : "FALSE");
            break;

        case NODE_SYSFUNC:
            printf("SysFunc(%s)\n", n->data.sysfunc.name);
            printNode(n->data.sysfunc.arg, d + 1);
            break;

        case NODE_BASICOP:
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

        case NODE_LIST:
            printf("LIST\n");
            printNode(n->data.list.elements, d + 1);
            break;
        }

        n = n->next;
    }
}

/** @brief Prints the complete AST. */
void afficheAST(ASTNode *root) {
    printNode(root, 0);
}

/**
 * @brief Recursively frees AST nodes and owned memory.
 * @param n Root of the node list to free.
 */
void freeAST(ASTNode *n) {
    while (n) {
        ASTNode *nx = n->next;

        switch (n->kind) {
        case NODE_ID:
            free(n->data.id);
            break;

        case NODE_SYSFUNC:
            free(n->data.sysfunc.name);
            freeAST(n->data.sysfunc.arg);
            break;

        case NODE_BASICOP:
            free(n->data.basicop.op);
            freeAST(n->data.basicop.args);
            break;

        case NODE_BINOP:
            free(n->data.binop.op);
            freeAST(n->data.binop.g);
            freeAST(n->data.binop.d);
            break;

        case NODE_ASSIGN:
            free(n->data.assign.id);
            freeAST(n->data.assign.e);
            break;

        case NODE_LIST:
            freeAST(n->data.list.elements);
            break;

        default:
            break;
        }

        free(n);
        n = nx;
    }
}
