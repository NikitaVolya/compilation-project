
#ifndef TYPECHECK_H
#define TYPECHECK_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "errors.h"

/**
 * @brief Performs semantic type checking on an AST node.
 * @param a AST node to analyze.
 * @param cur Current symbol table.
 * @return Inferred type, or type_error if invalid.
 * @note Stores the inferred type in the node.
 */
Type* typecheck(ASTNode *a, SymTable *cur);

/*
+----------------+---------------------------+--------------------+
| Operator       | Accepted operand types    | Result type        |
+----------------+---------------------------+--------------------+
| +              | Same / mixed numeric      | Merged type        |
| +              | boolean                   | int                |
| -              | int, double               | Merged numeric     |
| *              | int, double               | Merged numeric     |
| /              | int, double               | Merged numeric     |
| ^              | int, double               | Merged numeric     |
| and, or        | Same type                 | Same type          |
| ==, !=         | Same type / mixed numeric | boolean            |
| <, <=, >, >=   | Same non-list / numeric   | boolean            |
+----------------+---------------------------+--------------------+
| exp, log, ...  | int, double               | double             |
| len            | Any typed list            | int                |
| sum            | Any typed list            | Element type*      |
| not            | list<boolean>             | list<boolean>      |
| not            | boolean                   | boolean            |
+----------------+---------------------------+--------------------+
*/

#endif /* TYPECHECK_H */
