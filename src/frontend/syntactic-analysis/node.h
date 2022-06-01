#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

typedef union variable_value {
        bool boolean;
        long long integer;
        double floating;
        char * string;
} variable_value;

typedef struct variable {
        token_t type;
        char * name;
        variable_value value;
} variable;

typedef struct node_list {
        //
} node_list;

typedef struct node_file_block {
        //
} node_file_block;

typedef struct node_expression {
        //
} node_expression;

typedef struct node_function_call {
        //
} node_function_call;

typedef struct node_function {
        //
} node_function;

#endif  /* NODE_H */
