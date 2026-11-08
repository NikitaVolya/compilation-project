
#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Supported types in the language.
 */
typedef enum {
    TY_ERROR,        /**< Invalid or unresolved type. */
    TY_VOID,         /**< Absence of a value. */
    TY_BOOLEAN,      /**< Boolean type. */
    TY_INT,          /**< Integer type. */
    TY_DOUBLE,       /**< Floating-point type. */
    TY_LIST_INT,     /**< List of integers. */
    TY_LIST_DOUBLE,  /**< List of floating-point values. */
    TY_LIST_BOOLEAN  /**< List of booleans. */
} TypeKind;


/**
 * @brief Represents a language type.
 */
typedef struct Type {
    TypeKind kind; /**< Type identifier. */
} Type;


/** @brief Shared instances of built-in language types. */
extern Type *type_error;
extern Type *type_void;
extern Type *type_boolean;
extern Type *type_int;
extern Type *type_double;
extern Type *type_list_int;
extern Type *type_list_double;
extern Type *type_list_boolean;


/**
 * @brief Returns the textual representation of a type.
 * @param t Type to convert.
 * @return Type name, or "?" if unknown.
 */
const char* type_to_string(Type *t);

/**
 * @brief Checks whether two types are identical.
 * @param a First type.
 * @param b Second type.
 * @return 1 if equal, 0 otherwise.
 */
int type_equal(Type *a, Type *b);


/**
 * @brief Checks whether a source type can be implicitly converted to a destination type.
 * @param dest Destination (expected) type.
 * @param src Source (provided) type.
 * @return 1 if compatible, 0 otherwise.
 */
int type_compatible(Type *dest, Type *src);

/*
+----------------+-------+-------+--------+------------+--------------+---------------+
| DEST \ SOURCE  | bool  | int   | double | list<bool> | list<int>    | list<double>  |
+----------------+-------+-------+--------+------------+--------------+---------------+
| bool           |  YES  |  YES  |   NO   |     NO     |      NO      |      NO       |
| int            |  YES  |  YES  |  YES   |     NO     |      NO      |      NO       |
| double         |   NO  |  YES  |  YES   |     NO     |      NO      |      NO       |
| list<bool>     |   NO  |   NO  |   NO   |    YES     |      NO      |      NO       |
| list<int>      |   NO  |   NO  |   NO   |     NO     |     YES      |     YES       |
| list<double>   |   NO  |   NO  |   NO   |     NO     |     YES      |     YES       |
+----------------+-------+-------+--------+------------+--------------+---------------+

Special rules:
  TY_ERROR  -> Compatible with all types
  TY_VOID   -> Compatible only with TY_VOID and TY_ERROR
  NULL      -> Always incompatible
*/

/**
 * @brief Determines a common type for two operands.
 * @return Merged type, or type_error if incompatible.
 */
Type* type_merger(Type *a, Type *b);

/*
+--------------+--------------+--------------+-------+---------------+
| Type A       | Type B       | Result       | Valid | Rule          |
+--------------+--------------+--------------+-------+---------------+
| int          | int          | int          | YES   | Same type     |
| int          | double       | double       | YES   | Promotion     |
| int          | bool         | error        | NO    | Incompatible  |
| double       | double       | double       | YES   | Same type     |
| doouble      | bool         | error        | NO    | Incompatible  |
| bool         | bool         | bool         | YES   | Same type     |
| list<int>    | list<int>    | list<int>    | YES   | Same type     |
| list<int>    | list<double> | list<double> | YES   | Promotion     |
| list<bool>   | list<bool>   | list<bool>   | YES   | Same type     |
| list<bool>   | list<int>    | error        | NO    | Incompatible  |
+--------------+--------------+--------------+-------+---------------+
*/


#endif /* TYPES_H */
