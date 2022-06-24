#include "code-generator.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

//utils with functions to deal with code generation for each specific node type

void generate_type(token_t type);
void generate_declaration(node_expression * d);
void generate_assign_declaration (node_expression * ad);
void generate_assign(node_expression* a);
void generate_function(node_function* nf);
void generate_variable_value(variable * var);
void generate_return(variable* v);


char* variable_types[] = {"double", "bool", "char *"};
void generate_expression_list(node_expression_list* el);

void generate_expression(node_expression* expr){
  return;
}

void generate_code(program_t* ast)
{
    generate_header();
    generate_function(ast->main_function);
}

void generate_expression_list(node_expression_list* el)
{
    node_expression* prev;
    while (el != NULL) {
        generate_expression(el->expr);
        prev = el;
        el = el->next;
    }
}

void generate_args(node_list* args)
{
    node_list* prev;
    if(args!=NULL){
    node_expression* current = args->exprs;
      while (current != NULL) {
          generate_declaration(current);
          if (current->next != NULL) printf(",");
          prev = args;
          args = current->next;
      }
    }
}

// HEADER
void generate_header()
{
    printf(
        "#include<stdlib.h>\n"\
        "#include<stdio.h>\n"\
        "#include<strings.h>\n"\
        "\n");
}

void generate_type(token_t type)
{
    printf("%s", variable_types[type]);
}

//when called as a line, use ; at the end
void generate_declaration(node_expression * d)
{
    generate_type_code(d->var->type);
    printf(" %s", d->var->name);
}

void generate_assign_declaration (node_expression * ad) {
    generate_type_code(ad->var->type);
    generate_assign(ad);
}

void generate_assign(node_expression* a)
{
    printf("%s = ", a->var->name);
    switch(a->type) {
        case NUMBER_TYPE: 
        case BOOL_TYPE:
        case STRING_TYPE:
        case FILE_PATH_TYPE:
            generate_variable_value(a->var);
        break;
        case EXPRESSION_TYPE:
        case EXPRESSION_GRAMMAR_CONSTANT_TYPE:
        case EXPRESSION_VARIABLE_ASSIGNMENT:
        case EXPRESSION_VARIABLE_DECLARATION:
        case EXPRESSION_FILE_DECLARATION:
            generate_expression(a);
        break;
    }
    printf(";\n");
}

void generate_function(node_function* nf)
{
    generate_type_code(nf->return_variable->type);
    printf(" %s(", nf->name);
    generate_args(nf->args);
    printf(")\n");
    printf("{\n");
    generate_expression_list(nf->expressions);
    generate_return(nf->return_variable);
    printf("}\n");
}

void generate_variable_value(variable * var)
{
    switch (var->type) {
    case BOOL_TYPE:
        printf("%d", var->boolean);
        break;
    case NUMBER_TYPE:
        printf("%f", var->number);
        break;
    case STRING_TYPE:
        printf("%s", var->string);
        break;
    }
}

void generate_return(variable* v)
{
    printf("return ");
    if(v == NULL){
      printf(";\n");
      return;
    }
    switch (v->type)
    {
      case NUMBER_TYPE:
      case BOOL_TYPE:
      case STRING_TYPE:
        generate_variable_value(v);
      case ID:
        if (v->value->expr != NULL)
            generate_expression(v->value->expr);
        break;
      default:
        break;
    }
    printf(";\n");
}