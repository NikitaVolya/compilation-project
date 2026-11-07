
#ifndef SYM_TAB_H
#define SYM_TAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief Maximum number of symbols per table. */
#define TABLE_SIZE 211

/**
 * @brief Symbol declaration modifiers.
 */
typedef enum {
    DEFAULT_SYM,   /**< Regular symbol. */
    CONST_SYM,     /**< Constant symbol. */
    TYPECONST_SYM  /**< Type constant symbol. */
} SymFormat;

/**
 * @brief Represents a symbol entry.
 */
typedef struct SymEntry {
    char* name;            /**< Symbol identifier. */
    SymFormat format;      /**< Declaration modifier. */
    struct SymEntry* next; /**< Next entry in the chain. */
} SymEntry;

/**
 * @brief Symbol table supporting nested scopes.
 */
typedef struct SymTable {
    SymEntry* buckets[TABLE_SIZE]; /**< Array of symbol entries. */
    int scope;                     /**< Number of stored symbols. */
    struct SymTable* parent;       /**< Enclosing scope. */
} SymTable;

/**
 * @brief Possible results of symbol insertion.
 */
typedef enum {
    NO_ERROR = 0,               /**< Operation successful. */
    ERROR_CONST_SYM_FORMAT,     /**< Existing symbol is constant. */
    ERROR_TYPECONST_SYM_FORMAT, /**< Existing symbol is a type constant. */
    ERROR_TABLE_OVERFLOW        /**< Table capacity reached. */
} TABLE_INSERT_RESULT;


/**
 * @brief Creates an empty symbol table.
 * @param parent Enclosing scope, or NULL for the global scope.
 * @return Newly allocated symbol table.
 */
SymTable* create_table(SymTable *parent);

/**
 * @brief Frees a symbol table.
 * @param t Table to free.
 */
void free_table(SymTable* t);

/**
 * @brief Creates a new nested scope.
 * @param cur Current scope.
 * @return New child symbol table.
 */
SymTable* enter_scope(SymTable* cur);

/**
 * @brief Destroys the current scope and returns its parent.
 * @param cur Current scope.
 * @return Parent symbol table.
 */
SymTable* exit_scope(SymTable* cur);

/**
 * @brief Inserts a symbol or checks an existing declaration.
 * @param t Target symbol table.
 * @param name Symbol identifier.
 * @param format Declaration modifier.
 * @return Insertion status or declaration error.
 */
TABLE_INSERT_RESULT insert_symbol(SymTable* t, const char* name, SymFormat format);

/**
 * @brief Searches for a symbol in the current and parent scopes.
 * @return Matching entry, or NULL if not found.
 */
SymEntry* lookup_symbol(SymTable* t, const char* name);

/**
 * @brief Searches for a symbol in the current scope only.
 * @param t Symbol table to search.
 * @param name Symbol identifier.
 * @return Matching entry, or NULL if not found.
 */
SymEntry* local_lookup_symbol(SymTable* t, const char* name);

/**
 * @brief Removes a symbol from the current scope.
 * @return 0 if removed, 1 if not found.
 */
int remove_symbol(SymTable *t, const char* name);

/**
 * @brief Prints the current table as a formatted ASCII table.
 */
void print_sym_table(SymTable* t);

#endif /* SYM_TAB_H */
