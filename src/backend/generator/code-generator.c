#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../logger.h"
#include "../error.h"

#include "free_function_call.h"
#include "standard_functions.h"
#include "internal_functions.h"

#include "code-generator.h"

//utils with functions to deal with code generation for each specific node type

static FILE *open_output_file(const char *filename);
static bool generate_c_main(FILE *const output, node_function *main_function);

static void generate_header(FILE *const output);
static void generate_header_types_TexlerObject(FILE *const output);
static void generate_header_types(FILE *const output);
static void generate_header_macros_and_constants(FILE *const output);

static bool generate_function(FILE *const output, node_function *function);
static bool generate_args(FILE *const output, node_list *args);
static bool generate_return(FILE *const output, const variable *var);

static bool generate_expressions_list(FILE *const output,
                                      node_expression_list *expressions);
static bool generate_variable(FILE *const output, variable *var,
                              free_function_call_array *frees_stack);

static bool generate_variable_assignment(FILE *const output, variable *var,
                                         node_expression *expr);
static bool generate_variable_assignment_to_variable(FILE *const output,
                                                     variable *dest,
                                                     variable *source);
static bool generate_variable_assignment_to_constant(FILE *const output,
                                                     variable *dest,
                                                     variable *source);

// void generate_type_code(token_t type);
// void generate_declaration(node_expression *declaration);
// void generate_assign_declaration(node_expression *assign_declaration);
// void generate_assign(node_expression *assignment);
// void generate_return(variable *v);
// static void error_handler(char *msg);
//
// char *variable_types[] = { "double", "bool", "char *", "int" };

bool generate_code(program_t *ast, const char *filename)
{
        if (filename == NULL) {
                LogError("Please provide a filename for the C code.");
                return false;
        }

        FILE *out_file = open_output_file(filename);

        if (out_file == NULL)
                return false;

        generate_header(out_file);
        generate_internal_functions(out_file);
        generate_standard_functions(out_file);

        if (!generate_function(out_file, ast->main_function)) {
                fclose(out_file);
                return false;
        }

        if (!generate_c_main(out_file, ast->main_function)) {
                fclose(out_file);
                return false;
        }

        fclose(out_file);

        return true;
}

void generate_allocation_error_msg(FILE *const output, char *ptr_name)
{
        fprintf(output,
                "if (%s == NULL) {"
                "perror(\"Aborting due to\");"
                "exit(1);"
                "}",
                ptr_name);
}

static FILE *open_output_file(const char *filename)
{
        if (filename == NULL)
                return NULL;

        FILE *fptr = fopen(filename, "w+");
        if (fptr == NULL) {
                perror("Error while opening output file");
                return NULL;
        }

        return fptr;
}

static bool generate_c_main(FILE *const output, node_function *main_function)
{
        if (main_function == NULL)
                return false;

        fprintf(output,
                "int main(void)"
                "{"
                " %s();"
                "return 0;"
                "}",
                main_function->name);

        return true;
}

/* HEADER */
static void generate_header(FILE *const output)
{
        fprintf(output, "#include <ctype.h>\n"
                        "#include <dirent.h>\n"
                        "#include <stdio.h>\n"
                        "#include <stdlib.h>\n"
                        "#include <stdbool.h>\n"
                        "#include <string.h>\n");
        generate_header_macros_and_constants(output);
        generate_header_types(output);
        generate_internal_functions_headers(output);
        generate_standard_functions_headers(output);
}

static void generate_header_macros_and_constants(FILE *const output)
{
        fprintf(output, "#define BUFFER_SIZE 256\n");
        fprintf(output, "const char *DEFAULT_SEPARATORS = \" ,\";\n");
}

static void generate_header_types(FILE *const output)
{
        fprintf(output, "typedef enum {"
                        "TYPE_T_NONE = 0,"
                        "TYPE_T_BOOLEAN,"
                        "TYPE_T_STRING,"
                        "TYPE_T_REAL,"
                        "TYPE_T_INTEGER,"
                        "TYPE_T_FILEPTR,"
                        "TYPE_T_FILE_LIST,"
                        "N_TYPE_T"
                        "} type_t;\n");

        generate_header_types_TexlerObject(output);
}

