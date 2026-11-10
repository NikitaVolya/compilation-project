#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdarg.h>


extern int lexical_error_occurred;

/**
 * @brief Categories of compiler errors.
 */
typedef enum {
    ERROR_LEXICAL,
    ERROR_SYNTAX,
    ERROR_SEMANTIC
} ErrorCategory;

/**
 * @brief Reports a formatted compiler error.
 * @param category Error category.
 * @param line Source line number.
 * @param column Source column number.
 * @param fmt Formatted error message.
 */
void report_error(ErrorCategory category,
                  int line,
                  int column,
                  const char *fmt, ...);

/** @brief Returns the total number of reported errors. */
int get_error_count();

/** @brief Resets the error counter. */
void reset_errors();

#endif /* ERRORS_H */
