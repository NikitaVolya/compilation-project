#ifndef SYM_TAB_H
#define SYM_TAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TABLE_SIZE 211


typedef enum {
    DEFAULT_SYM, CONST_SYM, TYPECONST_SYM
} SymFormat;

typedef struct SymEntry {
    char* name;
    SymFormat format;
    struct SymEntry* next;
} SymEntry;

typedef struct SymTable {
    SymEntry* buckets[TABLE_SIZE];
    int scope;
    struct SymTable* parent;
} SymTable;

typedef enum {
    NO_ERROR = 0,
    ERROR_ID_ALREADY_EXIST,
    ERROR_TABLE_OVERFLOW
} TABLE_INSERT_RESULT;


SymTable* create_table(SymTable *parent);
void free_table(SymTable* t);

SymTable* enter_scope(SymTable* cur);
SymTable* exit_scope(SymTable* cur);

TABLE_INSERT_RESULT insert_symbol(SymTable* t, const char* name, SymFormat format);
SymEntry* lookup_symbol(SymTable* t, const char* name);
int remove_symbol(SymTable *t, const char* name);

void print_sym_table(SymTable* t);


#endif /* SYM_TAB_H */