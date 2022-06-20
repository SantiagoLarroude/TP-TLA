#include <stdbool.h> /* bool type */
#include <stdlib.h> /* malloc, calloc */
#include <string.h> /* strlen, strcpy */

/* Backend */
#include "../../backend/logger.h" /* LogDebug */

/* Frontend */
#include "../lexical-analysis/flex-actions.h" /* enum TokenID; free_yylval */

/* This file */
#include "bison-actions.h"

/* Prototypes */
// static bool const_to_expr_cp(const char* value,
//                              node_expression* expression);

/**
 * Implementación de "bison-grammar.h".
 */

void yyerror(const char *string)
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

void grammar_program(const node_function *value)
{
        LogDebug("%s(%p)\n", __func__, value);

        state.succeed = true;
}

node_function *grammar_new_function(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        return NULL;
}

node_expression *grammar_expression_fom_list(const node_list *list)
{
        LogDebug("%s(%p)\n", __func__, list);

        return NULL;
}

node_expression *
grammar_expression_from_funcall(const node_function_call *fun_list)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_list);

        return NULL;
}

node_expression *
grammar_expression_from_filehandler(const node_file_block *fhandler)
{
        LogDebug("%s(%p)\n", __func__, fhandler);

        return NULL;
}

node_expression *
grammar_expression_from_assignment(const node_expression *assignment)
{
        LogDebug("%s(%p)\n", __func__, assignment);

        return NULL;
}

node_function_call *
grammar_concat_function_call(const node_function_call *fun_list,
                             const node_function_call *fun_append)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_list, fun_append);

        return NULL;
}


node_function_call *grammar_function_call(const node_expression *fun_id,
                                          const node_list *args)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_id, args);

        return NULL;
}


node_function_call *grammar_function_call_from_id(const node_expression *id,
                                                  const node_function_call *fun)
{
        LogDebug("%s(%p, %p)\n", __func__, id, fun);

        return NULL;
}

node_expression *grammar_new_declaration_file_node(const variable *fpath,
                                                   const node_expression *id,
                                                   const node_list *separators)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, fpath, id, separators);

        return NULL;
}

node_expression *
grammar_new_declaration_stdout_node(const node_expression *id,
                                    const node_list *separators)
{
        LogDebug("%s(%p, %p)\n", __func__, id, separators);

        return NULL;
}

node_file_block *grammar_using_file(const node_expression *id,
                                    const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        return NULL;
}

node_expression *grammar_new_variable(const node_expression *id,
                                      const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        return NULL;
}

node_expression *grammar_new_conditional(const node_expression *condition,
                                         const node_expression *expr_true,
                                         const node_expression *expr_false)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, condition, expr_true,
                 expr_false);

        return NULL;
}

node_expression *grammar_new_loop(const node_expression *id,
                                  const node_expression *iterable,
                                  const node_expression *action)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, id, iterable, action);

        return NULL;
}

node_list *grammar_new_list(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        return NULL;
}

node_list *grammar_concat_list_expr(const node_list *list,
                                    const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        list->next = expr;
        list->next->next = NULL;

        return list;
}

node_list *grammar_concat_list_list(const node_list *head_list,
                                    const node_list *tail_list)
{
        LogDebug("%s(%p, %p)\n", __func__, head_list, tail_list);

        return grammar_new_list_from_range(head_list, tail_list);
}

node_list *grammar_new_list_from_range(const variable *start,
                                       const variable *end)
{
        LogDebug("%s(%p, %p)\n", __func__, start, end);

        node_list * list = malloc(sizeof(node_list));
        list->head = start;
        list->head->next = end;
        tail_list->next = NULL;

        return list;

        return NULL;
}

node_list *grammar_new_list_args(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_list * list = malloc(sizeof(node_list));
        list->head = expr;
        list->head->next = NULL;

        return list;
}

node_list *grammar_concat_list_args(const node_list *list,
                                    const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        list->head->next = expr;
        list->head->next->next = NULL;

        return list;
}

node_expression *
grammar_expression_arithmetic_num_add(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return lvalue->value + rvalue->value;
}

node_expression *
grammar_expression_arithmetic_num_sub(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return lvalue->value - rvalue->value;
}

node_expression *
grammar_expression_arithmetic_num_mul(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return lvalue->value * rvalue->value;
}

node_expression *
grammar_expression_arithmetic_num_div(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return lvalue->value / rvalue->value;
}

node_expression *
grammar_expression_arithmetic_num_mod(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return lvalue->value % rvalue->value;
}

node_expression *
grammar_expression_arithmetic_str_add(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        const size_t a = strlen(lvalue->value);
        const size_t b = strlen(rvalue->value);
        const size_t size_ab = a + b + 1;

        node_expression * concat = malloc(size_ab);

        memcpy(concat, lvalue->value, a);
        memcpy(concat + a, rvalue->value, b + 1);

        return concat;
}

node_expression *
grammar_expression_arithmetic_str_sub(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        char *p, *q, *r;
        char str = lvalue->value;
        char sub = rvalue->value;
        if (*sub && (q = r = strstr(str, sub)) != NULL) {
                size_t len = strlen(sub);
                while ((r = strstr(p = r + len, sub)) != NULL) {
                        memmove(q, p, r - p);
                        q += r - p;
                }
                memmove(q, p, strlen(p) + 1);
        }
        return lvalue;
}

node_expression *grammar_expression_bool_and(const node_expression *lvalue,
                                             const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return lvalue->value && rvalue->value;
}

node_expression *grammar_expression_bool_or(const node_expression *lvalue,
                                            const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        return lvalue->value || rvalue->value;
}

