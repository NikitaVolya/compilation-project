#include "sym_table.h"

static char *dupstr(const char *s) {
    char *r = (char *) malloc(strlen(s) + 1);
    if (!r) { 
        perror("malloc"); 
        exit(1); 
    }
    strcpy(r, s);
    
    return r;
}

SymEntry* create_sym_entry(const char *name, SymFormat format) {
    SymEntry *rep;

    if ((rep = (SymEntry*) malloc(sizeof(SymEntry))) == NULL) {
        perror("malloc"); 
        exit(1);
    }

    rep->name = dupstr(name);
    rep->format = format;
    rep->next = NULL;

    return rep;
}

void free_sym_entry(SymEntry* entry) {
    SymEntry *next;

    while (entry->next != NULL) {
        next = entry->next->next;
        free(entry->next->name);
        free(entry->next);

        entry->next = next;
    }

    free(entry->name);
    free(entry);
}


SymTable* create_table(SymTable *parent) {
    SymTable *rep;
    int i;

    if ((rep = (SymTable*) malloc(sizeof(SymTable))) == NULL) {
        perror("malloc"); 
        exit(1);
    }

    for (i = 0; i < TABLE_SIZE; i++)
        rep->buckets[i] = NULL;

    rep->parent = parent;
    rep->scope = 0;

    return rep;
}

void free_table(SymTable* t) {
    int i;

    for (i = 0; i < t->scope; i++) {
        free_sym_entry(t->buckets[i]);
    }

    free(t);
}

SymTable* enter_scope(SymTable* cur) {
    return create_table(cur);
}

SymTable* exit_scope(SymTable* cur) {
    SymTable *rep = cur->parent;

    free_table(cur);

    return rep;
}

TABLE_INSERT_RESULT insert_symbol(SymTable* t, const char* name, SymFormat format) {
    SymEntry *entry;
    TABLE_INSERT_RESULT rep;

    entry = local_lookup_symbol(t, name);

    if (entry == NULL) {

        if (t->scope >= TABLE_SIZE) {
            rep = ERROR_TABLE_OVERFLOW;
        } else {
            t->buckets[t->scope] = create_sym_entry(name, format);
            t->scope++;
            rep = NO_ERROR;
        }
        
    } else if (entry->format == CONST_SYM) {
        rep = ERROR_CONST_SYM_FORMAT;
    } else {
        rep = NO_ERROR;
    }

    return rep;
}

SymEntry* lookup_symbol(SymTable* t, const char* name) {
    SymEntry *rep;

    rep = local_lookup_symbol(t, name);

    if (rep == NULL && t->parent != NULL)
        rep = lookup_symbol(t->parent, name);

    return rep;
}

SymEntry* local_lookup_symbol(SymTable* t, const char* name) {
    int i;
    SymEntry *entry, *rep;

    rep = NULL;

    for (i = 0; i < t->scope && rep == NULL; i++) {
        entry = t->buckets[i];

        if (strcmp(entry->name, name) == 0) {
            rep = entry;
        }
    }

    return rep;
}

int remove_symbol(SymTable *t, const char* name) {
    SymEntry *sym;
    int i, rep;

    if (t->scope == 0) {
        rep = 1;
    } else {
        rep = 1;
        for (i = 0; i < t->scope && rep == 1; i++) {
            sym = t->buckets[i];

            if (strcmp(sym->name, name) == 0) {
                rep = 0;

                free_sym_entry(sym);

                if (t->scope > 1) {
                    t->buckets[i] = t->buckets[t->scope - 1];
                }
                
                t->buckets[t->scope - 1] = NULL;
                t->scope = t->scope - 1;
            }
        }
    } 

    return rep;
}


static void print_separator(size_t name_width, size_t type_width) {
    putchar('+');
    for (size_t i = 0; i < name_width + 2; i++)
        putchar('-');

    putchar('+');
    for (size_t i = 0; i < type_width + 2; i++)
        putchar('-');

    puts("+");
}

void print_sym_table(SymTable *t) {
    if (t == NULL)
        return;

    size_t name_width = strlen("Identifiant");
    size_t type_width = strlen("Type");

    // find size of 
    for (int i = 0; i < t->scope; i++) {
        SymEntry *entry = t->buckets[i];

        const char *name = entry->name;
        const char *type = "no";

        if (strlen(name) > name_width)
            name_width = strlen(name);

        if (strlen(type) > type_width)
            type_width = strlen(type);
    }

    /* Header */
    print_separator(name_width, type_width);

    printf("| %-*s | %-*s |\n",
           (int)name_width, "Identifiant",
           (int)type_width, "Type");

    print_separator(name_width, type_width);

    for (int i = 0; i < t->scope; i++) {
        for (SymEntry *entry = t->buckets[i];
             entry != NULL;
             entry = entry->next) {

            printf("| %-*s | %-*s |\n",
                   (int)name_width, entry->name,
                   (int)type_width,
                   "not");
        }
    }

    print_separator(name_width, type_width);
}
