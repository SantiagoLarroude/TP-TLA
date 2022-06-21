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

// TERMINARRRRRRRRRRRRR
node_expression *
grammar_expression_from_funcall(const node_function_call *fun_list)
{
        LogDebug("%s(%p, %p)\n", __func__, fun_list);

        node_expression * node = malloc(sizeof(node_expression));
        node->g_constant_type = NULL;
        node->g_expression_cmp_type = NULL;
        node->g_return_type = NULL;
        node->g_boolean_type = NULL;
        node->var = malloc(sizeof(variable));
        
        node->var->type = node->g_boolean_type;
        
        return node;
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

node_expression *
grammar_new_declaration_file_node(const variable *fpath,
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

node_file_block *
grammar_using_file(const node_expression *id,
                                    const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        return NULL;
}

node_expression *
grammar_new_variable(const node_expression *id,
                                      const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, id, expr);

        return NULL;
}

conditional_node *
grammar_new_conditional(const node_expression *condition,
                        const node_expression *expr_true,
                        const node_expression *expr_false)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, condition, 
                expr_true, expr_false);
        
        conditional_node* node = malloc(sizeof(conditional_node));
        node_expression* node->condition = malloc(sizeof(node_expression));
        node_expression* node->expr_true = malloc(sizeof(node_expression));
        node_expression* node->expr_false = malloc(sizeof(node_expression));
        
        node->condition = condition;
        node->expr_true = expr_true;
        node->expr_false = expr_false;
        
        return node;
}

loop_node *
grammar_new_loop(const node_expression *id,
                const node_expression *iterable,
                const node_expression *action)
{
        LogDebug("%s(%p, %p, %p)\n", __func__, id, iterable, action);

        loop_node * loop = malloc(sizeof(loop_node));
        loop->id = malloc(sizeof(node_expression));
        loop->iterable = malloc(sizeof(node_expression));
        loop->action = malloc(sizeof(node_expression));

        return loop;
}

list *
grammar_new_list(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);
        
        list * list = malloc(sizeof(list));
        list->head = malloc(sizeof(node_list));
        list->head->elem = malloc(sizeof(node_expression));

        list->head->elem = expr;
        if(expr != NULL) {
                list->head->elem->var->type = BLANK_TYPE;
                list->head->next = NULL;
        }
        list->head->next = NULL;
        return list; 
        /*
        Devuelve l->{elem (expr), next-> NULL}
        */
}

list *
grammar_concat_list_expr(const node_list *list,
                        const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        list* new_list = malloc(sizeof(node_list));
        new_list->type = LIST_COMMA_EXPRESSION_TYPE;
        new_list->head = malloc(sizeof(node_list));
        new_list->head = list;
        node_list* new_next = malloc(sizeof(node_list));
        new_next->head = expr;
        new_next->next = NULL;

        return new_list;

        // list->next = expr;
        // list->type = LIST_COMMA_EXPRESSION_TYPE;
        // list->next->next = NULL;

        // return list;
}

// node_list *
// recursive_list_add_from_list(const node_list* new_node,
//                         const node_list* first)
// {
//         if (first == NULL)
//         {

//                 node_list* new = malloc(sizeof(node_list));
//                 new->type = LIST_COMMA_RANGE_TYPE;
//                 new->next = NULL;
//                 new->elem = malloc(sizeof(node_expression));
//                 new->elem->var = malloc(sizeof(variable));
//                 // new->elem->var->value = new_var;
//                 // agragar bien los types mas abajo en los nodes
//                 return new;
//         }
//         first->next = recursive_list_add_from_list(new_var, first->next);
//         return first;
        
// }

node_list *
recursive_list_add(const variable_value new_var,
                const node_list* first)
{
        if (first == NULL)
        {
                node_list* new = malloc(sizeof(node_list));
                new->type = LIST_RANGE_TYPE;
                new->next = NULL;
                new->elem = malloc(sizeof(node_expression));
                new->elem->var = malloc(sizeof(variable));
                new->elem->var->value = new_var;
                // agragar bien los types mas abajo en los nodes
                return new;
        }
        first->next = recursive_list_add(new_var, first->next);
        return first;
        
}


