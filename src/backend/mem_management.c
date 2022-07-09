#include <stdlib.h>

#include "logger.h"
#include "symbols.h"
#include "mem_management.h"

void free_program(program_t *root)
{
        LogDebug("%s(%p)", __func__, root);
        free_main_function(root);
        free_table();
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

        if (function == NULL)
                return;

        if (function->name != NULL) {
                free(function->name);
        }

        free_node_list(function->args);
        free_node_expression_list(function->expressions);
        free_variable(function->return_variable);
        free(function);
}

void free_node_list(node_list *args)
{
        LogDebug("%s(%p)", __func__, args);

        if (args == NULL)
                return;

        if (args->type != LIST_RANGE_TYPE) {
                for (int i = 0; i < args->len; i++) {
                        free_node_expression(args->exprs[i]);
                }

                free(args->exprs);
        }

        free(args);
}

void free_node_expression_list(node_expression_list *expression_list)
{
        LogDebug("%s(%p)", __func__, expression_list);

        if (expression_list == NULL)
                return;

        node_expression_list *current = expression_list;

        while (current != NULL) {
                node_expression_list *nextNode = current->next;

                free_node_expression(current->expr);
                free(current);

                current = nextNode;
        }
}

void free_variable(variable *variable)
{
        LogDebug("%s(%p)", __func__, variable);

        if (variable == NULL)
                return;

        if (variable->name != NULL)
                free(variable->name);

        if (variable->type == EXPRESSION_TYPE) {
                free_node_expression(variable->value.expr);
        } else if ((variable->type == STRING_TYPE ||
                    variable->type == FILE_PATH_TYPE) &&
                   variable->value.string != NULL) {
                free(variable->value.string);
        }

        free(variable);
}

void free_node_expression(node_expression *exprs)
{
        LogDebug("%s(%p)", __func__, exprs);

        if (exprs == NULL)
                return;

        free_variable(exprs->var);
        free_node_file_block(exprs->file_handler);
        free_node_list(exprs->list_expr);
        free_node_loop(exprs->loop_expr);
        free_node_function_call(exprs->fun_call);
        free_node_expression(exprs->expr);

        free(exprs);
}

void free_node_file_block(node_file_block *file_handler)
{
        LogDebug("%s(%p)", __func__, file_handler);

        if (file_handler == NULL)
                return;

        free_variable(file_handler->var);
        free_node_expression_list(file_handler->exprs_list);

        free(file_handler);
}

void free_node_loop(node_loop *loop_expr)
{
        LogDebug("%s(%p)", __func__, loop_expr);

        if (loop_expr == NULL)
                return;

        free_node_expression(loop_expr->iterable);
        free_node_expression(loop_expr->action);
        free_variable(loop_expr->var);

        free(loop_expr);
}

void free_node_function_call(node_function_call *fun_call)
{
        LogDebug("%s(%p)", __func__, fun_call);

        if (fun_call == NULL)
                return;

        node_function_call *current = fun_call;
        node_function_call *aux;

        while (current != NULL) {
                aux = current->next;
                free_variable(current->id);
                free_node_list(current->args);
                free(current);
                current = aux;
        }
}
