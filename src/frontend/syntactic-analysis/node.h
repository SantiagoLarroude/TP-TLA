#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

typedef struct variable variable;
typedef struct node_list node_list;
typedef struct node_file_block node_file_block;
typedef struct node_expression node_expression;
typedef struct node_function_call node_function_call;
typedef struct node_function node_function;
typedef struct loop_node loop_node;
typedef struct conditional_node conditional_node;
typedef struct list list;

typedef struct program {
        node_function* value;
} program;

typedef union variable_value {
        bool boolean;
        double number;
        char * string;
} variable_value;

typedef enum GRAMMAR_CONSTANT_TYPE{
        NUMBER_TYPE = 1,
        STRING_TYPE,
        BOOL_TYPE,
        ID_TYPE
} GRAMMAR_CONSTANT_TYPE;

typedef enum GRAMMAR_RETURN_TYPE{
        RETURN__ID = 1,
        RETURN_NO_ID
} GRAMMAR_RETURN_TYPE;

typedef enum GRAMMAR_EXPRESSION_CMP_TYPE{
        CMP_EXP_EQUALS = 1,
        CMP_EXP_NOT_EQUALS,        
        CMP_EXP_GREATER_THAN,
        CMP_EXP_GREATER_EQUAL,
        CMP_EXP_LESS_THAN,
        CMP_EXP_LESS_EQUAL,
        CMP_EXP_IS,
} GRAMMAR_EXPRESSION_CMP_TYPE;

typedef enum GRAMMAR_BOOLEAN_TYPE{
        BOOL_TYPE_AND = 1,
        BOOL_TYPE_OR,
        BOOL_TYPE_NOT,
        BOOL_TYPE_NORMAL,
        BOOL_TYPE_IN_PARENTHESIS,
} GRAMMAR_BOOLEAN_TYPE;

typedef enum GRAMMAR_STRING_ARITHMETIC_TYPE {
        ARITHMETIC_ADD = 1,
        ARITHMETIC_SUB,
} GRAMMAR_STRING_ARITHMETIC_TYPE;

typedef enum GRAMMAR_NUMBER_ARITHMETIC_TYPE {
        NUMBER_ARITHMETIC_ADD = 1,
        NUMBER_ARITHMETIC_SUB,
        NUMBER_ARITHMETIC_MUL,
        NUMBER_ARITHMETIC_DIV,
        NUMBER_ARITHMETIC_MOD
} GRAMMAR_NUMBER_ARITHMETIC_TYPE;

typedef enum ARGUMENTS_LIST_TYPE {
        BLANK_TYPE = 1,
        EXPRESSION_TYPE,
        COMMA_TYPE
} ARGUMENTS_LIST_TYPE;

typedef enum LIST_TYPE {
        LIST_BLANK_TYPE = 1,
        LIST_EXPRESSION_TYPE,
        LIST_RANGE_TYPE,
        LIST_COMMA_EXPRESSION_TYPE,
        LIST_COMMA_RANGE_TYPE,
} LIST_TYPE;

typedef enum FILE_DECLARATION_TYPE {
        FILE_STRING_TYPE = 1,
        STDOUT_STRING_TYPE,
        FILE_FSTREAM_STDOUT_TYPE,
        STDOUT_FSTREAM_STDOUT_TYPE

} FILE_DECLARATION_TYPE;

typedef enum NODE_EXPRESSION_TYPE {
        EXPRESSION_VARIABLE,
        EXPRESSION_LIST,
        EXPRESSION_FILE_BLOCK,
        EXPRESSION_GRAMMAR_CONSTANT_TYPE,
        EXPRESSION_GRAMMAR_EXPRESSION_CMP_TYPE,
        EXPRESSION_BOOLEAN_TYPE,
        EXPRESSION_GRAMMAR_STRING_ARITHMETIC_TYPE,
        EXPRESSION_GRAMMAR_NUMBER_ARITHMETIC_TYPE,
        EXPRESSION_RETURN_TYPE,
        EXPRESSION_FILE_DECLARATION_TYPE,
        EXPRESSION_NODE_FUNCTION_CALL,
} NODE_EXPRESSION_TYPE;

struct variable {
        token_t type;
        char * name;
        variable_value value;
};

struct node_expression {
        variable * var;
        list * listExpr;
        node_file_block * fileHandler;
        node_list* node_list;
        GRAMMAR_CONSTANT_TYPE g_constant_type;
        GRAMMAR_EXPRESSION_CMP_TYPE g_expression_cmp_type;
        GRAMMAR_BOOLEAN_TYPE g_boolean_type;
        GRAMMAR_STRING_ARITHMETIC_TYPE g_string_arithmetic_type;
        GRAMMAR_NUMBER_ARITHMETIC_TYPE g_number_arithmetic_type;
        GRAMMAR_RETURN_TYPE g_return_type;
        FILE_DECLARATION_TYPE f_declaration_type;
        node_function_call * function_call_pointer;
        token_t type;
} ;

struct node_list {
        node_expression ** exprs;
        size_t len;
        token_t type;
};

struct list {
        size_t len;
        token_t type;
};

struct node_file_block {
        node_expression *id;
        node_expression *expr;
} ;

struct node_function_call {
        node_expression* id;
        node_function_call* function_call;
        node_function_call* function_append;
        node_list* node_list;
} ;

struct node_function {
        node_expression* expr;
} ;



// las creo santi que de ultima las reemplazamos o
// eliminamos pero asi entiendo mejor

// esto viene de querer hacerlo desde el mas simple o
// basico de los no terminales/terminales
// que es como lo mostro agustin en la clase

// typedef struct identifier identifier;

// struct identifier {
//         char* id;
// } identifier;

// struct expression {
//         identifier* id;
//         variable_value var;
//         token_t type;
// } expression;

struct loop_node {
        node_expression* id;
        node_expression* iterable;
        node_expression* action;
} ;

struct conditional_node {
        node_expression* condition;
        node_expression* expr_true;
        node_expression* expr_false;
} ;



#endif  /* NODE_H */
