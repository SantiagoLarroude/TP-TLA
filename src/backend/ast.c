#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "symbols.h"

extern int yylineno;


int check_compatibility(var_type t1, var_type t2) {
    if( t1 == tBOOL && t2 == tINT) return 1;
    if( t2 == tBOOL && t1 == tINT) return 1;
    if (t1==tFUNCTION && t2 == tVOID) return 1;
    if (t1 == tDOUBLE && t2 == tINT) return 1;
    if (t1 == tFIGURE && (
        t2 == tRECTANGLE ||
        t2 == tCIRCLE ||
        t2 == tLINE ||
        t2 == tDOT)) return 1;

    return t1 == t2;
}


root_node_t* new_root_node() {
    root_node_t* node = calloc(1, sizeof(root_node_t));
    node->type = ROOT;
    init_table();
    return node;
}

declaration_node_t* new_declaration_node(char* var_name, var_type type) {
    variable* var = calloc(1, sizeof(variable));
    var->name = calloc(1, strlen(var_name));
    strcpy(var->name, var_name);
    var->type = type;

    if (insert(var) < 0) {
        multiple_declaration(var_name, yylineno, 1);
        exit(1);
    }

    declaration_node_t* new_node = calloc(1, sizeof(declaration_node_t));
    new_node->type = DECLARATION_N;
    new_node->var = var;
    new_node->terminal = 1;
    return new_node;
}

assign_node_t* new_assign_node(char* variable_name, expression_node_t* expression) {  //TODO: ver tema de que tipo es este expression node
    variable* var = lookup(variable_name);
    if (var == NULL) {
        undefined_reference(variable_name, yylineno);
        exit(1);
    } else if (!check_compatibility(var->type, expression->expression_type)) {
        incompatible_types(var->type, expression->expression_type, yylineno);
        exit(1);
    }
    assign_node_t* new_node = calloc(1, sizeof(assign_node_t));
    new_node->type = ASSIGN_N;
    new_node->var = var;
    new_node->expression = expression;
    return new_node;
}

if_node_t* new_if_node(expression_node_t* expression, list_node_t* code) {
    if_node_t* if_node = calloc(1, sizeof(if_node_t));
    if_node->type = CTL_N;
    if_node->condition = expression;
    if_node->then = code;
    return if_node;
}

while_node_t* new_loop_node(expression_node_t* expression, list_node_t* code) {
    while_node_t* loop_node = calloc(1, sizeof(while_node_t));
    loop_node->type = LOOP_N;
    loop_node->condition = expression;
    loop_node->routine = code;
    return loop_node;
}

variable_node_t* new_var_node(char* variable_name) {
    variable* var = lookup(variable_name);
    if (var == NULL) {
       undefined_reference(variable_name, yylineno);
        exit(1);
    }
    variable_node_t* var_node = calloc(1, sizeof(variable_node_t));
    var_node->type = VARIABLE_REF_N;
    var_node->var_type = var->type;
    var_node->var = var;
    return var_node;
}

expression_node_t* not_expression_node(expression_node_t* right) {
    compound_expression_node_t* expression_node = calloc(1, sizeof(compound_expression_node_t));
    expression_node->type = EXPRESSION_N;
    expression_node->right = right;
    expression_node->operator= oNOT;
    expression_node->expression_type = tBOOL;
    return expression_node;
}

expression_node_t* new_compose_expr_node(expression_node_t* left, char operator, expression_node_t * right) {
    if (!check_compatibility(left->expression_type, right->expression_type)){
        incompatible_types_operation(left->expression_type, right->expression_type, operator, yylineno);
        exit(1);
    }
    if (left->expression_type > tBOOL) {
        illegal_operation(left->expression_type, operator, yylineno);
        exit(1);
    }
    compound_expression_node_t* expression_node = calloc(1, sizeof(compound_expression_node_t));
    expression_node->type = EXPRESSION_N;
    expression_node->expression_type = left->expression_type;
    expression_node->left = left;
    expression_node->right = right;
    expression_node->operator= operator;
    return expression_node;
}
/* Terminal nodes */

const_node_t* new_const_node(variable_value value) {
    const_node_t* const_node = calloc(1, sizeof(const_node_t));
    const_node->type = CONSTANT_N;
    const_node->value = value;
    return const_node;
}

const_node_t* new_true_node() {
    const_node_t* const_node = calloc(1, sizeof(const_node_t));
    const_node->type = CONSTANT_N;
    const_node->constant_type = tBOOL;
    const_node->value.bool_val = 1;
    return const_node;
}

const_node_t* new_false_node() {
    const_node_t* const_node = calloc(1, sizeof(const_node_t));
    const_node->type = CONSTANT_N;
    return const_node;
}

const_node_t* new_int_node(variable_value value) {
    const_node_t* const_node = calloc(1, sizeof(const_node_t));
    const_node->type = CONSTANT_N;
    const_node->constant_type = tINT;
    const_node->value.int_val = value.int_val;
    return const_node;
}

const_node_t* new_double_node(variable_value value) {
    const_node_t* const_node = calloc(1, sizeof(const_node_t));
    const_node->type = CONSTANT_N;
    const_node->constant_type = tDOUBLE;
    const_node->value.double_val = value.double_val;
    return const_node;
}

const_node_t* new_string_node(variable_value value) {
    const_node_t* string_node = calloc(1, sizeof(const_node_t));
    string_node->type = CONSTANT_N;
    string_node->constant_type = tTEXT;
    string_node->value.text_val = value.text_val;
    return string_node;
}

list_node_t* new_param_decl_node(char* name, var_type type) {
    list_node_t* params = calloc(1, sizeof(list_node_t));
    declaration_node_t* decl_node = new_declaration_node(name, type);
    decl_node->terminal = 0;
    params->node = decl_node;
    return params;
}

