#include "code_generator.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

//utils with functions to deal with code generation for each specific node type

void dummy(ast_node_t*);
void generate_declaration(declaration_node_t* declaration_node);
void generate_constant(const_node_t* constant);
void generate_if_code(if_node_t* if_node);
void generate_loop_code(while_node_t* loop_node);
void generate_function_call(func_call_node_t* fun_call);
void generate_function_declaration(function_node_t* node);
void generate_assign_expression(node_expression* a);
void generate_assign(assign_node_t* assign_node);
void generate_variable(variable_node_t* node);
void generate_expression_code(compound_expression_node_t* expression_node);
void generate_return_code(return_node_t* node);
void generate_decl_assign_code(decl_assign_node_t* node);


//map of handlers to generate code according to the node type. 
void (*generators[])(ast_node_t* node) = {
    dummy, //root
    generate_declaration,
    generate_constant,
    generate_if_code,
    generate_loop_code,
    generate_function_call,
    generate_function_declaration,
    generate_assign_expression,
    generate_assign,
    generate_variable,
    generate_expression_code,
    generate_return_code,
    generate_decl_assign_code,

};

char* types[] = {"int", "double", "bool", "char *", "void"};
char* ops[] = {"+", "-", "*", "/", "<", ">", "<=", ">=", "==", "!=", "!", "&&", "||"};

void generate_expression_list(node_expression_list* el);

void generate(ast_node_t* node)
{
    generators[node->type](node);
}

void dummy(ast_node_t* dummy) {};

void generate_code(root_node_t* tree)
{
    generate_initial_setup();
    generate_expression_list(tree->global_variables);
    generate_expression_list(tree->functions);
    printf("int main(){\n");
    printf("init();\n");
    generate_expression_list(tree->main);

    printf("loop();\nreturn 0;\n}\n\n\n");
}


// generar código de función (lista de expr)
void generate_expression_list(node_expression_list* el)
{
    node_expression* prev;
    while (el != NULL) {
        generate(el);
        prev = el;
        el = el->next;
        //free(prev);
    }
}

// TODO
void generate_params_list(list_node_t* list)
{
    list_node_t* prev;
    while (list != NULL) {
        generate(list->node);
        if (list->next != NULL) printf(",");
        prev = list;
        list = list->next;
        //free(prev);
    }
}

// HEADER
void generate_initial_setup()
{
    printf(
        "#include<stdlib.h>\n"\
        "#include<stdio.h>\n"\
        "#include<strings.h>\n"\
        // "#include\"lib/figures.h\"\n"
        // "#include\"lib/handlers.h\"\n"
        "\n");
}

// {"int", "double" ,"bool", "char *", "void"}
void generate_type(token_t type)
{
    printf("%s", types[type]);
}

// int i;
void generate_declaration(declaration_node_t* declaration_node)
{
    generate_type_code(declaration_node->var->type);
    printf(" %s ", declaration_node->var->name);
}

// let abc be True. 
// bool abc = true;
void generate_assign_expression(node_expression* a)
{
    printf("%s = ", a->var->name);
    generate_value(a->var->value);
    // genera según tipo de producción
    generate(a->expr);
    printf(";\n");
}

void generate_expression_code(node_expression_list* e)
{
    generate_type_code(nf->return_variable->type);
    if (expression_node->closed) {
        printf(" ( ");
    }
    if (expression_node->left != NULL)
        generate(expression_node->left);
    printf(" %s ", ops[expression_node->operator]);
    generate(expression_node->right);
    if (expression_node->closed)
        printf(" ) ");
}

void generate_function_call(func_call_node_t* fun_call)
{
    printf("%s(", fun_call->name);
    generate_params_list(fun_call->params);
    printf(")");
    if (fun_call->terminal)
        printf(";\n");
}

void generate_function(node_function* nf)
{
    generate_type_code(nf->return_variable->type);
    printf(" %s(", nf->name);
    // generate_params_list(node->parameters);
    printf(")\n");
    printf("{\n");
    generate_expression_list(nf->expressions);
    printf("return %s;\n", nf->return_variable->name);
    printf("}\n");
}

void generate_variable(variable * var)
{
    switch (var->type) {
    case bool:
        printf("%d", var->boolean);
        break;
    case double:
        printf("%f", var->number);
        break;
    case tTEXT:
        printf("%s", var->string);
        break;
    }
}

/*
void generate_variable(variable * var)
{
    generate_constant(var->)
    printf(" %s ", node->var->name);
}*/

/*void generate_return_code(return_node_t* node)
{
    printf("return ");
    if (node->expression != NULL)
        generate(node->expression);
    printf(";\n");
}*/

// !!!!!!!!
// void generate_assign(decl_assign_node_t* e)
// {
//     printf("%s %s = ", types[e->var->type], e->var->name);
//     // completar
//     generate(node->expression);
//     printf(";\n");
// }