#include <stdbool.h> /* bool type */
#include <stdlib.h> /* malloc, calloc */
#include <string.h> /* strlen, strcpy */

/* Backend */
#include "../../backend/logger.h" /* LogDebug */

/* Frontend */
#include "../lexical-analysis/flex-actions.h" /* enum TokenID; free_yylval */

/* This file */
#include "bison-actions.h"

#include "../../backend/symbols.h"

/* Prototypes */
// static bool const_to_expr_cp(const char* value,
//                              node_expression* expression);

/**
 * Implementación de "bison-grammar.h".
 */

void yyerror(const char* string)
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

program* grammar_program(const node_function* value)
{
        LogDebug("%s(%p)\n", __func__, value);
        program* p = (program*) calloc(1, sizeof(program));
        p->value = (node_function*) value;
        state.succeed = true;

        return p;
}

node_function*
grammar_new_function(const node_expression* expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_function* newFuncNode = calloc(1, sizeof(node_function));
        newFuncNode->expr = calloc(1, sizeof(node_expression));
        newFuncNode->expr = (node_expression*) expr;

        state.succeed = true;

        return newFuncNode;
}

node_expression* grammar_expression_from_list(const node_list* list)
{
        LogDebug("%s(%p)\n", __func__, list);

        node_expression* expr = (node_expression*) malloc(sizeof(node_expression));

        expr->listExpr = (node_list*) list;
        expr->type = EXPRESSION_LIST;

        return expr;
}

node_expression*
grammar_expression_from_filehandler(const node_file_block* fhandler)
{
        LogDebug("%s(%p)\n", __func__, fhandler);

        node_expression* fileExpr = calloc(1, sizeof(node_expression));
        fileExpr->fileHandler = (node_file_block*) fhandler;

        return fileExpr;
}

node_expression*
grammar_expression_from_assignment(const node_expression* assignment)
{
        LogDebug("%s(%p)\n", __func__, assignment);


        return (node_expression*) assignment;
}

node_function_call*
grammar_concat_function_call(const node_function_call* fun_list,
                             const node_function_call* fun_append)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_append);

        node_function_call* node = calloc(1, sizeof(node_function_call));
        node->function_call = (node_function_call*) fun_list;
        node->function_append = (node_function_call*) fun_append;

        return node;
}

node_function_call*
grammar_function_call(const node_expression* fun_id,
                      const node_list* args)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_id, args);

        node_function_call* node = calloc(1, sizeof(node_function_call));
        node->id = (node_expression*) fun_id;
        node->node_list = (node_list*) args;

        return node;
}

node_function_call*
grammar_function_call_from_id(const node_expression* id,
                              const node_function_call* fun)
{
        LogDebug("%s(%p, %p)\n", __func__, id, fun);

        node_function_call* node = calloc(1, sizeof(node_function_call));
        node->id = (node_expression*) id;
        node->function_call = (node_function_call*) fun;

        return node;
}

node_expression*
grammar_new_declaration_file_node(const variable* fpath,
                                  const node_expression* id,
                                  const node_list* separators)
{
        LogDebug("%s(%p, %p, %p)", __func__, fpath, id, separators);

        node_expression* node = calloc(1, sizeof(node_expression));
        if (separators == NULL) {
                node->f_declaration_type = FILE_STRING_TYPE;
        } else {
                node->f_declaration_type = FILE_FSTREAM_STDOUT_TYPE;
        }

        node->listExpr = (node_list*) separators;
        node->var = (variable*) fpath;

        return node;
}

node_expression*
grammar_new_declaration_stdout_node(const node_expression* id,
                                    const node_list* separators)
{
        LogDebug("%s(%p, %p)\n", __func__, id, separators);

        node_expression* node = calloc(1, sizeof(node_expression));
        if (separators == NULL) {
                node->f_declaration_type = STDOUT_STRING_TYPE;
        } else {
                node->f_declaration_type = STDOUT_FSTREAM_STDOUT_TYPE;
        }
        node->listExpr = (node_list*) separators;

        return node;
}

