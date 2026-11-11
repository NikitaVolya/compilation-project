#include"typecheck.h"


/** @brief Checks whether an operator is logical (and, or). */
int is_logic_operator(const char *op) {
    return !strcmp(op, "and") || !strcmp(op, "or");
}

/** @brief Checks whether a type is numeric (int or double). */
int is_num(Type *type) {
    return type_equal(type, type_int) || type_equal(type, type_double);
}

/** @brief Checks whether a type is a typed list. */
int is_list(Type *type) {
    return type_equal(type, type_list_int) || 
           type_equal(type, type_list_double) ||
           type_equal(type, type_list_boolean);
}

/** @brief Checks whether an operator tests equality (==, !=). */
int is_equal_operation(const char *op) {
    return !strcmp(op, "==") || !strcmp(op, "!=");
}

/** @brief Checks whether an operator is relational (<, <=, >, >=). */
int is_relation_operation(const char *op) {
    return !strcmp(op, "<=") || !strcmp(op, "<")  ||
           !strcmp(op, ">=") || !strcmp(op, ">");
}

/**
 * @brief Checks argument types and infers a system function's result.
 *
 * Supported functions:
 * - exp, log, cos, sin, sqrt: int/double -> double.
 * - len: list<T> -> int.
 * - sum: list<T> -> element type.
 * - not: list<boolean> -> list<boolean>.
 *        boolean       -> boolean
 *
 * @param a System function AST node.
 * @param cur Current symbol table.
 * @return Function result type, or type_error.
 */
Type* typecheck_sysfunc(ASTNode *a, SymTable *cur) {
    const char *name = a->data.sysfunc.name;
    ASTNode *arg = a->data.sysfunc.arg;
    Type *rep;

    
    typecheck(arg, cur);
    if (!strcmp(name, "exp") || !strcmp(name, "log") || !strcmp(name, "cos") ||
        !strcmp(name, "sin") || !strcmp(name, "sqrt")) {
        
        if (type_equal(arg->type, type_error)) {
            rep = type_error;
        } else if (type_equal(arg->type, type_int) || type_equal(arg->type, type_double)) {
            rep = type_double;
        } else {
            rep = type_error;

            report_error(ERROR_SEMANTIC, a->row, a->col,
                        "La fonction '%s' attend une double ou un int, mais a reçu %s",
                        name,
                        type_to_string(arg->type));
        }

    } else if (!strcmp(name, "len")) {
        
        if (type_equal(arg->type, type_error)) {
            rep = type_error;
        } else if (is_list(arg->type)) {
            rep = type_int;
        } else {
            rep = type_error;

            report_error(ERROR_SEMANTIC, a->row, a->col,
                        "La fonction 'len' attend une liste, mais a reçu %s",
                        type_to_string(arg->type));
        }

    } else if (!strcmp(name, "sum")) {
        
        if (type_equal(arg->type, type_error)) {
            rep = type_error;
        } else if (is_list(arg->type)) {
            rep = arg->data.list.elements->type;
        } else {
            rep = type_error;

            report_error(ERROR_SEMANTIC, a->row, a->col,
                        "La fonction 'sum' attend une liste, mais a reçu %s",
                        type_to_string(arg->type));
        }

    } else if (!strcmp(name, "not")) {

        if (type_equal(arg->type, type_error)) {
            rep = type_error;
        } else if (type_equal(arg->type, type_boolean)) {
            rep = type_boolean;
        } else if (type_equal(arg->type, type_list_boolean)) {
            rep = type_list_boolean;
        } else {
            rep = type_error;

            report_error(ERROR_SEMANTIC, a->row, a->col,
                        "La fonction 'not' attend une list des booleans ou un boolean, mais a reçu %s",
                        type_to_string(arg->type));
        }

    } else {
        rep = type_error;
    }

    return rep;
}


/**
 * @brief Infers a common type for a linked list of expressions.
 *
 * Each argument is type-checked and merged using type_merger().
 * An empty argument list produces type_error.
 *
 * @param args First argument in the linked list.
 * @param cur Current symbol table.
 * @return Common argument type, or type_error.
 */
Type* typecheck_basicop_args(ASTNode *args, SymTable *cur) {
    Type *rep, *merged;

    if (args == NULL) {
        rep = type_error;
    } else {
        typecheck(args, cur);
        rep = args->type;
        args = args->next;

        while (args != NULL) {
            typecheck(args, cur);

            merged = type_merger(rep, args->type);

            if (type_equal(merged, type_error)) {

                if (!type_equal(rep, type_error) &&
                    !type_equal(args->type, type_error)) {

                    report_error(
                        ERROR_SEMANTIC,
                        args->row,
                        args->col,
                        "Types incompatibles : %s et %s",
                        type_to_string(rep),
                        type_to_string(args->type)
                    );
                }
            }

            rep = merged;
            args = args->next;
        }
    }

    return rep;
}

