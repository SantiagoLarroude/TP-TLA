#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

typedef struct variable variable;
typedef struct node_list node_list;
typedef struct node_file_block node_file_block;
typedef struct node_expression node_expression;
typedef struct node_function_call node_function_call;
typedef struct node_function node_function;


typedef union variable_value {
        bool boolean;
        long long integer;
        double floating;
        char * string;
} variable_value;

typedef enum GRAMMAR_CONSTANT_TYPE{
        NUMBER_TYPE,
        STRING_TYPE,
        BOOL_TYPE,
        LIST_TYPE
} GRAMMAR_CONSTANT_TYPE;

typedef enum GRAMMAR_RETURN_TYPE{
        RETURN__ID,
        RETURN_NO_ID
} GRAMMAR_RETURN;

typedef enum GRAMMAR_EXPRESSION_CMP_TYPE{
        CMP_EXP_EQUALS,
        CMP_EXP_NOT_EQUALS,        
        CMP_EXP_GREATER_THAN,
        CMP_EXP_GREATER_EQUAL,
        CMP_EXP_LESS_THAN,
        CMP_EXP_LESS_EQUAL,
        CMP_EXP_IS,
} GRAMMAR_EXPRESSION_CMP_TYPE;

typedef enum GRAMMAR_BOOLEAN_TYPE{
        BOOL_TYPE_AND,
        BOOL_TYPE_OR,
        BOOL_TYPE_NOT,
        BOOL_TYPE_NORMAL,
        BOOL_TYPE_IN_PARENTHESIS,
} GRAMMAR_BOOLEAN_TYPE;

typedef enum GRAMMAR_STRING_ARITHMETIC {
        ARITHMETIC_ADD,
        ARITHMETIC_SUB,
} GRAMMAR_STRING_ARITHMETIC;

typedef enum GRAMMAR_NUMER_ARITHMETIC {
        NUMBER_ARITHMETIC_ADD,
        NUMBER_ARITHMETIC_SUB,
        NUMBER_ARITHMETIC_MUL,
        NUMBER_ARITHMETIC_DIV,
        NUMBER_ARITHMETIC_MOD
} GRAMMAR_NUMER_ARITHMETIC;

struct variable {
        token_t type;
        char * name;
        variable_value value;
} variable;


struct node_list {
        node_expression * expr;
        node_expression * next;
} node_list;

struct list {
        node_list * head;
} list;

struct node_file_block {
        //
} node_file_block;

struct node_expression {
        GRAMMAR_CONSTANT_TYPE g_constant_type;
        GRAMMAR_EXPRESSION_CMP_TYPE g_expression_cmp_type;
        GRAMMAR_RETURN_TYPE g_return_type;
        GRAMMAR_BOOLEAN_TYPE g_boolean_type;
        variable * var;
} node_expression;

struct node_function_call {
        //
} node_function_call;

struct node_function {
        //
} node_function;

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


#endif  /* NODE_H */