list *
grammar_concat_list_list(const list *head_list,
                        const list *tail_list)
{
        LogDebug("%s(%p, %p)\n", __func__, head_list, tail_list);

        list* new_list = malloc(sizeof(list));
        new_list->type = LIST_COMMA_RANGE_TYPE;
        // new_list->head = head_list->head;
        node_list* j = head_list->head;

        while (j->next != NULL) {
                new_list->head = recursive_list_add(j->elem->var->value, new_list->head);
                j->next = j->next->next;
        }

        j->next = tail_list->head
        while (j->next != NULL) {
                new_list->head = recursive_list_add(j->elem->var->value, new_list->head);
                j->next = j->next->next;
        }

        // node_list* i = tail_list->head;
        // while (i->next != NULL) {
        //         i->next = i->next->next;
        // }

        return new_list;
        // return grammar_new_list_from_range(head_list, tail_list);
}

list *
grammar_new_list_from_range(const variable *start,
                        const variable *end)
{
        LogDebug("%s(%p, %p)\n", __func__, start, end);

        // node_expression* first = malloc(sizeof(node_expression));
        // first->var = malloc(sizeof(variable));
        // first->var = *start;

        list * to_return = malloc(sizeof(list));
        variable * i = *start;
        // to_return->head = recursive_list_add(i->value, to_return->head);
        while (i->value < end->value) {
                to_return->head = recursive_list_add(i->value, to_return->head);
                (i->value)++;
        }
        
        to_return->type = LIST_RANGE_TYPE;
        // node_list* aux = NULL;
        // node_list* first = to_return->head;
        // to_return->head->next = aux;

        // node_list * list = malloc(sizeof(node_list));
        // list->head = malloc(sizeof(node_expression));


        // list->head->var->type = i->type;
        // list->head->var->value = i->value;
        // i->value++;
        
        // while (i->value < end->value) {
        //         node_expression* new_elem = malloc(sizeof(node_expression));
        //         new_elem->var = malloc(sizeof(variable));
        //         new_elem->var = i;

        //         node_list* next = malloc(sizeof(node_list));
        //         next->elem = new_elem;
        //         first = next;
        //         aux = first;

                // next->next = aux;
                // next->head = grammar_new_list(new_elem);
                // next->type = LIST_RANGE_TYPE;
                // aux = next;


                // i->value += 1;

        //         node_expression * head_next = malloc(sizeof(node_expression));
        //         head_next->var = malloc(sizeof(variable));
        //         head_next->var->type = i->type;
        //         head_next->var->value = i->value;
        //         list->next = head_next;

        //         list->head = list->next;
        //         i->value++;  
        // }
        
        // list->head->var = start;
        // list->head->next->var = end;
        // // tail_list->next = NULL;

        return to_return;
}

node_list *
grammar_new_list_args(const node_expression *expr)
{
        LogDebug("%s(%p)\n", __func__, expr);

        node_list * list = malloc(sizeof(node_list));
        if (expr == NULL) {
                list->elem = malloc(sizeof(node_expression));
                // list->head->var = malloc(sizeof(variable));
                list->type = BLANK_TYPE;
                // list->head->var = expr;
        } else {
                list->type = EXPRESSION_TYPE;
        }
        list->elem = expr;
        list->next = NULL;

        return list;
}

node_list *
grammar_concat_list_args(const node_list *list,
                        const node_expression *expr)
{
        LogDebug("%s(%p, %p)\n", __func__, list, expr);

        node_list * new_list = malloc(sizeof(node_list));
        new_list->head = list;
        new_list->head->var->type = COMMA_TYPE;
        new_list->head->next = expr;
        list->head->next->next = NULL;

        return new_list;
}

node_expression *
grammar_expression_arithmetic_num_add(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->var->type = NUMBER_ARITHMETIC_ADD;
        strcpy(node->var->name, "NUMBER_ARITHMETIC_ADD");
        node->var->value = lvalue->var->value + rvalue->var->value;

        return node;
}

node_expression *
grammar_expression_arithmetic_num_sub(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->var->type = NUMBER_ARITHMETIC_SUB;
        strcpy(node->var->name, "NUMBER_ARITHMETIC_SUB");
        node->var->value = lvalue->var->value - rvalue->var->value;

        return node;
}

node_expression *
grammar_expression_arithmetic_num_mul(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->var->type = NUMBER_ARITHMETIC_MUL;
        strcpy(node->var->name, "NUMBER_ARITHMETIC_MUL");
        node->var->value = lvalue->var->value * rvalue->var->value;

        return node;
}

node_expression *
grammar_expression_arithmetic_num_div(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->var->type = NUMBER_ARITHMETIC_DIV;
        strcpy(node->var->name, "NUMBER_ARITHMETIC_DIV");
        node->var->value = lvalue->var->value / rvalue->var->value;

        return node;
}

