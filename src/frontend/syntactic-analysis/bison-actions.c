#include <ctype.h> /* isdigit */
#include <stdbool.h> /* bool type */
#include <stdlib.h> /* malloc, calloc */
#include <string.h> /* strlen, strcpy */

/* Backend */
#include "../../backend/logger.h" /* LogDebug */
#include "../../backend/error.h"
#include "../../backend/symbols.h"
#include "../../backend/mem_management.h" /* free_node_list */

/* Frontend */
#include "../lexical-analysis/flex-actions.h" /* enum TokenID; free_yylval */

/* This file */
#include "bison-actions.h"
static variable *create_variable();
static node_expression *create_node_expression();
static variable *lookup_variable_create_dangling(const char *id);

/**
 * Implementación de "bison-grammar.h".
 */

void yyerror(program_t *root, const char *string)
{
        const int length = strlen(yytext);

        LogError("Mensaje: '%s' debido a '%s' (linea %d).", string, yytext,
                 yylineno);
        LogError("Mensaje: '%s.", string);
        LogError("En ASCII es:");
        LogErrorRaw("\t");

        for (int i = 0; i < length; ++i) {
                LogErrorRaw("[%d]", yytext[i]);
        }

        LogErrorRaw("\n\n");
}

program_t *new_program()
{
        LogDebug("%s\n", __func__);

        program_t *program = (program_t *)malloc(sizeof(program_t));
        if (program == NULL) {
                error_no_memory();
                exit(1);
        }

        memset(program, 0, sizeof(program_t));

        // Hash table for variables
        if (initialize_table() == false) {
                LogError("Could not initialize variables table. Aborting.");
                free_program(program);
                exit(1);
        }

        return program;
}

int grammar_program(program_t *root, node_function *fun)
{
        if (fun == NULL)
                return 1;

        root->main_function = fun;

        state.result = 0;
        state.succeed = true;

        if (count_dangling() > 0) {
                state.result = COMPILER_STATE_RESULTS_DANGLING_VARIABLES;
                state.succeed = false;
        } else if (state.syntax_error == true) {
                state.result = COMPILER_STATE_RESULTS_SYNTAX_ERROR;
                state.succeed = false;
        }

        return 0;
}

node_function *grammar_new_function(const char *name, const node_list *args,
                                    const node_expression_list *list,
                                    const variable *return_variable)
{
        LogDebug("%s(%p)\n", __func__, list);

        node_function *fun = (node_function *)calloc(1, sizeof(node_function));
        if (fun == NULL) {
                error_no_memory();
                exit(1);
        }

        fun->name = strdup(name);
        fun->args = (node_list *)args;
        fun->expressions = (node_expression_list *)list;
        fun->return_variable = (variable *)return_variable;

        if (args != NULL) {
                for (size_t i = 0; i < args->len; i++) {
                        if (args->exprs[i] != NULL &&
                            args->exprs[i]->type == EXPRESSION_VARIABLE) {
                                assign_scope_to_dangling_variable(
                                        args->exprs[i]->var);
                        }
                }
        }

        return fun;
}

node_expression_list *grammar_new_expression_list(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_expression_list *list = (node_expression_list *)calloc(
                1, sizeof(node_expression_list));
        if (list == NULL) {
                error_no_memory();
                exit(1);
        }

        list->next = NULL;
        list->expr = (node_expression *)expr;

        return list;
}

node_expression_list *grammar_concat_expressions(node_expression_list *list,
                                                 const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        node_expression_list *new_list_node = (node_expression_list *)calloc(
                1, sizeof(node_expression_list));

        if (new_list_node == NULL) {
                error_no_memory();
                exit(1);
        }

        new_list_node->next = NULL;
        new_list_node->expr = (node_expression *)expr;

        node_expression_list *tmp = list;
        while (tmp->next != NULL)
                tmp = tmp->next;

        tmp->next = new_list_node;

        return list;
}

node_expression *grammar_expression_from_list(const node_list *list)
{
        LogDebug("%s(%p)\n", __func__, list);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_LIST;
        node->list_expr = (node_list *)list;

        return node;
}

