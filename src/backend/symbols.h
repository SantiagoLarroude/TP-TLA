#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdbool.h> /* bool */

#include "../frontend/syntactic-analysis/node.h" /* node_* types */

typedef enum SYMBOLS_STATUS {
        MULTIPLE_DECLARATION = -2,
        OUT_OF_SCOPE,
        SUCCESS,
} SYMBOLS_STATUS;

// (1 + 2) -> abc

bool initialize_table();

int insert_variable(variable *var);
int insert_dangling_variable(variable *var);
variable *lookup_variable(const char *name);
variable *lookup_dangling_variable(const char *name);

void insert_function(char *name);

void next_scope();
void prev_scope();

bool is_dangling(variable *var);
bool assign_scope_to_dangling_variable(variable *var); // Assigns current scope
unsigned count_dangling();

void free_table();
void free_node_function_call(node_function_call * fun_call);
void free_main_function(node_function * main_function);
void free_node_function(node_function * function);
void free_node_list(node_list * args);
void free_node_expression_list(node_expression_list * expression_list);
void free_node_list(node_list * args);
void free_variable(variable * variable);
void free_node_expression(node_expression * exprs);
void free_node_file_block(node_file_block* file_handler);
void free_node_loop(node_loop * loop_expr);

#endif /* SYMBOLS_H */