node_expression *grammar_expression_bool_not(const node_expression *value)
{
        LogDebug("%s(%p)\n", __func__, value);

        return !value->value;
}

node_expression *
grammar_expression_cmp_equals(const node_expression *lvalue,
                                               const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        bool cmp = lvalue->value->type == rvalue->value->type
        if(!cmp){
                cmp = lvalue->value->value == lvalue->value->value;
                if(!cmp){
                        return lvalue->value->name == lvalue->value->name;
                }
        } else {
                return cmp;
        }
}

node_expression *
grammar_expression_cmp_not_equals(const node_expression *lvalue,
                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        if (grammar_expression_cmp_by_type(lvalue->))
        {
                /* code */
        }
        

        node_expression * node = malloc(sizeof(node_expression));
        node->value = malloc(sizeof(variable));
        node->value->value = lvalue->value->value != lvalue->value->value;

        return CMP_EXP_NOT_EQUALS;
}

node_expression *
grammar_expression_cmp_greater_than(const node_expression *lvalue,
                                    const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->value = malloc(sizeof(variable));
        node->value->value = lvalue->value->value > lvalue->value->value;

        return CMP_EXP_GREATER_THAN;
}

node_expression *
grammar_expression_cmp_greater_equal(const node_expression *lvalue,
                                     const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->value = malloc(sizeof(variable));
        node->value->value = lvalue->value->value >= lvalue->value->value;

        return CMP_EXP_GREATER_EQUAL;
}

node_expression *
grammar_expression_cmp_less_than(const node_expression *lvalue,
                                        const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->value = malloc(sizeof(variable));
        node->value->value = lvalue->value->value < lvalue->value->value;

        return CMP_EXP_LESS_THAN;
}

node_expression *
grammar_expression_cmp_less_equal(const node_expression *lvalue,
                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->value = malloc(sizeof(variable));
        node->value->value = lvalue->value->value <= lvalue->value->value;

        return CMP_EXP_LESS_EQUAL;
}

node_expression * grammar_expression_cmp_by_type(const node_expression *expr,
                                                const token_t type)
{
        LogDebug("%s(%p, %d)\n", __func__, expr, type);

        node_expression * node = malloc(sizeof(node_expression));
        node->value = malloc(sizeof(variable));
        node->value->value = expr->value->type == type;

        return CMP_EXP_IS;
}

node_expression *grammar_new_return_node(const char *id)
{
        LogDebug("%s(%p)\n", __func__, id);

        node_expression * node = malloc(sizeof(node_expression));
        if(*id != NULL){
                node->value = malloc(sizeof(variable));
                strcpy(node->value->name, id);
                // node->value->name = id;
        }
        // tendriamos que dejarlo asi?
        // xq se me ocurre que tendria que devolver
        // lo el node_expression que tiene ese id

        return node;
}
// CHEQUEAR
// id de que tipo tiene que ser?
node_expression *
grammar_new_assignment_expression(const node_expression *expr,
                                  const node_expression *id)
{
        LogDebug("%s(%p, %p)\n", __func__, expr, id);

        // node_expression * node = malloc(sizeof(node_expression));
        // variable * value = malloc(sizeof(variable));
        expr->value->type = id->value->type;

        return expr;
}

// CHEQUEAR
// de que valor tiene que ser el parametro de entrada
variable *
grammar_identifier(const char * id)
{
        LogDebug("%s(%p)\n", __func__, id);

        return id;
}

node_expression *
grammar_constant_bool(const variable_value *value)
{
        LogDebug("%s(%s)\n", __func__, value);

        variable * var = malloc(sizeof(variable));
        var->value = value;
        // var->type = BOOL_TYPE;
        strcpy(var->name, "BOOL_TYPE");
        // var->name = NULL;

        node_expression * node = malloc(sizeof(node_expression));
        node->value = var;
        node->g_constant_type = BOOL_TYPE;
        node->g_expression_cmp_type = NULL;
        node->g_return_type = NULL;

        return node;
}

node_expression *
grammar_constant_number(const variable_value *value)
{
        LogDebug("%s(%s)\n", __func__, value);
        
        variable * var = malloc(sizeof(variable));
        var->value = value;
        // var->type = NUMBER_TYPE;
        strcpy(var->name, "NUMBER_TYPE");
        // var->name = NULL;

        node_expression * node = malloc(sizeof(node_expression));
        node->value = var;
        node->g_constant_type = NUMBER_TYPE;
        node->g_expression_cmp_type = NULL;
        node->g_return_type = NULL;

        return node;
}

node_expression *
grammar_constant_string(const variable_value *value)
{
        LogDebug("%s(%s)\n", __func__, value);

        variable * var = malloc(sizeof(variable));
        var->value = value;
        // var->type = STRING_TYPE;
        strcpy(var->name, "STRING_TYPE");
        // var->name = NULL;

        node_expression * node = malloc(sizeof(node_expression));
        node->value = var;
        node->g_constant_type = STRING_TYPE;
        node->g_expression_cmp_type = NULL;
        node->g_return_type = NULL;

        return node;
}

// falta implementar y ubicar en el bison-grammar?

node_expression *
grammar_concat_expressions_filehandler(const node_expression *exprs,
                                       const node_file_block *fhandler)
{
        LogDebug("%s(%p, %p)\n", __func__, exprs, fhandler);

        return NULL;
}



node_expression *
grammar_new_assignment_identifier(const node_expression *from_id,
                                  const node_expression *to_id)
{
        LogDebug("%s(%p, %p)\n", __func__, from_id, to_id);

        return NULL;
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