node_expression *
grammar_expression_from_filehandler(const node_file_block *fhandler)
{
        LogDebug("%s(%p)\n", __func__, fhandler);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_FILE_HANDLE;
        node->file_handler = (node_file_block *)fhandler;

        return node;
}

node_function_call *
grammar_concat_function_call(node_function_call *fun_list,
                             node_function_call *fun_append)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_list, fun_append);

        node_function_call *aux = fun_list;
        while (aux->next != NULL)
                aux = aux->next;

        aux->next = (node_function_call *)fun_append;
        fun_append->prev = aux;

        return fun_list;
}

node_function_call *grammar_function_call(const char *fun_id,
                                          const node_list *args)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_id, args);

        variable *fun_id_table_entry = lookup_variable(fun_id);
        if (fun_id_table_entry == NULL ||
            fun_id_table_entry->type != FUNCTION_TYPE) {
                error_function_not_found(fun_id);
                exit(1);
        }

        node_function_call *node =
                (node_function_call *)calloc(1, sizeof(node_function_call));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->id = fun_id_table_entry;
        node->args = (node_list *)args; // Could be NULL
        node->next = NULL;
        node->prev = NULL;

        return node;
}

node_function_call *grammar_function_call_from_id(const char *id,
                                                  node_function_call *fun)
{
        LogDebug("%s(%p, %p)\n", __func__, id, fun);

        node_function_call *node =
                (node_function_call *)calloc(1, sizeof(node_function_call));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        variable *var = lookup_variable_create_dangling(id);

        node->id = var;
        node->next = fun;
        node->prev = NULL;

        fun->prev = node;

        return node;
}

node_expression *grammar_new_declaration_file_node(const char *fpath,
                                                   const char *id,
                                                   const node_list *separators)
{
        LogDebug("%s(%s, %p, %p)", __func__, fpath, id, separators);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_FILE_DECLARATION;
        node->list_expr = (node_list *)separators; // Could be NULL

        node->var = create_variable();

        node->var->type = FILE_PATH_TYPE;
        node->var->name = strdup(id);

        if (insert_variable(node->var) < SUCCESS) {
                error_multiple_declaration(id);
                exit(1);
        }

        node->var->value.string = strdup(fpath);

        if (separators != NULL) {
                if (separators->type != LIST_EXPRESSION_TYPE ||
                    separators->len != 1) {
                        error_invalid_separator_in_file_decalration_bad_type();
                        state.syntax_error = true;
                }

                // List with len != 0. Here are the strings with the separators
                node_list *separators_list = separators->exprs[0]->list_expr;

                for (int i = 0; i < separators_list->len; i++) {
                        if (separators_list->exprs[i]->type !=
                            EXPRESSION_GRAMMAR_CONSTANT_TYPE) {
                                error_invalid_separator_in_file_decalration_bad_type();
                                state.syntax_error = true;
                        } else if (separators_list->exprs[i]->var->type !=
                                   STRING_TYPE) {
                                error_invalid_separator_in_file_decalration_bad_type();
                                state.syntax_error = true;
                        } else {
                                // Alias
                                size_t len =
                                        strlen(separators_list->exprs[i]
                                                       ->var->value.string);

                                if (len < 3 || len > 4 ||
                                    (len == 4 &&
                                     separators_list->exprs[i]
                                                     ->var->value.string[1] !=
                                             '\\')) {
                                        // " + character + " = 3
                                        // " + \ + character + " = 4
                                        error_invalid_separator_in_file_decalration_bad_len(
                                                separators_list->exprs[i]
                                                        ->var->value.string);
                                        state.syntax_error = true;
                                }
                        }
                }
        }

        return node;
}

node_expression *
grammar_new_declaration_stdout_node(const char *id,
                                    const node_list *separators)
{
        LogDebug("%s(%p, %p)\n", __func__, id, separators);

        return grammar_new_declaration_file_node("STDOUT", id, separators);
}

node_file_block *grammar_using_file(const char *id,
                                    const node_expression_list *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        node_file_block *node =
                (node_file_block *)calloc(1, sizeof(node_file_block));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->var = lookup_variable(id);
        if (node->var == NULL) {
                error_variable_not_found(id);
                exit(1);
        }

        node->exprs_list = (node_expression_list *)expr;

        return node;
}