/**
 * @brief Checks variadic operations and infers their result type.
 *
 * Operator rules:
 * - + : Returns the merged type; boolean becomes int.
 * - - : Accepts numeric arguments only.
 * - * : Accepts numeric arguments only.
 * - and, or : Accepts booleans arguments only return boolean.
 *
 * @param a Variadic operation AST node.
 * @param cur Current symbol table.
 * @return Result type, or type_error.
 */
Type* typecheck_basicop(ASTNode *a, SymTable *cur) {
    Type *args_type, *rep;
    const char *op;

    args_type = typecheck_basicop_args(a->data.basicop.args, cur);
    op = a->data.basicop.op;

    /* An error was already detected in the arguments. */
    if (type_equal(args_type, type_error)) {
        rep = type_error;
    } else if (!strcmp(op, "+")) {
        
        if (type_equal(args_type, type_boolean)) {
            rep = type_int;
        } else if (is_num(args_type) || is_list(args_type)) {
            rep = args_type;
        } else {
            rep = type_error;

            report_error(
                ERROR_SEMANTIC,
                a->row,
                a->col,
                "L'opérateur '+' attend des nombres, des booléens ou des listes, mais a reçu %s",
                type_to_string(args_type)
            );
        }

    } else if (is_logic_operator(op)) {
        
        if (type_equal(args_type, type_boolean)) {
            rep = type_boolean;
        } else {
            rep = type_error;

            report_error(
                ERROR_SEMANTIC,
                a->row,
                a->col,
                "L'opérateur '%s' attend des booléens, mais a reçu %s",
                op,
                type_to_string(args_type)
            );
        }


    } else if (!strcmp(op, "-") || !strcmp(op, "*")) {

        if (is_num(args_type)) {
            rep = args_type;
        } else {
            rep = type_error;

            report_error(
                ERROR_SEMANTIC,
                a->row,
                a->col,
                "L'opérateur '%s' attend des nombres, mais a reçu %s",
                op,
                type_to_string(args_type)
            );
        }

    } else {
        rep = type_error;
    }

    return rep;
}

/**
 * @brief Checks binary operator operand types.
 *
 * Operator rules:
 * - /, ^         : Require numeric operands; return their merged type.
 * - ==, !=       : Accept identical types or mixed numeric types.
 * - <, <=, >, >= : Accept identical non-list types
 *                  or mixed numeric types.
 *
 * Comparison operators return boolean.
 * Invalid combinations produce type_error.
 *
 * @param a Binary operation AST node.
 * @param cur Current symbol table.
 * @return Result type, or type_error.
 */
Type* typecheck_binop(ASTNode *a, SymTable *cur) {
    Type *rep;
    ASTNode *l, *r;
    const char *op;

    op = a->data.binop.op;
    l = a->data.binop.g;
    r = a->data.binop.d;

    typecheck(l, cur);
    typecheck(r, cur);

    if (!strcmp(op, "/") || !strcmp(op, "^")) {

        rep = type_merger(l->type, r->type);

        if (type_equal(rep, type_error)) {
            rep = type_error;
        } else if (!is_num(rep)) {
            rep = type_error;

            report_error(
                ERROR_SEMANTIC,
                a->row,
                a->col,
                "L'opérateur '%s' attend des nombres, mais a reçu %s",
                op,
                type_to_string(rep)
            );
        }

    } else if (is_equal_operation(op)) {

        if (type_equal(l->type, type_error) || type_equal(r->type, type_error)) {
            rep = type_error;
        } else if (type_equal(l->type, r->type)) {
            rep = type_boolean;
        } else if (is_num(l->type) && is_num(r->type)) {
            rep = type_boolean;
        } else {
            rep = type_error;

            report_error(
                ERROR_SEMANTIC,
                a->row,
                a->col,
                "L'opérateur '%s' attend deux même types, mais a reçu %s",
                op,
                type_to_string(rep)
            );
        }

    } else if (is_relation_operation(op)) {

        if (type_equal(l->type, type_error) || type_equal(r->type, type_error)) {
            rep = type_error;
        } else if (is_list(l->type) || is_list(r->type)) {
            rep = type_error;

             report_error(
                ERROR_SEMANTIC,
                a->row,
                a->col,
                "L'opérateur relationnel '%s' ne prend pas en charge "
                "les listes (types reçus : %s et %s)",
                op,
                type_to_string(l->type),
                type_to_string(r->type)
            );

        } else if (type_equal(l->type, r->type)) {
            rep = type_boolean;
        } else if (is_num(l->type) && is_num(r->type)) {
            rep = type_boolean;
        } else {
            rep = type_error;

            report_error(
                ERROR_SEMANTIC,
                a->row,
                a->col,
                "Types incompatibles pour l'opérateur '%s' : %s et %s",
                op,
                type_to_string(l->type),
                type_to_string(r->type)
            );
        }

    } else {
        rep = type_error;
    }

    return rep;
}