static void generate_header_types_TexlerObject(FILE *const output)
{
        fprintf(output, "typedef struct TexlerObject TexlerObject;\n");
        fprintf(output, "struct TexlerObject {"
                        "union {"
                        "bool boolean;"
                        "struct {"
                        "char *string;"
                        "size_t length;"
                        "};"
                        "double real;"
                        "long integer;"
                        "struct {"
                        "FILE *stream;"
                        "fpos_t pos;"
                        "char **path_list;"
                        "union {"
                        "size_t n_line;"
                        "size_t n_files;"
                        "};"
                        "} file;"
                        "} value;"
                        "type_t type;"
                        "};");

        fprintf(output, "void free_texlerobject(TexlerObject *tex_obj)"
                        "{"
                        "if (tex_obj == NULL) { return; }"
                        ""
                        "switch (tex_obj->type) {"
                        "case TYPE_T_FILEPTR:"
                        "if (tex_obj->value.file.stream != NULL &&"
                        "tex_obj->value.file.stream != stdout &&"
                        "tex_obj->value.file.stream != stderr &&"
                        "tex_obj->value.file.stream != stdin)"
                        "{"
                        "fclose(tex_obj->value.file.stream);"
                        "}"
                        "break;"
                        "case TYPE_T_FILE_LIST:"
                        "if (tex_obj->value.file.path_list != NULL)"
                        "{"
                        "while (tex_obj->value.file.n_files > 0) {"
                        "if ("
                        "tex_obj->value.file.path_list"
                        "[tex_obj->value.file.n_files - 1]"
                        "!= NULL"
                        ")"
                        "{"
                        "free("
                        "tex_obj->value.file.path_list"
                        "[tex_obj->value.file.n_files -1]"
                        ");"
                        "}"
                        ""
                        "tex_obj->value.file.n_files--;"
                        "}"
                        "free(tex_obj->value.file.path_list);"
                        "}"
                        "break;"
                        "case TYPE_T_STRING:"
                        "if (tex_obj->value.string != NULL)"
                        "{"
                        "free(tex_obj->value.string);"
                        "}"
                        "break;"
                        "case TYPE_T_BOOLEAN: /* Fallsthrough */"
                        "case TYPE_T_REAL:"
                        "case TYPE_T_INTEGER:"
                        "break;"
                        "default:"
                        "break;"
                        "}"
                        "memset(tex_obj, 0, sizeof(TexlerObject));"
                        "free(tex_obj);"
                        "}");
}

/* Function generation */
static bool generate_function(FILE *const output, node_function *function)
{
        if (function == NULL)
                return false;

        if (function->return_variable == NULL) {
                fprintf(output, " void ");
        } else {
                fprintf(output, " TexlerObject * ");
        }

        fprintf(output, " %s (", function->name);
        if (!generate_args(output, function->args)) {
                error_in_function(function->name);
                return false;
        }
        fprintf(output, ") ");

        fprintf(output, " { ");
        // generate_expression_list(function->expressions);
        if (!generate_expressions_list(output, function->expressions)) {
                error_in_function(function->name);
                return false;
        }

        if (!generate_return(output, function->return_variable)) {
                error_in_function(function->name);
                return false;
        }

        fprintf(output, " } ");
        return true;
}

static bool generate_args(FILE *const output, node_list *args)
{
        if (args == NULL || args->type != LIST_ARGS_TYPE ||
            args->exprs == NULL)
                return false;

        for (size_t i = 0; i < args->len; i++) {
                node_expression *arg = args->exprs[i];
                if (arg == NULL)
                        continue;

                if (arg->var == NULL || arg->var->name == NULL ||
                    arg->type != EXPRESSION_VARIABLE) {
                        error_invalid_function_arguments();
                        return false;
                } else {
                        fprintf(output, "TexlerObject * %s", arg->var->name);
                        if (i != args->len - 1) {
                                fputc(',', output);
                        }
                }
        }

        return true;
}

