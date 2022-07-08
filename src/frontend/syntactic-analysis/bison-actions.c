#include <ctype.h> /* isdigit */
#include <stdbool.h> /* bool type */
#include <stdlib.h> /* malloc, calloc */
#include <string.h> /* strlen, strcpy */

/* Backend */
#include "../../backend/logger.h" /* LogDebug */
#include "../../backend/error.h"
#include "../../backend/symbols.h"

/* Frontend */
#include "../lexical-analysis/flex-actions.h" /* enum TokenID; free_yylval */

/* This file */
#include "bison-actions.h"

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

        program->main_function = NULL;

        // Hash table for variables
        if (initialize_table() == false) {
                LogError("Could not initialize variables table. Aborting.");
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

        return fun;

        // node_function* newFuncNode = calloc(1, sizeof(node_function));
        // newFuncNode->expr = calloc(1, sizeof(node_expression));
        // newFuncNode->expr = (node_expression*) expr;

        // state.succeed = true;

        // return newFuncNode;
}

node_expression_list *grammar_new_expression_list(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_expression_list *list =
                (node_expression_list *)calloc(1, sizeof(node_expression_list));
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

        node_expression_list *new_list_node =
                (node_expression_list *)calloc(1, sizeof(node_expression_list));

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

        return NULL;

        // node_expression* expr = (node_expression*) malloc(sizeof(node_expression));

        // expr->list_expr = (node_list*) list;
        // expr->type = EXPRESSION_LIST;

        // return expr;
}

node_expression *
grammar_expression_from_filehandler(const node_file_block *fhandler)
{
        LogDebug("%s(%p)\n", __func__, fhandler);

        return NULL;

        // node_expression* fileExpr = calloc(1, sizeof(node_expression));
        // fileExpr->fileHandler = (node_file_block*) fhandler;

        // return fileExpr;
}

node_expression *
grammar_expression_from_assignment(const node_expression *assignment)
{
        LogDebug("%s(%p)\n", __func__, assignment);

        return NULL;

        // return (node_expression*) assignment;
}

node_function_call *
grammar_concat_function_call(const node_function_call *fun_list,
                             const node_function_call *fun_append)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_append);

        return NULL;

        // node_function_call* node = calloc(1, sizeof(node_function_call));
        // node->function_call = (node_function_call*) fun_list;
        // node->function_append = (node_function_call*) fun_append;

        // return node;
}

node_function_call *grammar_function_call(const char *fun_id,
                                          const node_list *args)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_id, args);

        return NULL;

        // node_function_call* node = calloc(1, sizeof(node_function_call));
        // node->id = (char*) fun_id;
        // node->node_list = (node_list*) args;

        // return node;
}

node_function_call *grammar_function_call_from_id(const char *id,
                                                  const node_function_call *fun)
{
        LogDebug("%s(%p, %p)\n", __func__, id, fun);

        return NULL;
        // node_function_call* node = calloc(1, sizeof(node_function_call));
        // node->id = (char*) id;
        // node->function_call = (node_function_call*) fun;

        // return node;
}

node_expression *grammar_new_declaration_file_node(const char *fpath,
                                                   const char *id,
                                                   const node_list *separators)
{
        LogDebug("%s(%s, %p, %p)", __func__, fpath, id, separators);

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->type = EXPRESSION_FILE_DECLARATION;
        node->list_expr = (node_list *)separators; // Could be NULL

        node->var = (variable *)calloc(1, sizeof(variable));
        if (node->var == NULL) {
                error_no_memory();
                exit(1);
        }

        node->var->type = FILE_PATH_TYPE;
        node->var->name = strdup(id);

        if (insert_variable(node->var) < SUCCESS) {
                error_multiple_declaration(id);
                exit(1);
        }

        node->var->value.string = strdup(fpath);

        return node;
}

node_expression *
grammar_new_declaration_stdout_node(const char *id, const node_list *separators)
{
        LogDebug("%s(%p, %p)\n", __func__, id, separators);

        return grammar_new_declaration_file_node("STDOUT", id, separators);
}

node_file_block *grammar_using_file(const char *id,
                                    const node_expression_list *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        return NULL;
        // node_file_block* node_file_block = calloc(1, sizeof(node_file_block));
        // // node_file_block->id = calloc(1,sizeof(node_file_block));
        // // node_file_block->expr = calloc(1,sizeof(node_file_block));
        // node_file_block->id = (node_expression*) id;
        // node_file_block->expr = (node_expression*) expr;

        // return NULL;
}

