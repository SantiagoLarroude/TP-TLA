#ifndef AST_H
#define AST_H
#include "node.h"

typedef enum node_type{
    ROOT = 0,
    DECLARATION_N,
    CONSTANT_N,
    CTL_N,
    LOOP_N,
    FUNCALL_N,
    FUNDECL_N,
    ASSIGN_N,
    VARIABLE_REF_N,
    EXPRESSION_N,
    RETURN_N,
    DECL_ASSIGN_N,
}node_type;

typedef struct if_node_t{
    node_type type;
    struct expression_node_t * condition;
    struct list_node_t * then;
}if_node_t;
#define PADDING_SIZE sizeof(if_node_t)
typedef struct ast_node_t{
    node_type type;
    char padding[PADDING_SIZE];
} ast_node_t;

typedef struct list_node_t{
    ast_node_t * node;
    struct list_node_t * next;
}list_node_t;

typedef struct root_node_t{
    node_type type;
    list_node_t* global_variables;
    list_node_t* functions;
    list_node_t* main;
    int easter_egg;
}root_node_t;

typedef struct while_node_t{
    node_type type;
    ast_node_t * condition;
    ast_node_t * routine;
}while_node_t;


typedef struct declaration_node_t{
    node_type type;
    variable * var;
    int terminal;
    
}declaration_node_t;

typedef struct expression_node_t{
    node_type type;
    var_type expression_type;
}expression_node_t;
typedef struct assign_node_t{
    node_type type;
    variable * var;
    expression_node_t * expression;
    
}assign_node_t;

typedef struct function_node_t{
    node_type type;
    function * function;
    list_node_t * parameters;
    list_node_t * code;
} function_node_t;

typedef struct compound_expression_node_t{
    node_type type;
    var_type expression_type;
    expression_node_t * left;
    expression_node_t * right;
    char operator;
    int closed;
}compound_expression_node_t;

typedef struct variable_node_t{
    node_type type;
    var_type var_type;
    variable * var;
}variable_node_t;

typedef struct const_node_t{
    node_type type;
    var_type constant_type;
    variable_value value;
}const_node_t;

typedef struct func_call_node_t{
    node_type type;
    var_type ret_type;
    char * name;
    list_node_t * params;
    int terminal;
}func_call_node_t;

typedef struct return_node_t{
    node_type type;
    expression_node_t * expression;
}return_node_t;

typedef struct decl_assign_node_t{
    node_type type;
    variable * var;
    expression_node_t * expression;
    
}decl_assign_node_t;



list_node_t * concat_lists(list_node_t * list1, list_node_t * list2 );
list_node_t * concat_node(list_node_t * list, ast_node_t * code );
root_node_t * new_root_node();
declaration_node_t * new_declaration_node(char * name,var_type type);
assign_node_t* new_assign_node(char * name, expression_node_t * expression);
expression_node_t * not_expression_node(expression_node_t * right);
expression_node_t* new_compose_expr_node(expression_node_t * left,char operator,expression_node_t * right);

if_node_t* new_if_node(expression_node_t * condition,list_node_t*  code);
while_node_t* new_loop_node(expression_node_t * condition,list_node_t*  code);

variable_node_t* new_var_node(char * variable);
const_node_t* new_const_node(variable_value value);
list_node_t * new_param_decl_node(char * name, var_type type);
list_node_t * new_param_node(expression_node_t * expression_node);
function_node_t * new_function_node( char * name, var_type type, list_node_t * params, list_node_t * code, return_node_t * return_node);
func_call_node_t * new_function_call_node(char * name, list_node_t * params);
return_node_t * new_return_node(expression_node_t * expression);
decl_assign_node_t * new_assign_decl_node(char * name, var_type type, expression_node_t * expr);

const_node_t * new_double_node(variable_value value);
const_node_t * new_int_node(variable_value value);
const_node_t * new_string_node(variable_value value);
const_node_t * new_true_node();
const_node_t * new_false_node();
void set_terminal(func_call_node_t * fun_call);
void set_closed(expression_node_t * expr);
void set_easter_egg(root_node_t * root);
list_node_t * new_lambda_function(root_node_t * root,list_node_t * code, return_node_t * ret);
func_call_node_t * new_join_call(char * var1, char * var2);
//FREE

void free_root_node(root_node_t root); // root_node_t *
void free_node_list(list_node_t first);

#endif