node_file_block*
grammar_using_file(const node_expression* id,
                   const node_expression* expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        node_file_block* node_file_block = calloc(1, sizeof(node_file_block));
        // node_file_block->id = calloc(1,sizeof(node_file_block));
        // node_file_block->expr = calloc(1,sizeof(node_file_block));
        node_file_block->id = (node_expression*) id;
        node_file_block->expr = (node_expression*) expr;

        return NULL;
}

node_expression*
grammar_new_variable(const node_expression* id,
                     const node_expression* expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        node_expression* node = calloc(1, sizeof(node_expression));
        node->var = calloc(1, sizeof(variable));
        node->var = expr->var;
        node->var->type = id->var->type;

        switch (expr->type) {
        case BOOL_TYPE:
                node->var->value.boolean = expr->var->value.boolean;
                break;
        case NUMBER_TYPE:
                node->var->value.number = expr->var->value.number;
                break;
        case STRING_TYPE:
                strcpy(node->var->value.string, expr->var->value.string);
                break;
        default:
                break;
        }

        node->type = EXPRESSION_VARIABLE;

        return node;
}

node_expression*
grammar_new_conditional(const node_expression* condition,
                        const node_expression* expr_true,
                        const node_expression* expr_false)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, condition, expr_true, expr_false);

        node_expression* expr = malloc(sizeof(node_expression));
        expr->type = EXPRESSION_CONDITIONAL;

        conditional_node* node = calloc(1, sizeof(conditional_node));
        node->condition = calloc(1, sizeof(node_expression));
        node->condition = (node_expression*) condition;
        node->expr_true = calloc(1, sizeof(node_expression));
        node->expr_true = (node_expression*) expr_true;
        node->expr_false = calloc(1, sizeof(node_expression));
        node->expr_false = (node_expression*) expr_false;

        expr->conditional = node;

        return expr;
}

node_expression*
grammar_new_loop(const node_expression* id,
                 const node_expression* iterable,
                 const node_expression* action)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, id, iterable, action);

        node_expression* expr = malloc(sizeof(node_expression));
        expr->type = EXPRESSION_LOOP;

        loop_node* loop = calloc(1, sizeof(loop_node));
        loop->id = calloc(1, sizeof(node_expression));
        loop->iterable = calloc(1, sizeof(node_expression));
        loop->action = calloc(1, sizeof(node_expression));

        expr->loop = loop;

        return expr;
}

node_list*
grammar_new_list(const node_expression* expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_list* lst = (node_list*) malloc(sizeof(node_list));

        if (expr == NULL) {
                lst->exprs = NULL;
                lst->len = 0;
                lst->type = LIST_BLANK_TYPE;
        } else {
                lst->len = 1;
                lst->type = LIST_EXPRESSION_TYPE;

                lst->exprs = (node_expression**) malloc(sizeof(node_expression*));
                lst->exprs[0] = (node_expression*) expr;
        }

        return lst;
        /*
        Devuelve l->{elem (expr), next-> NULL}
        */
}

node_list*
grammar_concat_list_expr(node_list* list,
                         const node_expression* expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        node_expression** expanded = (node_expression**) realloc(list->exprs, (list->len + 1) * sizeof(node_expression*));
        expanded[list->len] = (node_expression*) expr;

        list->exprs[(list->len)++] = (node_expression*) expanded;

        return list;

}

node_list*
grammar_concat_list_list(node_list* head_list,
                         const node_list* tail_list)
{
        LogDebug("%s(%p, %p)\n", __func__, head_list, tail_list);




        node_list* concat = realloc(head_list, head_list->len + tail_list->len);

        for (size_t i = 0, last_head = head_list->len; i < tail_list->len; i++) {
                concat->exprs[last_head + i] = tail_list->exprs[i];
        }

        head_list->len += tail_list->len;
        head_list = concat;
        return concat;
}