node_expression *grammar_new_variable(const char *id,
                                      const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        variable *id_var = create_variable();

        id_var->name = strdup(id);
        id_var->type = CONSTANT_TYPE;

        id_var->value.expr = (node_expression *)expr;

        if (insert_variable(id_var) < SUCCESS) {
                error_multiple_declaration(id);
                exit(1);
        }

        node_expression *node = create_node_expression();

        node->var = id_var;
        node->type = EXPRESSION_VARIABLE_DECLARATION;

        return node;
}

node_expression *grammar_new_conditional(const node_expression *condition,
                                         const node_expression *expr_true,
                                         const node_expression *expr_false)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, condition, expr_true,
                 expr_false);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_CONDITIONAL;

        node->conditional =
                (node_conditional *)calloc(1, sizeof(node_conditional));
        if (node->conditional == NULL) {
                error_no_memory();
                exit(1);
        }

        node->conditional->condition = (node_expression *)condition;
        node->conditional->true_condition = (node_expression *)expr_true;
        node->conditional->else_condition = (node_expression *)expr_false;

        return node;
}

node_expression *grammar_new_loop(const char *id,
                                  const node_expression *iterable,
                                  const node_expression *action)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, id, iterable, action);

        variable *var = lookup_dangling_variable(id);

        // TODO: Handle variable in previous scope
        if (var != NULL) {
                assign_scope_to_dangling_variable(var);
                var->type = LOOP_VARIABLE_TYPE;
        }

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_LOOP;

        node->loop_expr = (node_loop *)calloc(1, sizeof(node_loop));
        if (node->loop_expr == NULL) {
                error_no_memory();
                exit(1);
        }

        node->loop_expr->var = var;
        node->loop_expr->iterable = (node_expression *)iterable;
        node->loop_expr->action = (node_expression *)action;

        return node;
}

node_list *grammar_new_list(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_list *list = (node_list *)calloc(1, sizeof(node_list));
        if (list == NULL) {
                error_no_memory();
                exit(1);
        }

        list->exprs = (node_expression **)calloc(1, sizeof(node_expression *));
        if (list->exprs == NULL) {
                error_no_memory();
                exit(1);
        }

        list->len = 0;
        list->exprs[list->len] = (node_expression *)expr;

        if (expr == NULL) {
                list->type = LIST_BLANK_TYPE;
        } else {
                list->len++;
                list->type = LIST_EXPRESSION_TYPE;
        }

        return list;
}

node_list *grammar_concat_list_expr(node_list *list,
                                    const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        node_expression **tmp_arr = realloc(
                list->exprs, sizeof(node_expression *) * (1 + list->len));
        if (tmp_arr == NULL) {
                error_no_memory();
                exit(1);
        }

        tmp_arr[list->len++] = (node_expression *)expr;

        list->exprs = tmp_arr;

        return list;
}

node_list *grammar_concat_list_list(node_list *head_list,
                                    const node_list *tail_list)
{
        LogDebug("%s(%p, %p)\n", __func__, head_list, tail_list);

        size_t new_len = tail_list->len + head_list->len;

        node_expression **tmp_arr =
                realloc(head_list->exprs, sizeof(node_expression *) * new_len);
        if (tmp_arr == NULL) {
                error_no_memory();
                exit(1);
        }

        for (size_t i = head_list->len; i < new_len; i++) {
                tmp_arr[i] = tail_list->exprs[i - head_list->len];
        }

        head_list->exprs = tmp_arr;
        head_list->len = new_len;

        free_node_list((node_list *)tail_list);

        return head_list;
}

node_list *grammar_new_list_from_range(const char *start, const char *end)
{
        LogDebug("%s(%p, %p)\n", __func__, start, end);

        if (start == NULL || end == NULL) {
                return NULL;
        }

        long range_start = atol(start);
        long range_end = atol(end);

        if ((range_start == 0 && isdigit(start[0]) == 0) ||
            (range_end == 0 && isdigit(end[0]) == 0)) {
                error_invalid_range(start, end);
                exit(1);
        }

        if (range_start > range_end) {
                error_invalid_range(start, end);
                exit(1);
        }

        node_list *list = (node_list *)calloc(1, sizeof(node_list));
        if (list == NULL) {
                error_no_memory();
                exit(1);
        }

        list->len = labs(range_end - range_start);
        list->type = LIST_RANGE_TYPE;
        list->from = range_start;
        list->to = range_end;

        return list;
}

