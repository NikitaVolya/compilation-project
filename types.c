#include "types.h"

/** @brief Built-in type instances. */
static Type _error        = { TY_ERROR };
static Type _void         = { TY_VOID };
static Type _boolean      = { TY_BOOLEAN };
static Type _int          = { TY_INT };
static Type _double       = { TY_DOUBLE };
static Type _list_int     = { TY_LIST_INT };
static Type _list_double  = { TY_LIST_DOUBLE };
static Type _list_boolean = { TY_LIST_BOOLEAN };

Type *type_error          = &_error;
Type *type_void           = &_void;
Type *type_boolean        = &_boolean;
Type *type_int            = &_int;
Type *type_double         = &_double;
Type *type_list_int       = &_list_int;
Type *type_list_double    = &_list_double;
Type *type_list_boolean   = &_list_boolean;


const char *type_to_string(Type *t) {

    if (!t) 
        return "?";

    switch (t->kind) {
        case TY_ERROR:        return "error";
        case TY_VOID:         return "void";
        case TY_BOOLEAN:      return "bool";
        case TY_INT:          return "int";
        case TY_DOUBLE:       return "double";
        case TY_LIST_INT:     return "list[int]";
        case TY_LIST_DOUBLE:  return "list[double]";
        case TY_LIST_BOOLEAN: return "list[boolean]";
        default:              return "?";
    }
}

int type_equal(Type *a, Type *b) {
    int rep;

    if (a == b) { 
        rep = 1;
    } else if (!a || !b) { 
        rep = 0;
    } else if (a->kind != b->kind) { 
        rep = 0;
    } else {
        rep = 1;
    }

    return rep;
}



int type_compatible(Type *dest, Type *src) {
    int src_num, dest_num, src_list_num, dest_list_num;

    /* Reject undefined types. */
    if (!dest || !src)
        return 0;

    /* Prevent cascading errors during type checking. */
    if (dest->kind == TY_ERROR || src->kind == TY_ERROR)
        return 1;

    /* Accept identical types without conversion. */
    if (type_equal(dest, src))
        return 1;

    /* Allow conversions between int and double. */
    src_num  = (src->kind == TY_INT || src->kind == TY_DOUBLE);
    dest_num = (dest->kind == TY_INT || dest->kind == TY_DOUBLE);

    if (dest_num && src_num)
        return 1;

    /* Allow conversions between list<int> and list<double>. */
    src_list_num = (src->kind == TY_LIST_INT ||
                    src->kind == TY_LIST_DOUBLE);

    dest_list_num = (dest->kind == TY_LIST_INT ||
                     dest->kind == TY_LIST_DOUBLE);

    if (src_list_num && dest_list_num)
        return 1;

    /* Allow conversions between boolean and integer values. */
    if (dest->kind == TY_BOOLEAN && src->kind == TY_INT)
        return 1;

    if (dest->kind == TY_INT && src->kind == TY_BOOLEAN)
        return 1;

    /* Reject all other conversions. */
    return 0;
}


Type* type_merger(Type *a, Type *b) {
    Type *rep;

    /* Reject undefined or erroneous types. */
    if (!a || !b) {
        rep = type_error;
    } else if (type_equal(a, type_error) || type_equal(b, type_error)) {
        rep = type_error;

    /* Preserve identical types. */
    } else if (type_equal(a, b)) {
        rep = a;

    /* Promote mixed numeric types to double. */
    } else if ((type_equal(a, type_int) && type_equal(b, type_double)) ||
               (type_equal(a, type_double) && type_equal(b, type_int))) {
        rep = type_double;

    /* Promote mixed numeric lists to list<double>. */
    } else if ((type_equal(a, type_list_int)    && type_equal(b, type_list_double)) ||
               (type_equal(a, type_list_double) && type_equal(b, type_list_int))) {
        rep = type_list_double;

    /* Reject incompatible type combinations. */
    } else {
        rep = type_error;
    }

    return rep;
}

