#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

typedef unsigned int token_t;

typedef struct variable variable;
typedef struct node_list node_list;

typedef struct node_file_block node_file_block;
typedef struct node_expression node_expression;
typedef struct node_function_call node_function_call;
typedef struct node_function node_function;
// typedef struct loop_node loop_node;
// typedef struct conditional_node conditional_node;
typedef struct node_expression_list node_expression_list;
typedef struct list list;

typedef struct program_t {
        node_function* main_function;
} program_t;

typedef union variable_value {
        bool boolean;
        double number;
        char* string;
        node_expression* expr;
} variable_value;

typedef enum NODE_T {
        NUMBER_TYPE = 1,
        BOOL_TYPE,
        STRING_TYPE,
        FILE_PATH_TYPE,
        VARIABLE_TYPE,
        EXPRESSION_TYPE,
        LIST_ARGS_TYPE,
        LIST_EXPRESSION_TYPE,
        LIST_BLANK_TYPE,
        EXPRESSION_GRAMMAR_CONSTANT_TYPE,
        EXPRESSION_VARIABLE_ASSIGNMENT,
        EXPRESSION_VARIABLE_DECLARATION,
        EXPRESSION_FILE_DECLARATION,
} NODE_T;

struct variable {
        char* name;

        token_t type; // si es un str, number, id, boolean,
        variable_value value;
};

struct node_expression {
        variable* var;
        node_file_block* fileHandler;
        node_list* listExpr;
        node_expression* expr;

        node_function_call* function_call_pointer;

        token_t type; // en que produccion se hizo
};

struct node_list {
        node_expression** exprs;
        size_t len;
        token_t type;
};

struct node_file_block {
        char* id;
        node_expression* expr;
};

struct node_function_call {
        char* id;
        node_function_call* function_call;
        node_function_call* function_append;
        node_list* node_list;
};

struct node_expression_list {
        struct node_expression_list* next;
        node_expression* expr;
};

struct node_function {
        char* name;
        node_list* args;
        node_expression_list* expressions;
        variable* return_variable;
};

#endif  /* NODE_H */
