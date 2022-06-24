#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdbool.h>    /* bool */

#include "../frontend/syntactic-analysis/node.h"        /* node_* types */

typedef enum SYMBOLS_STATUS {
    MULTIPLE_DECLARATION = -2,
    OUT_OF_SCOPE,
    SUCCESS,
} SYMBOLS_STATUS;

// (1 + 2) -> abc

bool initialize_table();

int insert_variable(variable* var);
variable* lookup_variable(const char* name);

void free_table();

#endif /* SYMBOLS_H */