node_list*
grammar_new_list_from_range(const variable* start,
                            const variable* end)
{
        LogDebug("%s(%p, %p)\n", __func__, start, end);

        if (start->type != TYPE_NUMBER || end->type != TYPE_NUMBER) {
                LogError("Invalid type for range.\n");
                return NULL;
        }

        const long long vstart = start->value.number;
        const long long vend = end->value.number;
        const long long diff = (vend > vstart) ? vend - vstart : vstart - vend;

        node_list* lst = (node_list*) malloc(sizeof(node_list));
        lst->exprs = (node_expression**) malloc(diff * sizeof(node_expression*));

        for (long long i = 0; i < diff; i++) {
                node_expression* expr = (node_expression*) malloc(sizeof(node_expression));
                variable* var = (variable*) malloc(sizeof(variable));

                var->type = TYPE_NUMBER;
                var->value.number = i;

                expr->var = var;
                expr->type = NUMBER_TYPE;

                lst->exprs[i] = expr;
        }

        lst->type = LIST_RANGE_TYPE;
        lst->len = diff;

        return lst;
}

node_list*
grammar_new_list_args(const node_expression* expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_list* lst = (node_list*) malloc(sizeof(node_list));

        if (expr == NULL) {
                lst->exprs = NULL;
                lst->type = BLANK_TYPE;
                lst->len = 0;
        } else {
                lst->exprs = (node_expression**) malloc(sizeof(node_expression*));

                lst->exprs[0] = (node_expression*) expr;
                lst->len = 1;
                lst->type = EXPRESSION_TYPE;
        }
        return lst;
}

node_list*
grammar_concat_list_args(node_list* list,
                         const node_expression* expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        if (expr == NULL)
                return list;

        list->exprs = (node_expression**) realloc(list->exprs, (1 + list->len) * sizeof(node_expression*));

        list->exprs[list->len] = (node_expression*) expr;
        list->len += 1;
        list->type = EXPRESSION_TYPE;

        return list;
}

node_expression*
grammar_expression_arithmetic_num_add(const node_expression* lvalue,
                                      const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->var = (variable*) calloc(1, sizeof(variable));
        node->var->type = NUMBER_ARITHMETIC_ADD;
        char* name = "NUMBER_ARITHMETIC_ADD";
        node->var->name = calloc(1, strlen(name));
        strcpy(node->var->name, name);
        node->var->value.number = lvalue->var->value.number + rvalue->var->value.number;

        return (node_expression*) node;
}

node_expression*
grammar_expression_arithmetic_num_sub(const node_expression* lvalue,
                                      const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));

        node->type = EXPRESSION_VARIABLE;

        node->var = (variable*) calloc(1, sizeof(variable));
        node->var->type = NUMBER_ARITHMETIC_SUB;

        char* name = "NUMBER_ARITHMETIC_SUB";
        node->var->name = calloc(1, strlen(name));
        strcpy(node->var->name, name);

        node->var->value.number = lvalue->var->value.number - rvalue->var->value.number;

        return (node_expression*) node;
}

node_expression*
grammar_expression_arithmetic_num_mul(const node_expression* lvalue,
                                      const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));

        node->type = EXPRESSION_VARIABLE;

        node->var = (variable*) calloc(1, sizeof(variable));
        node->var->type = NUMBER_ARITHMETIC_MUL;

        char* name = "NUMBER_ARITHMETIC_MUL";
        node->var->name = calloc(1, strlen(name));
        strcpy(node->var->name, name);

        node->var->value.number = lvalue->var->value.number * rvalue->var->value.number;

        return node;
}