node_expression *grammar_new_variable(const char *id,
                                      const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        variable *id_var = (variable *)calloc(1, sizeof(variable));
        if (id_var == NULL) {
                error_no_memory();
                exit(1);
        }

        id_var->name = strdup(id);
        id_var->type = EXPRESSION_TYPE;

        id_var->value.expr = (node_expression *)expr;

        if (insert_variable(id_var) < SUCCESS) {
                error_multiple_declaration(id);
                exit(1);
        }

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

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

        return NULL;
        // node_expression* expr = malloc(sizeof(node_expression));
        // expr->type = EXPRESSION_CONDITIONAL;

        // conditional_node* node = calloc(1, sizeof(conditional_node));
        // node->condition = calloc(1, sizeof(node_expression));
        // node->condition = (node_expression*) condition;
        // node->expr_true = calloc(1, sizeof(node_expression));
        // node->expr_true = (node_expression*) expr_true;
        // node->expr_false = calloc(1, sizeof(node_expression));
        // node->expr_false = (node_expression*) expr_false;

        // expr->conditional = node;

        // return expr;
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

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

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

        // node_list* lst = (node_list*) malloc(sizeof(node_list));

        // if (expr == NULL) {
        //         lst->exprs = NULL;
        //         lst->len = 0;
        //         lst->type = LIST_BLANK_TYPE;
        // } else {
        //         lst->len = 1;
        //         lst->type = LIST_EXPRESSION_TYPE;

        //         lst->exprs = (node_expression**) malloc(sizeof(node_expression*));
        //         lst->exprs[0] = (node_expression*) expr;
        // }

        // return lst;
        /*
        Devuelve l->{elem (expr), next-> NULL}
        */
}

node_list *grammar_concat_list_expr(node_list *list,
                                    const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        return NULL;
        // node_expression** expanded = (node_expression**) realloc(list->exprs, (list->len + 1) * sizeof(node_expression*));
        // expanded[list->len] = (node_expression*) expr;

        // list->exprs[(list->len)++] = (node_expression*) expanded;

        // return list;
}

node_list *grammar_concat_list_list(node_list *head_list,
                                    const node_list *tail_list)
{
        LogDebug("%s(%p, %p)\n", __func__, head_list, tail_list);

        return NULL;

        // node_list* concat = realloc(head_list, head_list->len + tail_list->len);

        // for (size_t i = 0, last_head = head_list->len; i < tail_list->len; i++) {
        //         concat->exprs[last_head + i] = tail_list->exprs[i];
        // }

        // head_list->len += tail_list->len;
        // head_list = concat;
        // return concat;
}

node_list *grammar_new_list_from_range(const char *start, const char *end)
{
        LogDebug("%s(%p, %p)\n", __func__, start, end);

        if (start == NULL || end == NULL) {
                return NULL;
        }

        long range_start = atol(start);
        long range_end = atol(end);

        // atol retuns 0 on error. Manually check that start is a number
        if ((range_start == 0 && isdigit(start[0]) == 0) ||
            (range_end == 0 && isdigit(end[0]) == 0)) {
                error_invalid_range(start, end);
                exit(1);
        }

        /* [From, To]
         * [5, 1] : Invalid
         * [-1, -8] : Invalid
         * */
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

        // node_list* lst = (node_list*) malloc(sizeof(node_list));

        // if (expr == NULL) {
        //         lst->exprs = NULL;
        //         lst->type = BLANK_TYPE;
        //         lst->len = 0;
        // } else {
        //         lst->exprs = (node_expression**) malloc(sizeof(node_expression*));

        //         lst->exprs[0] = (node_expression*) expr;
        //         lst->len = 1;
        //         lst->type = EXPRESSION_TYPE;
        // }
        // return lst;
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
        list->type = EXPRESSION_TYPE;

        return list;
}

node_expression *
grammar_expression_arithmetic_num_add(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->var->type = NUMBER_ARITHMETIC_ADD;
        // char* name = "NUMBER_ARITHMETIC_ADD";
        // node->var->name = calloc(1, strlen(name));
        // strcpy(node->var->name, name);
        // node->var->value.number = lvalue->var->value.number + rvalue->var->value.number;

        // return (node_expression*) node;
}

node_expression *
grammar_expression_arithmetic_num_sub(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));

        // node->type = EXPRESSION_VARIABLE;

        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->var->type = NUMBER_ARITHMETIC_SUB;

        // char* name = "NUMBER_ARITHMETIC_SUB";
        // node->var->name = calloc(1, strlen(name));
        // strcpy(node->var->name, name);

        // node->var->value.number = lvalue->var->value.number - rvalue->var->value.number;

        // return (node_expression*) node;
}