node_expression *
grammar_expression_arithmetic_num_mod(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->var->type = NUMBER_ARITHMETIC_MOD;
        strcpy(node->var->name, "NUMBER_ARITHMETIC_MOD");
        node->var->value = lvalue->var->value % rvalue->var->value;

        return node;
}

node_expression *
grammar_expression_arithmetic_str_add(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        const size_t a = strlen(lvalue->var->value);
        const size_t b = strlen(rvalue->var->value);
        const size_t size_ab = a + b + 1;

        node_expression * concat = malloc(size_ab);

        memcpy(concat, lvalue->value, a);
        memcpy(concat + a, rvalue->value, b + 1);

        concat->var->type = ARITHMETIC_ADD;

        return concat;
}

node_expression *
grammar_expression_arithmetic_str_sub(const node_expression *lvalue,
                                      const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        char *p, *q, *r;
        char str = lvalue->var->value;
        char sub = rvalue->var->value;
        if (*sub && (q = r = strstr(str, sub)) != NULL) {
                size_t len = strlen(sub);
                while ((r = strstr(p = r + len, sub)) != NULL) {
                        memmove(q, p, r - p);
                        q += r - p;
                }
                memmove(q, p, strlen(p) + 1);
        }

        lvalue->var->type = ARITHMETIC_SUB;

        return lvalue;
}

node_expression *
grammar_expression_bool_and(const node_expression *lvalue,
                            const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->g_constant_type = NULL;
        node->g_expression_cmp_type = NULL;
        node->g_return_type = NULL;
        node->g_boolean_type = BOOL_TYPE_AND;
        node->var = malloc(sizeof(variable));
        node->var->value = (lvalue->var->value) && (rvalue->var->value);

        node->var->type = node->g_boolean_type;

        return node;
}

node_expression *
grammar_expression_bool_or(const node_expression *lvalue,
                        const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        node_expression * node = malloc(sizeof(node_expression));
        node->g_constant_type = NULL;
        node->g_expression_cmp_type = NULL;
        node->g_return_type = NULL;
        node->g_boolean_type = BOOL_TYPE_OR;
        node->var = malloc(sizeof(variable));
        node->var->value = (lvalue->var->value) || (rvalue->var->value);

        node->var->type = node->g_boolean_type;

        return node;
}

node_expression *
grammar_expression_bool_not(const node_expression *value)
{
        LogDebug("%s(%p)\n", __func__, value);

        node_expression * node = malloc(sizeof(node_expression));
        node->g_constant_type = NULL;
        node->g_expression_cmp_type = NULL;
        node->g_return_type = NULL;
        node->g_boolean_type = BOOL_TYPE_AND;
        node->var = malloc(sizeof(variable));
        node->var->value = !(value->var->value);

        node->var->type = node->g_boolean_type;

        return node;

}

node_expression *
grammar_expression_cmp_equals(const node_expression *lvalue,
                                const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        // hacer un chequeo de CMP_EXP_IS??
        // if (grammar_expression_cmp_by_type(lvalue->))
        // {
        //         /* code */
        // }
        

        node_expression * node = malloc(sizeof(node_expression));
        node->g_expression_cmp_type = CMP_EXP_EQUALS;
        node->var = malloc(sizeof(variable));
        node->var->type = node->g_expression_cmp_type;
        node->var->value = lvalue->var->value != lvalue->var->value;

        // return CMP_EXP_EQUALS;
        return node;
}

node_expression *
grammar_expression_cmp_not_equals(const node_expression *lvalue,
                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);


        // hacer un chequeo de CMP_EXP_IS??
        // if (grammar_expression_cmp_by_type(lvalue->))
        // {
        //         /* code */
        // }
        

        node_expression * node = malloc(sizeof(node_expression));
        node->var = malloc(sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_NOT_EQUALS;
        node->var->type = node->g_expression_cmp_type;
        node->var->value = lvalue->var->value != lvalue->var->value;

        // return CMP_EXP_NOT_EQUALS;
        return node;
}