node_expression*
grammar_expression_arithmetic_num_div(const node_expression* lvalue,
                                      const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));

        node->type = EXPRESSION_VARIABLE;
        node->var = (variable*) calloc(1, sizeof(variable));
        node->var->type = NUMBER_ARITHMETIC_DIV;

        char* name = "NUMBER_ARITHMETIC_DIV";
        node->var->name = calloc(1, strlen(name));
        strcpy(node->var->name, name);
        // strcpy(node->var->name, "NUMBER_ARITHMETIC_DIV");

        node->var->value.number = lvalue->var->value.number / rvalue->var->value.number;

        return node;
}

node_expression*
grammar_expression_arithmetic_num_mod(const node_expression* lvalue,
                                      const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));

        node->type = EXPRESSION_VARIABLE;
        node->var = (variable*) calloc(1, sizeof(variable));
        node->var->type = NUMBER_ARITHMETIC_MOD;

        char* name = "NUMBER_ARITHMETIC_MOD";
        node->var->name = calloc(1, strlen(name));
        strcpy(node->var->name, name);
        // strcpy(node->var->name, "NUMBER_ARITHMETIC_MOD");

        node->var->value.number = (double) (((long long) lvalue->var->value.number) % ((long long) rvalue->var->value.number));

        return node;
}

node_expression*
grammar_expression_arithmetic_str_add(const node_expression* lvalue,
                                      const node_expression* rvalue)
{
        const size_t a = strlen(lvalue->var->value.string);
        const size_t b = strlen(rvalue->var->value.string);
        const size_t size_ab = a + b + 1;

        node_expression* concat = (node_expression*) malloc(sizeof(char*) * size_ab);

        memcpy(concat, lvalue->var->value.string, a);
        memcpy(concat + a, rvalue->var->value.string, b + 1);

        concat->var->type = ARITHMETIC_ADD;

        return (node_expression*) concat;
}

node_expression*
grammar_expression_arithmetic_str_sub(const node_expression* lvalue,
                                      const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        char* p, * q, * r;
        char* str = lvalue->var->value.string;
        char* sub = rvalue->var->value.string;
        if (*sub && (q = r = strstr(str, sub)) != NULL) {
                size_t len = strlen(sub);
                while ((r = strstr(p = r + len, sub)) != NULL) {
                        memmove(q, p, r - p);
                        q += r - p;
                }
                memmove(q, p, strlen(p) + 1);
        }

        lvalue->var->type = ARITHMETIC_SUB;

        return (node_expression*) lvalue;
}

node_expression*
grammar_expression_bool_and(const node_expression* lvalue,
                            const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->g_boolean_type = BOOL_TYPE_AND;
        node->var = (variable*) calloc(1, sizeof(variable));

        node->var->value.boolean = (lvalue->var->value.boolean && rvalue->var->value.boolean) ? true : false;

        node->var->type = node->g_boolean_type;

        return node;
}

node_expression*
grammar_expression_bool_or(const node_expression* lvalue,
                           const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->g_boolean_type = BOOL_TYPE_OR;
        node->var = (variable*) calloc(1, sizeof(variable));

        node->var->value.boolean = (lvalue->var->value.boolean || rvalue->var->value.boolean) ? true : false;

        node->var->type = node->g_boolean_type;

        return node;
}

node_expression*
grammar_expression_bool_not(const node_expression* value)
{
        LogDebug("%s(%p)\n", __func__, value);

        node_expression* node = calloc(1, sizeof(node_expression));
        node->g_boolean_type = BOOL_TYPE_NOT;
        node->var = (variable*) calloc(1, sizeof(variable));

        node->var->value.boolean = !value->var->value.boolean;

        node->var->type = node->g_boolean_type;

        return node;

}

node_expression*
grammar_expression_cmp_equals(const node_expression* lvalue,
                              const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);


        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->g_expression_cmp_type = CMP_EXP_EQUALS;
        node->var = (variable*) calloc(1, sizeof(variable));
        node->var->type = node->g_expression_cmp_type;
        switch (node->var->type) {
        case TYPE_NUMBER:
                if (lvalue->var->value.number == rvalue->var->value.number) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        case TYPE_STRING:
                if (strcmp(lvalue->var->value.string, rvalue->var->value.string) == 0) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        default:
                break;
        }

        return node;
}