node_expression *
grammar_expression_arithmetic_num_mul(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));

        // node->type = EXPRESSION_VARIABLE;

        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->var->type = NUMBER_ARITHMETIC_MUL;

        // char* name = "NUMBER_ARITHMETIC_MUL";
        // node->var->name = calloc(1, strlen(name));
        // strcpy(node->var->name, name);

        // node->var->value.number = lvalue->var->value.number * rvalue->var->value.number;

        // return node;
}

node_expression *
grammar_expression_arithmetic_num_div(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));

        // node->type = EXPRESSION_VARIABLE;
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->var->type = NUMBER_ARITHMETIC_DIV;

        // char* name = "NUMBER_ARITHMETIC_DIV";
        // node->var->name = calloc(1, strlen(name));
        // strcpy(node->var->name, name);
        // // strcpy(node->var->name, "NUMBER_ARITHMETIC_DIV");

        // node->var->value.number = lvalue->var->value.number / rvalue->var->value.number;

        // return node;
}

node_expression *
grammar_expression_arithmetic_num_mod(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));

        // node->type = EXPRESSION_VARIABLE;
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->var->type = NUMBER_ARITHMETIC_MOD;

        // char* name = "NUMBER_ARITHMETIC_MOD";
        // node->var->name = calloc(1, strlen(name));
        // strcpy(node->var->name, name);
        // // strcpy(node->var->name, "NUMBER_ARITHMETIC_MOD");

        // node->var->value.number = (double) (((long long) lvalue->var->value.number) % ((long long) rvalue->var->value.number));

        // return node;
}

node_expression *
grammar_expression_arithmetic_str_add(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // const size_t a = strlen(lvalue->var->value.string);
        // const size_t b = strlen(rvalue->var->value.string);
        // const size_t size_ab = a + b + 1;

        // node_expression* concat = (node_expression*) malloc(sizeof(char*) * size_ab);

        // memcpy(concat, lvalue->var->value.string, a);
        // memcpy(concat + a, rvalue->var->value.string, b + 1);

        // concat->var->type = ARITHMETIC_ADD;

        // return (node_expression*) concat;
}

node_expression *
grammar_expression_arithmetic_str_sub(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // char* p, * q, * r;
        // char* str = lvalue->var->value.string;
        // char* sub = rvalue->var->value.string;
        // if (*sub && (q = r = strstr(str, sub)) != NULL) {
        //         size_t len = strlen(sub);
        //         while ((r = strstr(p = r + len, sub)) != NULL) {
        //                 memmove(q, p, r - p);
        //                 q += r - p;
        //         }
        //         memmove(q, p, strlen(p) + 1);
        // }

        // lvalue->var->type = ARITHMETIC_SUB;

        // return (node_expression*) lvalue;
}

node_expression *grammar_expression_bool_and(const node_expression *lvalue,
                                             const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->g_boolean_type = BOOL_TYPE_AND;
        // node->var = (variable*) calloc(1, sizeof(variable));

        // node->var->value.boolean = (lvalue->var->value.boolean && rvalue->var->value.boolean) ? true : false;

        // node->var->type = node->g_boolean_type;

        // return node;
}

node_expression *grammar_expression_bool_or(const node_expression *lvalue,
                                            const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->g_boolean_type = BOOL_TYPE_OR;
        // node->var = (variable*) calloc(1, sizeof(variable));

        // node->var->value.boolean = (lvalue->var->value.boolean || rvalue->var->value.boolean) ? true : false;

        // node->var->type = node->g_boolean_type;

        // return node;
}

node_expression *grammar_expression_bool_not(const node_expression *value)
{
        LogDebug("%s(%p)\n", __func__, value);

        return NULL;

        // node_expression* node = calloc(1, sizeof(node_expression));
        // node->g_boolean_type = BOOL_TYPE_NOT;
        // node->var = (variable*) calloc(1, sizeof(variable));

        // node->var->value.boolean = !value->var->value.boolean;

        // node->var->type = node->g_boolean_type;

        // return node;
}

node_expression *grammar_expression_cmp_equals(const node_expression *lvalue,
                                               const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->g_expression_cmp_type = CMP_EXP_EQUALS;
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->var->type = node->g_expression_cmp_type;
        // switch (node->var->type) {
        // case TYPE_NUMBER:
        //         if (lvalue->var->value.number == rvalue->var->value.number) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // case TYPE_STRING:
        //         if (strcmp(lvalue->var->value.string, rvalue->var->value.string) == 0) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // default:
        //         break;
        // }

        // return node;
}