node_expression *
grammar_expression_cmp_greater_than(const node_expression *lvalue,
                                    const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        // hacer un chequeo de CMP_EXP_IS??
        // if (grammar_expression_cmp_by_type(lvalue->))
        // {
        //         /* code */
        // }

        node_expression * node = malloc(sizeof(node_expression));
        node->var = malloc(sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_GREATER_THAN;
        node->var->type = node->g_expression_cmp_type;
        node->var->value = lvalue->var->value > lvalue->var->value;

        // return CMP_EXP_GREATER_THAN;
        return node;
}

node_expression *
grammar_expression_cmp_greater_equal(const node_expression *lvalue,
                                     const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        // hacer un chequeo de CMP_EXP_IS??
        // if (grammar_expression_cmp_by_type(lvalue->))
        // {
        //         /* code */
        // }

        node_expression * node = malloc(sizeof(node_expression));
        node->var = malloc(sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_GREATER_EQUAL;
        node->var->type = node->g_expression_cmp_type;
        node->var->value = lvalue->var->value >= lvalue->var->value;

        // return CMP_EXP_GREATER_EQUAL;
        return node;
}

node_expression *
grammar_expression_cmp_less_than(const node_expression *lvalue,
                                        const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        // hacer un chequeo de CMP_EXP_IS??
        // if (grammar_expression_cmp_by_type(lvalue->))
        // {
        //         /* code */
        // }

        node_expression * node = malloc(sizeof(node_expression));
        node->var = malloc(sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_LESS_THAN;
        node->var->type = node->g_expression_cmp_type;
        node->var->value = lvalue->var->value < lvalue->var->value;

        // return CMP_EXP_LESS_THAN;
        return node;
}

node_expression *
grammar_expression_cmp_less_equal(const node_expression *lvalue,
                                  const node_expression *rvalue)
{
        LogDebug("%s(%p, %p)\n", __func__, lvalue, rvalue);

        // hacer un chequeo de CMP_EXP_IS??
        // if (grammar_expression_cmp_by_type(lvalue->))
        // {
        //         /* code */
        // }

        node_expression * node = malloc(sizeof(node_expression));
        node->var = malloc(sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_LESS_EQUAL;
        node->var->type = node->g_expression_cmp_type;
        node->var->value = lvalue->var->value <= lvalue->var->value;

        // return CMP_EXP_LESS_EQUAL;
        return node;
}

node_expression *
grammar_expression_cmp_by_type(const node_expression *expr,
                                const token_t type)
{
        LogDebug("%s(%p, %d)\n", __func__, expr, type);

        node_expression * node = malloc(sizeof(node_expression));
        node->var = malloc(sizeof(variable));
        node->g_expression_cmp_type = CMP_EXP_IS;
        node->var->type = node->g_expression_cmp_type;
        node->var->value = expr->var->type == type;

        // return CMP_EXP_IS;
        return node;
}

node_expression *
grammar_new_return_node(const node_expression *id)
{
        LogDebug("%s(%p)\n", __func__, id);

        node_expression * node = malloc(sizeof(node_expression));
        if(*id != NULL){
                node->var = malloc(sizeof(variable));
                strcpy(node->var->name, id->var->name);
                node->var->type = id->var->type;
                // node->value->name = id;
        }
        // tendriamos que dejarlo asi?
        
        // xq se me ocurre que tendria que devolver
        // el node_expression que tiene ese id

        // estilo ciclar y buscarlo pero no tiene mucho
        // sentido

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
        expr->var->type = id->var->type;
        strcpy(expr->var->name, id->var->name);

        return expr;
}

// CHEQUEAR
// de que valor tiene que ser el parametro de entrada
node_expression *
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
        var->type = BOOL_TYPE;
        strcpy(var->name, "BOOL_TYPE"); // ?

        node_expression * newNode = malloc(sizeof(node_expression));
        newNode->var = var;
        // newNode->type = ?

        return node;
}

node_expression *
grammar_constant_number(const variable_value *value)
{
        LogDebug("%s(%s)\n", __func__, value);
        
        variable * var = malloc(sizeof(variable));
        var->value = atoi(value);
        var->type = NUMBER_TYPE;
        strcpy(var->name, "NUMBER_TYPE"); // ?

        node_expression * newNode = malloc(sizeof(node_expression));
        newNode->var = var;
        // newNode->type = ?

        return node;
}

node_expression *
grammar_constant_string(const variable_value *value)
{
        LogDebug("%s(%s)\n", __func__, value);

        variable * var = malloc(sizeof(variable));
        strcpy(var->value, value);
        var->type = STRING_TYPE;
        strcpy(var->name, "STRING_TYPE"); // ?

        node_expression * newNode = malloc(sizeof(node_expression));
        newNode->var = var;
        // newNode->type = ?

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
