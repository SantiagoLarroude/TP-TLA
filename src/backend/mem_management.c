#include <stdlib.h>
#include <string.h> /* memset */
#include <stdbool.h>

#include "error.h"
#include "logger.h"
#include "symbols.h"
#include "mem_management.h"

typedef struct dynamic_ptr_arr {
        void **array;
        size_t len; // Array size
        size_t elements; // Number of elements with valid content
} dynamic_ptr_arr;

static dynamic_ptr_arr free_addresses;

static void init_free_address_list();
static bool has_been_freed(void *ptr);
static void free_free_address_list();

void init_mem_manager()
{
        init_free_address_list();
}

void free_program(program_t *root)
{
        LogDebug("%s(%p)", __func__, root);

        if (root != NULL) {
                free_main_function(root);
        }

        free_table();

        free_free_address_list();

        if (root != NULL) {
                free(root);
        }
}

void free_main_function(program_t *program)
{
        LogDebug("%s(%p)", __func__, program);

        if (program == NULL)
                return;

        free_node_function(program->main_function);
}

void free_node_function(node_function *function)
{
        LogDebug("%s(%p)", __func__, function);

        if (function == NULL || has_been_freed(function))
                return;

        free_and_keep_address(function->name);

        free_node_list(function->args);
        free_node_expression_list(function->expressions);
        free_variable(function->return_variable);

        free_and_keep_address(function);
}

void free_node_list(node_list *args)
{
        LogDebug("%s(%p)", __func__, args);

        if (args == NULL || has_been_freed(args))
                return;

        if (args->type != LIST_RANGE_TYPE) {
                for (int i = 0; i < args->len; i++) {
                        free_node_expression(args->exprs[i]);
                }

                free_and_keep_address(args->exprs);
        }

        free_and_keep_address(args);
}

void free_node_expression_list(node_expression_list *expression_list)
{
        LogDebug("%s(%p)", __func__, expression_list);

        if (expression_list == NULL || has_been_freed(expression_list))
                return;

        node_expression_list *current = expression_list;

        while (current != NULL) {
                node_expression_list *nextNode = current->next;

                free_node_expression(current->expr);

                free_and_keep_address(current);

                current = nextNode;
        }
}

void free_variable(variable *variable)
{
        LogDebug("%s(%p)", __func__, variable);

        if (variable == NULL || has_been_freed(variable))
                return;

        free_and_keep_address(variable->name);

        if (variable->type == CONSTANT_TYPE) {
                free_node_expression(variable->value.expr);
        } else if (variable->type == STRING_TYPE ||
                   variable->type == FILE_PATH_TYPE) {
                free_and_keep_address(variable->value.string);
        }

        free_and_keep_address(variable);
}

void free_node_expression(node_expression *exprs)
{
        LogDebug("%s(%p)", __func__, exprs);

        if (exprs == NULL || has_been_freed(exprs))
                return;

        free_variable(exprs->var);
        free_node_file_block(exprs->file_handler);
        free_node_list(exprs->list_expr);
        free_node_loop(exprs->loop_expr);
        free_node_function_call(exprs->fun_call);

        if (exprs->type == EXPRESSION_VARIABLE_ASSIGNMENT ||
            exprs->type == EXPRESSION_BOOLEAN_NOT) {
                free_node_expression(exprs->expr);
        } else {
                free_node_expression(exprs->left);
                free_node_expression(exprs->right);
        }

        free_and_keep_address(exprs);
}

void free_node_file_block(node_file_block *file_handler)
{
        LogDebug("%s(%p)", __func__, file_handler);

        if (file_handler == NULL || has_been_freed(file_handler))
                return;

        free_variable(file_handler->var);
        free_node_expression_list(file_handler->exprs_list);

        free_and_keep_address(file_handler);
}

void free_node_loop(node_loop *loop_expr)
{
        LogDebug("%s(%p)", __func__, loop_expr);

        if (loop_expr == NULL || has_been_freed(loop_expr))
                return;

        free_node_expression(loop_expr->iterable);
        free_node_expression(loop_expr->action);
        free_variable(loop_expr->var);

        free_and_keep_address(loop_expr);
}

void free_node_function_call(node_function_call *fun_call)
{
        LogDebug("%s(%p)", __func__, fun_call);

        if (fun_call == NULL || has_been_freed(fun_call))
                return;

        node_function_call *current = fun_call;
        node_function_call *aux;

        while (current != NULL) {
                aux = current->next;
                free_variable(current->id);
                free_node_list(current->args);

                free_and_keep_address(current);

                current = aux;
        }
}

void free_and_keep_address(void *ptr)
{
        if (ptr == NULL || has_been_freed(ptr) == true)
                return;

        if (free_addresses.elements == free_addresses.len) {
                free_addresses.len += 10;

                free_addresses.array =
                        (void **)realloc(free_addresses.array,
                                         free_addresses.len * sizeof(void *));
                if (free_addresses.array == NULL) {
                        error_no_memory();
                        exit(1);
                }

                memset(free_addresses.array + free_addresses.elements, 0,
                       free_addresses.len - free_addresses.elements);
        }

        free_addresses.array[free_addresses.elements++] = ptr;

        free(ptr);
}

static void init_free_address_list()
{
        free_addresses.elements = 0;
        free_addresses.len = 50;
        free_addresses.array =
                (void **)calloc(free_addresses.len, sizeof(void *));
        if (free_addresses.array == NULL) {
                error_no_memory();
                exit(1);
        }
}

static bool has_been_freed(void *ptr)
{
        if (ptr == NULL)
                return true;

        for (size_t i = 0; i < free_addresses.elements; ++i)
                if (free_addresses.array[i] == ptr)
                        return true;

        return false;
}

static void free_free_address_list()
{
        if (free_addresses.array == NULL)
                return;

        memset(free_addresses.array, 0, free_addresses.len);
        free_addresses.len = 0;
        free_addresses.elements = 0;

        free(free_addresses.array);
}
