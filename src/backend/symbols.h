#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdbool.h>    /* bool */

#include "../frontend/syntactic-analysis/node.h"        /* node_* types */

typedef enum SYMBOLS_STATUS {
    MULTIPLE_DECLARATION = -2,
    OUT_OF_SCOPE,
    SUCCESS,
} SYMBOLS_STATUS;


bool init_table();

int insert_variable();
variable* lookup_variable(char* name);

void free_table();

#endif /* SYMBOLS_H */