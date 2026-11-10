
#include "errors.h"

static int error_count = 0;

int lexical_error_occurred = 0;

static const char *error_category_name(ErrorCategory category) {
    switch (category) {
        case ERROR_LEXICAL:  return "Lexicale";
        case ERROR_SYNTAX:   return "Syntaxique";
        case ERROR_SEMANTIC: return "Sémantique";
        default:             return "Inconnue";
    }
}

void report_error(ErrorCategory category,
                  int line,
                  int column,
                  const char *fmt, ...) {
    va_list args;
    int i;

    for (i = 0; i < column + 3; i++)
        putc(' ', stderr);
    putc('^', stderr);

    fprintf(stderr, "\n[%s] Ligne %d, colonne %d : ",
            error_category_name(category), line, column);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
    error_count++;
}

int get_error_count(void) {
    return error_count;
}

void reset_errors(void) {
    error_count = 0;
}