node_expression *
grammar_expression_cmp_not_equals(const node_expression *lvalue,
                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->g_expression_cmp_type = CMP_EXP_NOT_EQUALS;
        // node->var->type = node->g_expression_cmp_type;

        // switch (node->var->type) {
        // case TYPE_NUMBER:
        //         if (lvalue->var->value.number != rvalue->var->value.number) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // case TYPE_STRING:
        //         if (!strcmp(lvalue->var->value.string, rvalue->var->value.string) == 0) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // default:
        //         break;
        // }

        // return node;
}

node_expression *
grammar_expression_cmp_greater_than(const node_expression *lvalue,
                                    const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->g_expression_cmp_type = CMP_EXP_GREATER_THAN;
        // node->var->type = node->g_expression_cmp_type;
        // switch (node->var->type) {
        // case TYPE_NUMBER:
        //         if (lvalue->var->value.number > rvalue->var->value.number) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // case TYPE_STRING:
        //         if (strcmp(lvalue->var->value.string, rvalue->var->value.string) > 0) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // default:
        //         break;
        // }
        // return node;
}

node_expression *
grammar_expression_cmp_greater_equal(const node_expression *lvalue,
                                     const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->g_expression_cmp_type = CMP_EXP_GREATER_EQUAL;
        // node->var->type = node->g_expression_cmp_type;
        // switch (node->var->type) {
        // case TYPE_NUMBER:
        //         if (lvalue->var->value.number >= rvalue->var->value.number) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // case TYPE_STRING:
        //         if (strcmp(lvalue->var->value.string, rvalue->var->value.string) >= 0) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // default:
        //         break;
        // }
        // return node;
}

node_expression *grammar_expression_cmp_less_than(const node_expression *lvalue,
                                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->g_expression_cmp_type = CMP_EXP_LESS_THAN;
        // node->var->type = node->g_expression_cmp_type;

        // return node;
}

node_expression *
grammar_expression_cmp_less_equal(const node_expression *lvalue,
                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->var = (variable*) calloc(1, sizeof(variable_value));
        // node->g_expression_cmp_type = CMP_EXP_LESS_EQUAL;
        // node->var->type = node->g_expression_cmp_type;
        // switch (node->var->type) {
        // case TYPE_NUMBER:
        //         if (lvalue->var->value.number <= rvalue->var->value.number) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // case TYPE_STRING:
        //         if (strcmp(lvalue->var->value.string, rvalue->var->value.string) <= 0) {
        //                 node->var->value.boolean = true;
        //         } else {
        //                 node->var->value.boolean = false;
        //         }
        //         break;
        // default:
        //         break;
        // }

        // return node;
}

// Chequear si recibe id o expr con el grammar (no coincide)
node_expression *grammar_expression_cmp_by_type(const char *id,
                                                const token_t type)
{
        LogDebug("%s(%p, %d)\n", __func__, id, type);

        return NULL;

        // node_expression* node = calloc(1, sizeof(node_expression));
        // node->var = calloc(1, sizeof(variable_value));
        // node->g_expression_cmp_type = CMP_EXP_IS;
        // node->var->type = node->g_expression_cmp_type;
        // node->var->value.boolean = (expr->var->type == type) ? true : false;

        // return node;
}

node_expression *
grammar_expression_from_funcall(const node_function_call *fn_calls)
{
        LogDebug("%s(%p)\n", __func__, fn_calls);

        return NULL;

        // node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        // node->var = (variable*) calloc(1, sizeof(variable));
        // node->fun_call = (node_function_call*) fn_calls;

        // return node;
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

        // node_expression* node = calloc(1, sizeof(node_expression));
        // if (id != NULL) {
        //         node->var = calloc(1, sizeof(variable));
        //         node->var->name = calloc(1, strlen(id->var->name) + 1);
        //         strcpy(node->var->name, id->var->name);
        //         node->var->type = id->var->type;
        //         node->var->value = id->var->value;

        //         node->type = RETURN__ID;
        // } else {
        //         node->var = calloc(1, sizeof(variable));
        //         char* name = "RETURN_NO_ID";
        //         node->var->name = calloc(1, strlen(name) + 1);
        //         strcpy(node->var->name, name);
        //         node->type = RETURN_NO_ID;
        // }

        // return node;
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

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->type = EXPRESSION_VARIABLE_ASSIGNMENT;

        node->var = var;
        node->expr = (node_expression *)expr;

        return node;
}

