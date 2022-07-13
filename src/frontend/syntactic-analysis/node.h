#ifndef NODE_H
#define NODE_H

#include <stddef.h> /* size_t */
#include <stdbool.h>

typedef unsigned int token_t;

typedef struct variable variable;
typedef struct node_list node_list;

typedef struct node_file_block node_file_block;
typedef struct node_expression node_expression;
typedef struct node_loop node_loop;
typedef struct node_conditional node_conditional;

typedef struct node_function_call node_function_call;
typedef struct node_function node_function;
typedef struct node_expression_list node_expression_list;
typedef struct list list;

typedef struct program_t {
        node_function *main_function;
} program_t;

typedef union variable_value {
        bool boolean;
        double number;
        char *string;
        node_expression *expr;
} variable_value;

typedef enum NODE_T {
        UNKNOWN_TYPE = 0,
        NUMBER_TYPE = 1,
        BOOL_TYPE,
        STRING_TYPE,
        FILE_PATH_TYPE,
        LOOP_VARIABLE_TYPE,
        VARIABLE_TYPE,
        FUNCTION_TYPE,
        CONSTANT_TYPE,
        LIST_ARGS_TYPE,
        LIST_EXPRESSION_TYPE,
        LIST_RANGE_TYPE,
        LIST_BLANK_TYPE,
        EXPRESSION_GRAMMAR_CONSTANT_TYPE,
        EXPRESSION_VARIABLE_ASSIGNMENT,
        EXPRESSION_VARIABLE_DECLARATION,
        EXPRESSION_VARIABLE_WITH_FILTER,
        EXPRESSION_VARIABLE,
        EXPRESSION_FILE_DECLARATION,
        EXPRESSION_LOOP,
        EXPRESSION_LIST,
        EXPRESSION_FUNCTION_CALL,
        EXPRESSION_FILE_HANDLE,
        EXPRESSION_STR_ARITHMETIC_ADD,
        EXPRESSION_STR_ARITHMETIC_SUB,
        EXPRESSION_NUMBER_ARITHMETIC_ADD,
        EXPRESSION_NUMBER_ARITHMETIC_SUB,
        EXPRESSION_NUMBER_ARITHMETIC_MUL,
        EXPRESSION_NUMBER_ARITHMETIC_DIV,
        EXPRESSION_NUMBER_ARITHMETIC_MOD,
        EXPRESSION_BOOLEAN_NOT,
        EXPRESSION_BOOLEAN_AND,
        EXPRESSION_BOOLEAN_OR,
        EXPRESSION_COMPARE_EQUALS,
        EXPRESSION_COMPARE_NOT_EQUALS,
        EXPRESSION_COMPARE_GREATER_THAN,
        EXPRESSION_COMPARE_GREATER_EQUAL,
        EXPRESSION_COMPARE_LESS_THAN,
        EXPRESSION_COMPARE_LESS_EQUAL,
        EXPRESSION_VARIABLE_TYPE_COMPARISON,
        EXPRESSION_CONDITIONAL,
} NODE_T;

struct variable {
        char *name;

        token_t type; // si es un str, number, id, boolean,
        variable_value value;
};

struct node_expression {
        variable *var;
        token_t compare_type;

        node_file_block *file_handler;
        node_list *list_expr;
        node_loop *loop_expr;
        node_conditional *conditional;
        union {
                node_expression *expr;
                struct {
                        node_expression *left;
                        node_expression *right;
                };
        };

        node_function_call *fun_call;

        token_t type; // en que produccion se hizo
};

struct node_list {
        union {
                struct {
                        long from;
                        long to;
                };

                node_expression **exprs;
        };
        size_t len;
        token_t type;
};

struct node_file_block {
        variable *var;
        node_expression_list *exprs_list;
};

struct node_loop {
        variable *var;
        node_expression *iterable;
        node_expression *action;
};

struct node_conditional {
        node_expression *condition;
        node_expression *true_condition;
        node_expression *else_condition;
};

struct node_function_call {
        variable *id;
        node_list *args;

        node_function_call *next;
};

struct node_expression_list {
        node_expression_list *next;
        node_expression *expr;
};

struct node_function {
        char *name;
        node_list *args;
        node_expression_list *expressions;
        variable *return_variable;
};

#endif /* NODE_H */