node_list *grammar_new_list_args(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_list *list = (node_list *)calloc(1, sizeof(node_list));
        if (list == NULL) {
                error_no_memory();
                exit(1);
        }

        list->len = 0;
        list->type = LIST_ARGS_TYPE;

        list->exprs = (node_expression **)calloc(1, sizeof(node_expression *));
        if (list->exprs == NULL) {
                error_no_memory();
                exit(1);
        }

        list->exprs[list->len] = (node_expression *)expr;
        if (expr == NULL) {
                list->len = 0;
        } else {
                list->len++;
        }

        return list;
}

node_list *grammar_new_list_args_from_id(const char *id)
{
        node_expression *variable_node = create_node_expression();

        variable_node->type = EXPRESSION_VARIABLE;
        variable_node->var = lookup_variable_create_dangling(id);

        return grammar_new_list_args(variable_node);
}

node_list *grammar_concat_list_args(node_list *list,
                                    const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        if (expr == NULL)
                return list;

        node_expression **expanded = (node_expression **)realloc(
                list->exprs, (1 + list->len) * sizeof(node_expression *));
        if (expanded == NULL) {
                error_no_memory();
                exit(1);
        }

        expanded[list->len] = (node_expression *)expr;
        list->exprs = expanded;
        list->len += 1;
        list->type = LIST_ARGS_TYPE;

        return list;
}

node_list *grammar_concat_list_args_with_id(node_list *list, const char *id)
{
        node_expression *variable_node = create_node_expression();

        variable_node->type = EXPRESSION_VARIABLE;
        variable_node->var = lookup_variable_create_dangling(id);

        return grammar_concat_list_args(list, variable_node);
}