/**
 * @brief Checks an assignment and updates the symbol table.
 *
 * - Checks the assigned expression.
 * - Rejects error and void expression types.
 * - Inserts or updates the symbol using its declaration format.
 * - Returns void on success, error otherwise.
 *
 * @param a Assignment AST node.
 * @param cur Current symbol table.
 * @return type_void on success, or type_error.
 */
Type* typecheck_assign(ASTNode *a, SymTable *cur) {
    ASTNode *e = a->data.assign.e;
    const char *id = a->data.assign.id;
    SymFormat format = a->data.assign.format;
    TABLE_INSERT_RESULT res;
    SymEntry *entry;
    Type *rep;

    typecheck(e, cur);

    if (type_equal(e->type, type_error) || type_equal(e->type, type_void)) {
        rep = type_error;
    } else {
        rep = type_void;

        res = insert_symbol(cur, id, format, e->type);

        switch (res) {
            case NO_ERROR:
                return a->type = type_void;

            case ERROR_NO_TYPE:
                rep = type_error;

                report_error(
                    ERROR_SEMANTIC,
                    a->row,
                    a->col,
                    "Impossible de déterminer le type de la variable '%s'",
                    id
                );
                break;

            case ERROR_TABLE_OVERFLOW:
                rep = type_error;

                report_error(
                    ERROR_SEMANTIC,
                    a->row,
                    a->col,
                    "La table des symboles est pleine : "
                    "impossible de déclarer '%s'",
                    id
                );
                break;

            case ERROR_CONST_SYM_FORMAT:
                rep = type_error;

                report_error(
                    ERROR_SEMANTIC,
                    a->row,
                    a->col,
                    "Impossible de modifier la constante '%s'",
                    id
                );
                break;

            case ERROR_TYPECONST_SYM_FORMAT:
                entry = local_lookup_symbol(cur, id);
                rep = type_error;

                report_error(
                    ERROR_SEMANTIC,
                    a->row,
                    a->col,
                    "Impossible de modifier le type de '%s' : "
                    "type attendu %s, type reçu %s",
                    id,
                    entry ? type_to_string(entry->type) : "inconnu", 
                    type_to_string(e->type)
                );
                break;

            default:
                rep = type_error;

                report_error(
                    ERROR_SEMANTIC,
                    a->row,
                    a->col,
                    "Erreur inconnue lors de l'affectation de '%s'",
                    id
                );
                break;
        }
    }

    a->type = rep;
    return rep;
}

/**
 * @brief Infers the element type of a list expression.
 *
 * List type rules:
 * - int elements                     -> list<int>.
 * - double or mixed numeric elements -> list<double>.
 * - boolean elements                 -> list<boolean>.
 * - Incompatible or empty elements   -> type_error.
 *
 * @param a List AST node.
 * @param cur Current symbol table.
 * @return Inferred list type, or type_error.
 */
Type* typecheck_list(ASTNode *a, SymTable *cur) {
    Type *elements_type, *rep;

    elements_type = typecheck_basicop_args(a->data.list.elements, cur);

    if (type_equal(elements_type, type_error)) {
        rep = type_error;
    } else if (type_equal(elements_type, type_int)) {
        rep = type_list_int;
    } else if (type_equal(elements_type, type_double)) {
        rep = type_list_double;
    } else if (type_equal(elements_type, type_boolean)) {
        rep = type_list_boolean;
    } else {
        rep = type_error;

        report_error(
            ERROR_SEMANTIC,
            a->row,
            a->col,
            "Type d'élément non autorisé dans une liste : %s",
            type_to_string(elements_type)
        );
    }

    return rep;
}

Type* typecheck(ASTNode *a, SymTable *cur) {
    Type *rep;
    SymEntry *sym;

    if (a == NULL) {
        rep = type_void;
    } else {

        switch (a->kind) {

            case NODE_LIST:    a->type = typecheck_list(a, cur);    break;

            case NODE_ID:
                sym = lookup_symbol(cur, a->data.id);

                if (sym == NULL) {
                    a->type = type_error;
                    report_error(ERROR_SEMANTIC, a->row, a->col,
                        "Identifiant '%s' non déclaré",
                        a->data.id);
                } else {
                    a->type = sym->type;
                }
                break;

            case NODE_ASSIGN:  a->type = typecheck_assign(a, cur);  break;
            
            case NODE_BINOP:   a->type = typecheck_binop(a, cur);   break;
            case NODE_BASICOP: a->type = typecheck_basicop(a, cur); break;
            case NODE_SYSFUNC: a->type = typecheck_sysfunc(a, cur); break;

            case NODE_NUM:     a->type = type_int;                  break;
            case NODE_DECIMAL: a->type = type_double;               break;
            case NODE_BOOLEAN: a->type = type_boolean;              break;
            default:           a->type = type_void;                 break;
        }

        rep = a->type;
    }

    return rep;
}