node_expression *grammar_new_assignment_from_id(const char *from,
                                                const char *to)
{
        LogDebug("%s(%s, %s)\n", __func__, from, to);

        variable *var = lookup_variable(from);
        if (var == NULL) {
                // Store as dangling variable
                var = (variable *)calloc(1, sizeof(variable));
                if (var == NULL) {
                        error_no_memory();
                        exit(1);
                }

                var->type = UNKNOWN_TYPE;
                var->name = strdup(from);

                if (insert_dangling_variable(var) < SUCCESS) {
                        error_multiple_declaration(from);
                        exit(1);
                }
        }

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->type = VARIABLE_TYPE;

        node->var = var;

        return grammar_new_assignment_expression(node, to);
}

node_expression *grammar_expression_type_from_id(const char *id)
{
        variable *var = lookup_variable(id);
        if (var == NULL) {
                error_variable_not_found(id);
                exit(1);
        }

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->type = VARIABLE_TYPE;

        node->var = var;

        return node;
}

node_expression *grammar_constant_bool(const char *rval)
{
        LogDebug("%s(%s)\n", __func__, rval);

        // printf("\n\nrval en grammar_constant_bool: %s\n\n", rval);

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->type = EXPRESSION_GRAMMAR_CONSTANT_TYPE;

        node->var = (variable *)calloc(1, sizeof(variable));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->var->type = BOOL_TYPE;
        if (strcmp(rval, "True") == 0) {
                node->var->value.boolean = true;
        } else {
                node->var->value.boolean = false;
        }

        return node;

        // node_expression* newNode = calloc(1, sizeof(node_expression));
        // newNode->var = calloc(1, sizeof(variable));
        // newNode->var->type = BOOL_TYPE;

        // if (strcmp(rval, "True") == 0) {
        //         newNode->var->value.boolean = true;
        // } else {
        //         newNode->var->value.boolean = false;
        // }
        // char* name = "BOOL_TYPE";
        // newNode->var->name = calloc(1, strlen(name) + 1);
        // strcpy(newNode->var->name, name);

        // newNode->type = BOOL_TYPE;

        // return newNode;
}

node_expression *grammar_constant_number(const char *rval)
{
        // printf("\nEntro a grammar\n");

        LogDebug("%s(%s)\n", __func__, rval);

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->type = EXPRESSION_GRAMMAR_CONSTANT_TYPE;

        node->var = (variable *)calloc(1, sizeof(variable));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->var->type = NUMBER_TYPE;
        node->var->value.number = atof(rval);

        return node;

        // node->var = calloc(1, sizeof(variable*));
        // // double rvalBis = strtod(*rval, NULL);
        // double rvalBis = atof(rval);
        // int rvalBisBis = atoi(rval);
        // printf("\nf: %f", rvalBis);
        // printf("\nd: %d", rvalBisBis);
        // printf("\nrval: %s", rval);
        // node->var->value.number = rvalBis;
        // node->var->type = NUMBER_TYPE;

        // char* name = "NUMBER_TYPE";
        // node->var->name = calloc(1, strlen(name) + 1);
        // strcpy(node->var->name, name);

        // node->type = NUMBER_TYPE;
        // printf("\nSalio a grammar\n\n");
        // return node;
}

node_expression *grammar_constant_string(const char *rval)
{
        LogDebug("%s(%s)\n", __func__, rval);

        node_expression *node =
                (node_expression *)calloc(1, sizeof(node_expression));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->type = EXPRESSION_GRAMMAR_CONSTANT_TYPE;

        node->var = (variable *)calloc(1, sizeof(variable));
        if (node == NULL) {
                error_no_memory();
                exit(1);
        }

        node->var->type = STRING_TYPE;
        strcpy(node->var->value.string, rval);

        return node;

        // node_expression* newNode = calloc(1, sizeof(node_expression));
        // newNode->var = calloc(1, sizeof(variable));
        // newNode->var->type = STRING_TYPE;

        // char* name = "STRING_TYPE";
        // newNode->var->name = calloc(1, strlen(name) + 1);
        // strcpy(newNode->var->name, name);

        // newNode->var->value.string = (char*) malloc(strlen(rval) + 1);
        // strcpy(newNode->var->value.string, rval);

        // newNode->type = STRING_TYPE;

        // return newNode;
}