function_node_t* new_function_node(char* name, var_type type, list_node_t* params, list_node_t* code, return_node_t * return_node) {
    
    if (return_node->expression == NULL && type != tVOID){
        incompatible_return_type(type, tVOID, name, yylineno);
        exit(1);
    }

    if(return_node->expression != NULL && return_node->expression->expression_type != type){
        incompatible_return_type(type, return_node->expression->expression_type, name, yylineno);
        exit(1);
    }
    
    function* func = calloc(1, sizeof(function));
    func->name = name;
    func->return_type = type;
    if (insert((variable*)func) < 0) {
        multiple_declaration(name, 0, yylineno);
        exit(1);
    }
    int param_qty = 0;
    list_node_t* aux = params;
    while (aux != NULL) {
        param_qty++;
        param_type_node* new_node = calloc(1, sizeof(param_type_node));
        new_node->type = ((declaration_node_t *)aux->node)->var->type;
        new_node->next = func->first;
        func->first = new_node;
        aux = aux->next;
    }
    func->param_qty = param_qty;

    function_node_t* func_node = calloc(1, sizeof(function_node_t));
    func_node->type = FUNDECL_N;
    func_node->function = func;
    list_node_t * return_code_list = calloc(1,sizeof(list_node_t));
    return_code_list->node = return_node;
    code = concat_lists(return_code_list,code);
    func_node->code = code;
    func_node->parameters = params;
}



func_call_node_t* new_function_call_node(char* name, list_node_t* params) {
    function* function = lookup(name);
    if (function == NULL) {
        undefined_reference(name,yylineno );
        exit(1);
    }
    func_call_node_t* func_call_node = calloc(1, sizeof(func_call_node_t));
    func_call_node->type = FUNCALL_N;
    list_node_t* aux = params;
    int param_count=0;
    param_type_node* aux_type = function->first;
    while (aux != NULL) {
        if (aux_type == NULL) {
            incompatible_parameter_count(param_count, function->param_qty, function->name, yylineno);
            exit(1);
        }
        if (!check_compatibility(aux_type->type, ((expression_node_t*)aux->node)->expression_type)) {
            incompatible_types_function(aux_type->type, ((expression_node_t*)aux->node)->expression_type, function->name, yylineno);
            exit(1);
        } else {
            param_count ++;
            aux = aux->next;
            aux_type = aux_type->next;
        }
    }
    if (aux_type == NULL && aux != NULL) {
        incompatible_parameter_count(param_count, function->param_qty, function->name, yylineno);
        exit(1);
        //ERROR: PARAMETROS DE MAS EN LA LLAMADA A LA FUNCION
    }
    func_call_node->name = name;
    func_call_node->params = params;
    func_call_node->ret_type = function->return_type;
    return func_call_node;
}

return_node_t* new_return_node(expression_node_t* expression_node) {
    return_node_t* ret_node = calloc(1, sizeof(return_node_t));
    ret_node->type = RETURN_N;
    ret_node->expression = expression_node;
    return ret_node;
}

list_node_t* new_param_node(expression_node_t* expression_node) {
    list_node_t* params = calloc(1, sizeof(list_node_t));
    params->node = expression_node;
    return params;
}


list_node_t* concat_node(list_node_t* list, ast_node_t* code) {
    list_node_t* new_node = calloc(1, sizeof(list_node_t));
    new_node->node = code;
    new_node->next = list;
    list = new_node;
    return list;
}

list_node_t* concat_lists(list_node_t* main, list_node_t* instructions) {
    list_node_t* aux = instructions;
    //  if(aux == NULL)
    //     aux = instructions;
    //else{
    while (aux->next != NULL) aux = aux->next;
    // aux->next = instructions;

    aux->next = main;
    //}

    return instructions;
}

void set_terminal(func_call_node_t * fun_call){
    fun_call->terminal = 1;
}

void set_closed(expression_node_t * expression){
    if(expression->type == EXPRESSION_N){
        ((compound_expression_node_t *)expression)->closed = 1;
    }
}

void set_easter_egg(root_node_t * root){
    root->easter_egg = 1;
}

char *random_string(size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK";
    char * str = malloc(size+1);
    for (size_t n = 0; n < size; n++) {
        int key = rand() % sizeof(charset);
        str[n] = charset[key];
    }
    str[size] = '\0';
    return str;
}

list_node_t * new_lambda_function(root_node_t * root, list_node_t * code, return_node_t * ret){
    
    char * name = NULL;
    while(name == NULL || lookup(name) != NULL){
        if(name != NULL) free(name);
        name = random_string(10);
    }
     
    function_node_t * func = new_function_node(name,tVOID,NULL, code, ret);
    root->functions = concat_node(root->functions, func);
    variable_node_t * var = new_var_node(name);
    return new_param_node(var);

}

func_call_node_t * new_join_call(char * var1, char * var2){
    variable_node_t * f1 = new_var_node(var1);
    variable_node_t * f2 = new_var_node(var2);
    list_node_t * params = new_param_node(f1);
    params->next = new_param_node(f2); 
    return new_function_call_node("join",params);
}

decl_assign_node_t * new_assign_decl_node(char * name, var_type type, expression_node_t * expr){
    variable* var = calloc(1, sizeof(variable));
    var->name = calloc(1, strlen(name));
    strcpy(var->name, name);
    var->type = type;
    if (insert(var) < 0) {
        multiple_declaration(name,1,  yylineno);
        exit(1);
    }
    decl_assign_node_t * node = calloc(1,sizeof(decl_assign_node_t));
    node->expression = expr;
    node->var = var;
    node->type = DECL_ASSIGN_N;
    return node;
}