static bool generate_expressions_list(FILE *const output,
                                      node_expression_list *expressions)
{
        if (expressions == NULL || expressions->expr == NULL)
                return false;

        free_function_call_array *frees_stack =
                (free_function_call_array *)calloc(
                        1, sizeof(free_function_call_array));
        if (frees_stack == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        while (expressions != NULL && expressions->expr != NULL) {
                switch (expressions->expr->type) {
                case EXPRESSION_VARIABLE_DECLARATION:
                        generate_variable(output, expressions->expr->var,
                                          frees_stack);
                        break;
                case EXPRESSION_VARIABLE_ASSIGNMENT:
                        generate_variable_assignment(output,
                                                     expressions->expr->var,
                                                     expressions->expr->expr);
                        break;
                default:
                        LogDebug("Got expression of type: %d\n"
                                 "\tFunction:",
                                 expressions->expr->type, __func__);
                        break;
                }

                expressions = expressions->next;
        }

        while (frees_stack->size > 0) {
                free_function_call *ffc = pop_free_function_call(&frees_stack);

                if (ffc != NULL)
                        fprintf(output, "%s(%s);", ffc->fun, ffc->name);

                free_struct_free_function_call(&ffc);
        }

        free_struct_free_function_call_array(&frees_stack);
        return true;
}

static bool generate_variable(FILE *const output, variable *var,
                              free_function_call_array *frees_stack)
{
        if (output == NULL || var == NULL || frees_stack == NULL)
                return false;

        if (var->name == NULL)
                return false;

        fprintf(output,
                "TexlerObject * %s = "
                "(TexlerObject *)calloc(1, sizeof(TexlerObject));",
                var->name);

        generate_allocation_error_msg(output, var->name);

        switch (var->type) {
        case NUMBER_TYPE:
                fprintf(output, "%s->type = TYPE_T_REAL;", var->name);
                fprintf(output, "%s->value.real = %f;", var->name,
                        var->value.number);
                break;
        case BOOL_TYPE:
                fprintf(output, "%s->type = TYPE_T_BOOLEAN;", var->name);
                fprintf(output, "%s->value.boolean = %d;", var->name,
                        var->value.boolean);
                break;
        case STRING_TYPE:
                fprintf(output, "%s->type = TYPE_T_BOOLEAN;", var->name);
                fprintf(output, "%s->value.string = %s;", var->name,
                        var->value.string);
                fprintf(output, "%s->value.length = %ld;", var->name,
                        strlen(var->value.string));
                break;
        case CONSTANT_TYPE:
                switch (var->value.expr->var->type) {
                case NUMBER_TYPE:
                        fprintf(output, "%s->type = TYPE_T_REAL;", var->name);
                        fprintf(output, "%s->value.real = %f;", var->name,
                                var->value.expr->var->value.number);
                        break;
                case BOOL_TYPE:
                        fprintf(output, "%s->type = TYPE_T_BOOLEAN;",
                                var->name);
                        fprintf(output, "%s->value.boolean = %d;", var->name,
                                var->value.expr->var->value.boolean);
                        break;
                case STRING_TYPE:
                        fprintf(output, "%s->type = TYPE_T_BOOLEAN;",
                                var->name);
                        fprintf(output, "%s->value.string = %s;", var->name,
                                var->value.expr->var->value.string);
                        fprintf(output, "%s->value.length = %ld;", var->name,
                                strlen(var->value.expr->var->value.string));
                        break;
                }
                break;
        default:
                LogDebug("Got variable type: %d\n"
                         "\tFunction:",
                         var->type, __func__);
        }

        push_free_function_call(frees_stack, var->name, "free_texlerobject");

        return true;
}

static bool generate_variable_assignment(FILE *const output, variable *var,
                                         node_expression *expr)
{
        if (output == NULL || var == NULL || expr == NULL)
                return false;

        switch (expr->type) {
        case EXPRESSION_GRAMMAR_CONSTANT_TYPE: // ie: True -> ID.
                if (!generate_variable_assignment_to_constant(output, var,
                                                              expr->var)) {
                        return false;
                }
                break;
        case VARIABLE_TYPE: // ID -> ID.
                if (!generate_variable_assignment_to_variable(output, var,
                                                              expr->var)) {
                        return false;
                }
                break;
        default:
                break;
        }

        return true;
}

static bool generate_variable_assignment_to_variable(FILE *const output,
                                                     variable *dest,
                                                     variable *source)
{
        if (output == NULL || dest == NULL || source == NULL)
                return false;

        if (dest->name == NULL || source->name == NULL)
                return false;

        fprintf(output, "memcpy(%s, %s, sizeof(TexlerObject));", dest->name,
                source->name);

        return true;
}

static bool generate_variable_assignment_to_constant(FILE *const output,
                                                     variable *dest,
                                                     variable *source)
{
        if (output == NULL || dest == NULL || source == NULL)
                return false;

        if (dest->name == NULL)
                return false;

        fprintf(output, "%s->value", dest->name);
        switch (source->type) {
        case NUMBER_TYPE:
                fprintf(output, ".real = %f;", source->value.number);
                fprintf(output, "%s->type = TYPE_T_REAL;", dest->name);
                break;
        case BOOL_TYPE:
                fprintf(output, ".boolean = %d;", source->value.boolean);
                fprintf(output, "%s->type = TYPE_T_BOOLEAN;", dest->name);
                break;
        case STRING_TYPE:
                fprintf(output, ".string = %s;", source->value.string);
                fprintf(output, ".length = %ld;",
                        strlen(source->value.string));
                fprintf(output, "%s->type = TYPE_T_STRING;", dest->name);
                break;
        default:
                LogDebug("Got variable of type: %d\n"
                         "\tFunction:",
                         source->type, __func__);
                return false;
                break;
        }

        return true;
}

static bool generate_return(FILE *const output, const variable *var)
{
        if (var == NULL) {
                fprintf(output, "return;");
        } else {
                fprintf(output, "return %s;", var->name);
        }

        return true;
}

//  void generate_expression(node_expression *expr)
//  {
//          return;
//  }
//
//
//  void generate_type_code(token_t type)
//  {
//          printf("%s", variable_types[type]);
//  }
//
//  void generate_assign_declaration(node_expression *assign_declaration)
//  {
//          generate_type_code(assign_declaration->var->type);
//          printf(" ");
//          generate_assign(assign_declaration);
//  }
//
//  void generate_assign(node_expression *assignment)
//  {
//          printf("%s = ", assignment->var->name);
//          switch (assignment->type) {
//          case NUMBER_TYPE:
//          case BOOL_TYPE:
//          case STRING_TYPE:
//          case FILE_PATH_TYPE:
//                  generate_variable_value(assignment->var);
//                  break;
//          case VARIABLE_TYPE: // id -> id.
//          case EXPRESSION_TYPE:
//          case EXPRESSION_GRAMMAR_CONSTANT_TYPE:
//          case EXPRESSION_VARIABLE_ASSIGNMENT:
//          case EXPRESSION_VARIABLE_DECLARATION:
//          case EXPRESSION_FILE_DECLARATION:
//                  generate_expression(assignment);
//                  break;
//          default:
//                  error_handler(
//                          "Error - assigning a value or expression to variable");
//          }
//          printf(";\n");
//  }
//
//
//  void generate_variable_value(variable *var)
//  {
//          switch (var->type) {
//          case BOOL_TYPE:
//                  printf("%d", var->boolean);
//                  break;
//          case NUMBER_TYPE:
//                  printf("%f", var->number);
//                  break;
//          case STRING_TYPE:
//                  printf("%s", var->string);
//                  break;
//          case FILE_PATH_TYPE:
//                  printf("%s", var->string); // TODO
//                  break;
//          default:
//                  error_handler("Error - generating variable");
//          }
//  }
//
// //TODO
// void generate_loop(node_expression *loop_node)
// {
//         printf("while(");
//         // condition
//         printf("){\n");
//         // do part()
//         printf("}\n");
//         // printf("for ( %s ;", loop_node->var->id);
// }
//
// void generate_file_handler(node_file_block *block_node)
// {
// }
//
// void our_function_columns()
// {
// }
//
// void our_function_lines()
// {
// }
//
// void our_function_byIndex()
// {
// }
//
// void our_function_filter()
// {
// }
//
// void our_function_toString()
// {
// } // medio gila esta, seria printear el nombre de la funcion
//
// void our_function_at()
// {
// }
//
// static void error_handler(char *msg)
// {
//         perror(msg);
//         exit(1);
// }
//
//
// //    FILE * input_file = fopen("name_file", "r");
// //    FILE * output_file = fopen("name_file_2", "w");
// //
// //    if(input_file == NULL || output_file == NULL) {
// //        error_handler("Error - file handeling failed");
// //    }
// //
// //    int c;
// //    while((c = fgetc(input_file)) != EOF) {
// //        fprintf(output_file, "%c",c);
// //    }
// //
// //    fprintf(output_file, "%s", input_file);
// //
// //
// //    fclose(input_file);
// //    fclose(output_file);