node_expression *
grammar_expression_arithmetic_num_add(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_NUMBER_ARITHMETIC_ADD;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_arithmetic_num_sub(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_NUMBER_ARITHMETIC_SUB;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_arithmetic_num_mul(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_NUMBER_ARITHMETIC_MUL;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_arithmetic_num_div(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_NUMBER_ARITHMETIC_DIV;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_arithmetic_num_mod(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_NUMBER_ARITHMETIC_MOD;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_arithmetic_str_add(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_STR_ARITHMETIC_ADD;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_arithmetic_str_sub(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_STR_ARITHMETIC_SUB;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *grammar_expression_bool_and(const node_expression *lvalue,
                                             const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_BOOLEAN_AND;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *grammar_expression_bool_or(const node_expression *lvalue,
                                            const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_BOOLEAN_OR;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *grammar_expression_bool_not(const node_expression *value)
{
        LogDebug("%s(%p)\n", __func__, value);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_BOOLEAN_NOT;
        node->expr = (node_expression *)value;

        return node;
}

node_expression *grammar_expression_cmp_equals(const node_expression *lvalue,
                                               const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_COMPARE_EQUALS;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_cmp_not_equals(const node_expression *lvalue,
                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_COMPARE_NOT_EQUALS;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_cmp_greater_than(const node_expression *lvalue,
                                    const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_COMPARE_GREATER_THAN;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_cmp_greater_equal(const node_expression *lvalue,
                                     const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_COMPARE_GREATER_EQUAL;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_cmp_less_than(const node_expression *lvalue,
                                 const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_COMPARE_LESS_THAN;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *
grammar_expression_cmp_less_equal(const node_expression *lvalue,
                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_COMPARE_LESS_EQUAL;
        node->left = (node_expression *)lvalue;
        node->right = (node_expression *)rvalue;

        return node;
}

node_expression *grammar_expression_cmp_by_type(const char *id,
                                                const char *type)
{
        LogDebug("%s(%p, %d)\n", __func__, id, type);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_VARIABLE_TYPE_COMPARISON;

        if (strcmp(type, "File") == 0) {
                node->compare_type = FILE_PATH_TYPE;
        } else if (strcmp(type, "Number") == 0) {
                node->compare_type = NUMBER_TYPE;
        } else if (strcmp(type, "String") == 0) {
                node->compare_type = STRING_TYPE;
        } else if (strcmp(type, "Boolean") == 0) {
                node->compare_type = BOOL_TYPE;
        }

        node->var = lookup_variable_create_dangling(id);

        return node;
}

node_expression *
grammar_expression_from_funcall(const node_function_call *fn_calls)
{
        LogDebug("%s(%p)\n", __func__, fn_calls);

        node_expression *node = create_node_expression();

        node->fun_call = (node_function_call *)fn_calls;
        node->type = EXPRESSION_FUNCTION_CALL;

        return node;
}

variable *grammar_new_return_node(const char *id)
{
        LogDebug("%s(%p)\n", __func__, id);

        if (id == NULL)
                return NULL;

        variable *var = lookup_variable(id);
        if (var == NULL) {
                error_variable_not_found(id);
                exit(1);
        }

        return var;
}

node_expression *grammar_new_assignment_expression(const node_expression *expr,
                                                   const char *id)
{
        LogDebug("%s(%p, %p)\n", __func__, expr, id);

        variable *var = lookup_variable(id);
        if (var == NULL) {
                error_variable_not_found(id);
                exit(1);
        }

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_VARIABLE_ASSIGNMENT;

        node->var = var;
        node->expr = (node_expression *)expr;

        return node;
}

node_expression *grammar_new_assignment_from_id(const char *from,
                                                const char *to)
{
        LogDebug("%s(%s, %s)\n", __func__, from, to);

        variable *var = lookup_variable_create_dangling(from);

        node_expression *node = create_node_expression();

        node->type = VARIABLE_TYPE;

        node->var = var;

        return grammar_new_assignment_expression(node, to);
}

node_expression *grammar_expression_type_from_id(const char *id)
{
        variable *var = lookup_variable_create_dangling(id);

        node_expression *node = create_node_expression();

        node->type = VARIABLE_TYPE;

        node->var = var;

        return node;
}

node_expression *grammar_constant_bool(const char *rval)
{
        LogDebug("%s(%s)\n", __func__, rval);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_GRAMMAR_CONSTANT_TYPE;

        node->var = create_variable();

        node->var->type = BOOL_TYPE;
        if (strcmp(rval, "True") == 0) {
                node->var->value.boolean = true;
        } else {
                node->var->value.boolean = false;
        }

        return node;
}

node_expression *grammar_constant_number(const char *rval)
{
        LogDebug("%s(%s)\n", __func__, rval);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_GRAMMAR_CONSTANT_TYPE;

        node->var = create_variable();

        node->var->type = NUMBER_TYPE;
        node->var->value.number = atof(rval);

        return node;
}

node_expression *grammar_constant_string(const char *rval)
{
        LogDebug("%s(%s)\n", __func__, rval);

        node_expression *node = create_node_expression();

        node->type = EXPRESSION_GRAMMAR_CONSTANT_TYPE;

        node->var = create_variable();

        node->var->type = STRING_TYPE;
        node->var->value.string = strdup(rval);

        return node;
}

static node_expression *create_node_expression()
{
        LogDebug("%s()\n", __func__);

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        return node;
}

static variable *create_variable()
{
        LogDebug("%s()\n", __func__);

        variable *var = (variable *)calloc(1, sizeof(variable));
        if (var == NULL) {
                error_no_memory();
                exit(1);
        }

        return var;
}

static variable *lookup_variable_create_dangling(const char *id)
{
        variable *var = lookup_variable(id);
        if (var != NULL)
                return var;

        var = lookup_dangling_variable(id);
        if (var != NULL)
                return var;

        // Store as dangling variable
        var = create_variable();

        var->type = UNKNOWN_TYPE;
        var->name = strdup(id);

        if (insert_dangling_variable(var) < SUCCESS) {
                error_multiple_declaration(id);
                exit(1);
        }

        return var;
}