node_expression*
grammar_expression_cmp_not_equals(const node_expression* lvalue,
                                  const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);


        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->var = (variable*) calloc(1, sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_NOT_EQUALS;
        node->var->type = node->g_expression_cmp_type;

        switch (node->var->type) {
        case TYPE_NUMBER:
                if (lvalue->var->value.number != rvalue->var->value.number) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        case TYPE_STRING:
                if (!strcmp(lvalue->var->value.string, rvalue->var->value.string) == 0) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        default:
                break;
        }

        return node;
}

node_expression*
grammar_expression_cmp_greater_than(const node_expression* lvalue,
                                    const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->var = (variable*) calloc(1, sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_GREATER_THAN;
        node->var->type = node->g_expression_cmp_type;
        switch (node->var->type) {
        case TYPE_NUMBER:
                if (lvalue->var->value.number > rvalue->var->value.number) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        case TYPE_STRING:
                if (strcmp(lvalue->var->value.string, rvalue->var->value.string) > 0) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        default:
                break;
        }
        return node;
}

node_expression*
grammar_expression_cmp_greater_equal(const node_expression* lvalue,
                                     const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->var = (variable*) calloc(1, sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_GREATER_EQUAL;
        node->var->type = node->g_expression_cmp_type;
        switch (node->var->type) {
        case TYPE_NUMBER:
                if (lvalue->var->value.number >= rvalue->var->value.number) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        case TYPE_STRING:
                if (strcmp(lvalue->var->value.string, rvalue->var->value.string) >= 0) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        default:
                break;
        }
        return node;
}

node_expression*
grammar_expression_cmp_less_than(const node_expression* lvalue,
                                 const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->var = (variable*) calloc(1, sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_LESS_THAN;
        node->var->type = node->g_expression_cmp_type;

        return node;
}

node_expression*
grammar_expression_cmp_less_equal(const node_expression* lvalue,
                                  const node_expression* rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->var = (variable*) calloc(1, sizeof(variable_value));
        node->g_expression_cmp_type = CMP_EXP_LESS_EQUAL;
        node->var->type = node->g_expression_cmp_type;
        switch (node->var->type) {
        case TYPE_NUMBER:
                if (lvalue->var->value.number <= rvalue->var->value.number) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        case TYPE_STRING:
                if (strcmp(lvalue->var->value.string, rvalue->var->value.string) <= 0) {
                        node->var->value.boolean = true;
                } else {
                        node->var->value.boolean = false;
                }
                break;
        default:
                break;
        }

        return node;
}

node_expression*
grammar_expression_cmp_by_type(const node_expression* expr,
                               const token_t type)
{
        LogDebug("%s(%p, %d)\n", __func__, expr, type);
        node_expression* node = calloc(1, sizeof(node_expression));
        node->var = calloc(1, sizeof(variable_value));
        node->g_expression_cmp_type = CMP_EXP_IS;
        node->var->type = node->g_expression_cmp_type;
        node->var->value.boolean = (expr->var->type == type) ? true : false;

        return node;
}

node_expression*
grammar_expression_from_funcall(const node_function_call* fn_calls)
{

        LogDebug("%s(%p)\n", __func__, fn_calls);

        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->var = (variable*) calloc(1, sizeof(variable));
        node->function_call_pointer = (node_function_call*) fn_calls;

        return node;
}

node_expression*
grammar_new_return_node(const node_expression* id)
{
        LogDebug("%s(%p)\n", __func__, id);

        node_expression* node = calloc(1, sizeof(node_expression));
        if (id != NULL) {
                node->var = calloc(1, sizeof(variable));
                node->var->name = calloc(1, strlen(id->var->name) + 1);
                strcpy(node->var->name, id->var->name);
                node->var->type = id->var->type;
                node->var->value = id->var->value;

                node->type = RETURN__ID;
        } else {
                node->var = calloc(1, sizeof(variable));
                char* name = "RETURN_NO_ID";
                node->var->name = calloc(1, strlen(name) + 1);
                strcpy(node->var->name, name);
                node->type = RETURN_NO_ID;
        }

        return node;
}


node_expression*
grammar_new_assignment_expression(const node_expression* expr,
                                  const node_expression* id)
{
        LogDebug("%s(%p, %p)\n", __func__, expr, id);
        node_expression* node = (node_expression*) calloc(1, sizeof(node_expression));
        node->var = expr->var;
        node->type = id->type;
        strcpy(node->var->name, id->var->name);

        return node;
}

node_expression*
grammar_identifier(const char* id)
{
        LogDebug("%s(%p)\n", __func__, id);

        node_expression* node = calloc(1, sizeof(node_expression));
        node->var = calloc(1, sizeof(variable));
        node->var->type = ID_TYPE;
        node->var->name = calloc(1, strlen(id));
        strcpy(node->var->name, id);

        node->type = ID_TYPE;

        return node;
}

node_expression*
grammar_constant_bool(const char* rval)
{
        // LogDebug("%s(%s)\n", __func__, rval);

        printf("\n\nrval en grammar_constant_bool: %s\n\n", rval);

        node_expression* newNode = calloc(1, sizeof(node_expression));
        newNode->var = calloc(1, sizeof(variable));
        newNode->var->type = BOOL_TYPE;

        if (strcmp(rval, "True") == 0) {
                newNode->var->value.boolean = true;
        } else {
                newNode->var->value.boolean = false;
        }
        char* name = "BOOL_TYPE";
        newNode->var->name = calloc(1, strlen(name) + 1);
        strcpy(newNode->var->name, name);

        newNode->type = BOOL_TYPE;

        return newNode;
}

node_expression*
grammar_constant_number(const char* rval)
{
        printf("\nEntro a grammar\n");

        // LogDebug("%s(%s)\n", __func__, rval);

        node_expression* node = calloc(1, sizeof(node_expression));

        node->var = calloc(1, sizeof(variable*));
        // double rvalBis = strtod(*rval, NULL);
        double rvalBis = atof(rval);
        int rvalBisBis = atoi(rval);
        printf("\nf: %f", rvalBis);
        printf("\nd: %d", rvalBisBis);
        printf("\nrval: %s", rval);
        node->var->value.number = rvalBis;
        node->var->type = NUMBER_TYPE;

        char* name = "NUMBER_TYPE";
        node->var->name = calloc(1, strlen(name) + 1);
        strcpy(node->var->name, name);

        node->type = NUMBER_TYPE;
        printf("\nSalio a grammar\n\n");
        return node;
}

node_expression*
grammar_constant_string(const char* rval)
{
        LogDebug("%s(%s)\n", __func__, rval);

        node_expression* newNode = calloc(1, sizeof(node_expression));
        newNode->var = calloc(1, sizeof(variable));
        newNode->var->type = STRING_TYPE;

        char* name = "STRING_TYPE";
        newNode->var->name = calloc(1, strlen(name) + 1);
        strcpy(newNode->var->name, name);

        newNode->var->value.string = (char*) malloc(strlen(rval) + 1);
        strcpy(newNode->var->value.string, rval);

        newNode->type = STRING_TYPE;

        return newNode;
}

/*
void
free_programblock(struct node_block* program)
{
        free_yylval();

        if (program == NULL)
                return;

        free(program->exp);
        free(program);
}

static bool const_to_expr_cp(const char* value, node_expression* expression)
{
        size_t len = strlen(value);

        expression->value = calloc(sizeof(char), len);
        if (expression->value == NULL) {
                log_error_no_mem();
                return false;
        }

        expression->length = len;
        strcpy(expression->value, value);
        return true;
}
*/
