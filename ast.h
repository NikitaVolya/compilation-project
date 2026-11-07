
#ifndef AST_H
#define AST_H

#include "sym_table.h"

/**
 * @brief Types of Abstract Syntax Tree (AST) nodes.
 */
typedef enum {
    NODE_NUM,
    NODE_ID,
    NODE_DECIMAL,
    NODE_BOOLEAN,
    NODE_SYSFUNC,
    NODE_BASICOP,
    NODE_BINOP,
    NODE_ASSIGN,
    NODE_LIST
} NodeKind;

/**
 * @brief Represents a node in the Abstract Syntax Tree.
 */
typedef struct ASTNode {
    NodeKind kind;                    /**< Type of the node. */

    union {
        int num;                      /**< Integer literal. */
        char *id;                     /**< Identifier name. */
        double decimal;               /**< Decimal literal. */
        int boolean;                  /**< Boolean literal. */

                                      /** @brief System function call. */
        struct {
            char *name;               /**< Function name. */
            struct ASTNode *arg;      /**< Function argument. */
        } sysfunc;

                                      /** @brief Operation with multiple arguments. */
        struct {
            char *op;                 /**< Operator name. */
            struct ASTNode *args;     /**< Linked list of arguments. */
        } basicop;

                                      /** @brief Binary operation. */
        struct {
            char *op;                 /**< Operator name. */
            struct ASTNode *g, *d;    /**< Left and right operands. */
        } binop;

                                      /** @brief Variable assignment. */
        struct {
            char *id;                 /**< Target identifier. */
            struct ASTNode *e;        /**< Assigned expression. */
            SymFormat format;         /**< Symbol format. */
        } assign;

                                      /** @brief List expression. */
        struct {
            struct ASTNode *elements; /**< Linked list of elements. */
        } list;

    } data;

    struct ASTNode *next;             /**< Next node in a linked list. */
} ASTNode;


/** @brief Creates an integer literal node. */
ASTNode *makeNum(int n);

/** @brief Creates an identifier node. */
ASTNode *makeId(const char *x);

/** @brief Creates a decimal literal node. */
ASTNode *makeDecimal(double d);

/** @brief Creates a boolean literal node. */
ASTNode *makeBoolean(int b);

/** @brief Creates a system function call node. */
ASTNode *makeSysFunc(const char *name, ASTNode *arg);

/** @brief Creates an operation node with multiple arguments. */
ASTNode *makeBasicOp(const char *op, ASTNode *args);

/** @brief Creates a binary operation node. */
ASTNode *makeBinOp(const char *op, ASTNode *g, ASTNode *d);

/** @brief Creates an assignment node. */
ASTNode *makeAssign(const char *id, ASTNode *e);

/** @brief Creates a list node. */
ASTNode *makeList(ASTNode *elements);

/** @brief Prints the Abstract Syntax Tree. */
void afficheAST(ASTNode *root);

/** @brief Recursively frees the Abstract Syntax Tree. */
void freeAST(ASTNode *root);

#endif /* AST_H */
