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
                                      node_expression_list *expressions,
                                      const char *working_filename);
static bool generate_expression(FILE *const output,
                                free_function_call_array *frees_stack,
                                node_expression *expr,
                                const char *working_filename);

static bool generate_variable(FILE *const output, variable *var,
                              node_list *separators,
                              free_function_call_array *frees_stack);

static bool generate_variable_assignment(FILE *const output, variable *var,
                                         node_expression *expr);
static bool generate_variable_assignment_to_variable(FILE *const output,
                                                     variable *dest,
                                                     variable *source);
static bool generate_variable_assignment_to_constant(FILE *const output,
                                                     variable *dest,
                                                     variable *source);
static bool generate_variable_assignment_from_number_arithmetic(
        FILE *const output, variable *var, node_expression *operation);

static bool generate_variable_file(FILE *const output, variable *var,
                                   node_list *separators,
                                   free_function_call_array *frees_stack);

static bool generate_expressions_list_with_file(FILE *const output,
                                                node_file_block *fhandler);
static bool generate_loop_expression(FILE *const output, node_loop *loop,
                                     free_function_call_array *frees_stack,
                                     const char *working_filename);
static bool generate_loop_action(FILE *const output, node_loop *loop,
                                 free_function_call_array *frees_stack,
                                 const char *working_filename);
static bool
generate_loop_function_calls_expression(FILE *const output, node_loop *loop,
                                        free_function_call_array *frees_stack,
                                        const char *working_filename);
static bool generate_conditional(FILE *const output,
                                 node_conditional *conditional,
                                 const char *working_filename);

static const char *
generate_number_arithmetic_add(FILE *const output, node_expression *left,
                               node_expression *right,
                               const char *assign_variable_name);
static const char *
generate_number_arithmetic_mul(FILE *const output, node_expression *left,
                               node_expression *right,
                               const char *assign_variable_name);

static bool generate_string_arithmetic_expression(FILE *const output,
                                                  variable *var,
                                                  node_expression *operation);

static bool generate_string_arithmetic_add_expression(node_expression *left,
                                                      node_expression *right,
                                                      variable *var,
                                                      FILE *const output);
static bool generate_string_arithmetic_sub_expression(node_expression *left,
                                                      node_expression *right,
                                                      variable *var,
                                                      FILE *const output);
static const char *generate_number_arithmetic_mul_with_file(
        FILE *const output, node_expression *left, node_expression *right,
        const char *assign_variable_name);

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
        if (output == NULL || ptr_name == NULL)
                return;

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

        fputs("int main(const int argc, const char **argv)", output);
        fputc('{', output);

        if (main_function->args == NULL || main_function->args->len == 0) {
                fprintf(output, "%s();", main_function->name);
        } else {
                //

                free_function_call_array *frees_stack =
                        (free_function_call_array *)calloc(
                                1, sizeof(free_function_call_array));
                if (frees_stack == NULL) {
                        perror("Aborting due to");
                        exit(1);
                }

                for (long n_args = 0; n_args < main_function->args->len;
                     n_args++) {
                        fprintf(output,
                                "TexlerObject * %s = "
                                "(TexlerObject *)"
                                "calloc(1, sizeof(TexlerObject));",
                                main_function->args->exprs[n_args]->var->name);

                        generate_allocation_error_msg(
                                output,
                                main_function->args->exprs[n_args]->var->name);

                        push_free_function_call(
                                frees_stack,
                                main_function->args->exprs[n_args]->var->name,
                                "free_texlerobject");

                        // Caso base: argv[0] es un número
                        // HI: argv[n] es un número
                        // TI: argv[n+1] es un número

                        fprintf(output, "%s->type = TYPE_T_REAL;",
                                main_function->args->exprs[n_args]->var->name);
                        fprintf(output, "%s->value.real = atof(argv[%ld]);",
                                main_function->args->exprs[n_args]->var->name,
                                n_args + 1);

                        // La prueba resulta ser errónea, pero lo revisaremos
                        // en la versión 2 de este paper.
                }

                fprintf(output, "%s(", main_function->name);
                for (long n_args = 0; n_args < main_function->args->len - 1;
                     n_args++) {
                        fprintf(output, "%s",
                                main_function->args->exprs[n_args]->var->name);
                }
                fprintf(output, "%s",
                        main_function->args
                                ->exprs[main_function->args->len - 1]
                                ->var->name);
                fputs(");", output);

                while (frees_stack->size > 0) {
                        free_function_call *ffc =
                                pop_free_function_call(&frees_stack);

                        if (ffc != NULL)
                                fprintf(output, "%s(%s);", ffc->fun,
                                        ffc->name);

                        free_struct_free_function_call(&ffc);
                }
        }
        fputs("return 0;", output);
        fputc('}', output);

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
                        "char *separators;"
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
                        "if ("
                        "tex_obj->value.file.separators != NULL"
                        "&&"
                        "tex_obj->value.file.separators != DEFAULT_SEPARATORS"
                        ")"
                        "{"
                        "free(tex_obj->value.file.separators);"
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
        if (!generate_expressions_list(output, function->expressions, NULL)) {
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
                                      node_expression_list *expressions,
                                      const char *working_filename)
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

        bool gen_expression_return = true;
        while (expressions != NULL && expressions->expr != NULL &&
               gen_expression_return == true) {
                generate_expression(output, frees_stack, expressions->expr,
                                    working_filename);

                expressions = expressions->next;
        }

        /* while (frees_stack->size > 0) {
                free_function_call *ffc = pop_free_function_call(&frees_stack);

                if (ffc != NULL)
                        fprintf(output, "%s(%s);", ffc->fun, ffc->name);

                free_struct_free_function_call(&ffc);
        } */

        free_struct_free_function_call_array(&frees_stack);
        return gen_expression_return;
}

static bool generate_expression(FILE *const output,
                                free_function_call_array *frees_stack,
                                node_expression *expr,
                                const char *working_filename)
{
        if (output == NULL || expr == NULL)
                return false;

        switch (expr->type) {
        case EXPRESSION_VARIABLE_ASSIGNMENT:
                if (!generate_variable_assignment(output, expr->var,
                                                  expr->expr)) {
                        return false;
                }
                break;
        case EXPRESSION_VARIABLE_DECLARATION: /* Fallsthrough */
        case EXPRESSION_FILE_DECLARATION:
                if (!generate_variable(output, expr->var, expr->list_expr,
                                       frees_stack)) {
                        return false;
                }
                break;
        case EXPRESSION_LOOP:
                if (!generate_loop_expression(output, expr->loop_expr,
                                              frees_stack, working_filename)) {
                        return false;
                }
                break;
        case EXPRESSION_FILE_HANDLE:
                if (!generate_expressions_list_with_file(output,
                                                         expr->file_handler)) {
                        return false;
                }
                break;

        case EXPRESSION_CONDITIONAL:
                if (!generate_conditional(output, expr->conditional,
                                          working_filename)) {
                        return false;
                }
                break;
        default:
                LogDebug("Got expression of type: %d\n"
                         "\tFunction:",
                         expr->type, __func__);
                break;
        }

        return true;
}

static bool generate_variable(FILE *const output, variable *var,
                              node_list *separators,
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

        push_free_function_call(frees_stack, var->name, "free_texlerobject");

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
                fprintf(output, "%s->type = TYPE_T_STRING;", var->name);
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
                        fprintf(output, "%s->type = TYPE_T_STRING;",
                                var->name);
                        fprintf(output, "%s->value.string = %s;", var->name,
                                var->value.expr->var->value.string);
                        fprintf(output, "%s->value.length = %ld;", var->name,
                                strlen(var->value.expr->var->value.string));
                        break;
                }
                break;
        case FILE_PATH_TYPE:
                if (separators == NULL)
                        generate_variable_file(output, var, NULL, frees_stack);
                else
                        generate_variable_file(output, var,
                                               separators->exprs[0]->list_expr,
                                               frees_stack);
                break;
        default:
                LogDebug("Got variable type: %d\n"
                         "\tFunction:",
                         var->type, __func__);
        }

        return true;
}

static bool generate_variable_file(FILE *const output, variable *var,
                                   node_list *separators,
                                   free_function_call_array *frees_stack)
{
        if (output == NULL || var == NULL || frees_stack == NULL)
                return false;

        char *frees_string =
                generate_complete_free_function_call_array(frees_stack);

        char *str_separators = NULL;
        if (separators == NULL || separators->exprs == NULL ||
            strcmp(separators->exprs[0]->var->value.string, "") == 0) {
                str_separators = strdup("NULL");
        } else {
                str_separators =
                        strdup(separators->exprs[0]->var->value.string);

                for (size_t i = 1; i < separators->len; i++) {
                        size_t new_size = strlen(str_separators) +
                                          strlen(separators->exprs[i]
                                                         ->var->value.string) +
                                          1;
                        char *aux = (char *)realloc(str_separators,
                                                    sizeof(char) * new_size);
                        if (aux == NULL) {
                                error_no_memory();
                                exit(1);
                        }

                        strcat(aux, separators->exprs[i]->var->value.string);
                        aux[new_size - 1] = '\0';

                        str_separators = aux;
                }
        }

        if (strstr(var->name, "input") == var->name) {
                fprintf(output,
                        "if (open_file(%s, \"r\", %s, %s) == false)"
                        "{",
                        var->value.string, var->name, str_separators);

                if (frees_string != NULL)
                        fprintf(output, "%s", frees_string);

                fprintf(output, "return;"
                                "}");

        } else if (strstr(var->name, "output") == var->name) {
                if (strlen(var->value.string) == 0 ||
                    strcmp(var->value.string, "\"\"") == 0) // Filename: ""
                {
                        size_t stream_str_len = 1 + strlen(var->name) +
                                                strlen("->value.file.stream");
                        char *stream_str =
                                (char *)calloc(stream_str_len, sizeof(char));
                        if (stream_str == NULL) {
                                error_no_memory();
                                exit(1);
                        }
                        snprintf(stream_str, stream_str_len,
                                 "%s->value.file.stream", var->name);

                        fprintf(output, "%s->type = TYPE_T_FILEPTR;",
                                var->name);
                        fprintf(output, "%s->value.file.stream = tmpfile();",
                                var->name);
                        generate_allocation_error_msg(output, stream_str);

                        if (stream_str != NULL)
                                free(stream_str);

                } else if (strcmp(var->value.string, "STDOUT") == 0) {
                        fprintf(output, "%s->type = TYPE_T_FILEPTR;",
                                var->name);
                        fprintf(output, "%s->value.file.stream = stdout;",
                                var->name);
                } else {
                        fprintf(output,
                                "if (open_file(%s, \"w+\", %s, %s) == false)"
                                "{",
                                var->value.string, var->name, str_separators);
                        // if (strcmp(str_separators, "NULL") != 0) {
                        //         change_separators_from_input(str_separators, );
                        // }

                        if (frees_string != NULL)
                                fprintf(output, "%s", frees_string);

                        fprintf(output, "return;"
                                        "}");
                }
        } else {
                error_invalid_file_variable_name(var->name);
                return false;
        }

        if (frees_string != NULL)
                free(frees_string);

        if (str_separators != NULL)
                free(str_separators);

        return true;
}

static bool generate_string_arithmetic_expression(FILE *const output,
                                                  variable *var,
                                                  node_expression *operation)
{
        if (output == NULL || var == NULL || operation == NULL)
                return false;

        if (operation->type == EXPRESSION_STR_ARITHMETIC_ADD) {
                return generate_string_arithmetic_add_expression(
                        operation->left, operation->right, var, output);
        } else {
                return generate_string_arithmetic_sub_expression(
                        operation->left, operation->right, var, output);
        }

        return true;
}

static bool generate_string_arithmetic_add_expression(node_expression *left,
                                                      node_expression *right,
                                                      variable *var,
                                                      FILE *const output)
{
        if (left->type == EXPRESSION_GRAMMAR_CONSTANT_TYPE &&
            right->type == EXPRESSION_GRAMMAR_CONSTANT_TYPE) {
                long aux_len = 1 + strlen(left->var->value.string) +
                               strlen(right->var->value.string);
                char *aux = (char *)calloc(aux_len, sizeof(char));
                if (aux == NULL) {
                        error_no_memory();
                        exit(1);
                }

                strncpy(aux, left->var->value.string, aux_len);
                strncat(aux, right->var->value.string, aux_len);

                if (var->type == FILE_PATH_TYPE) {
                        fprintf(output,
                                "copy_buffer_content("
                                "%s"
                                ","
                                "%s->value.file.stream"
                                ");",
                                aux, var->name);
                } else if (var->type == STRING_TYPE) {
                        fprintf(output, "%s = %s", var->name, aux);
                }

                free(aux);
        } else if (left->type == VARIABLE_TYPE &&
                   left->var->type == FILE_PATH_TYPE) {
                if (var->type != FILE_PATH_TYPE) {
                        LogError("File addition must be stored in a file");
                        return false;
                }

                fprintf(output,
                        "copy_file_content("
                        "%s->value.file.stream"
                        ","
                        "%s->value.file.stream);",
                        left->var->name, var->name);

                if (right->type == VARIABLE_TYPE &&
                    right->var->type == FILE_PATH_TYPE) {
                        fprintf(output,
                                "copy_file_content("
                                "%s->value.file.stream"
                                ","
                                "%s->value.file.stream);",
                                right->var->name, var->name);
                } else if (right->type == EXPRESSION_GRAMMAR_CONSTANT_TYPE) {
                        fprintf(output,
                                "copy_buffer_content("
                                "%s"
                                ","
                                "%s->value.file.stream);",
                                right->var->value.string, var->name);
                } else if (right->type ==
                           VARIABLE_TYPE) { // es ID pero no de tipo file
                        fprintf(output,
                                "copy_buffer_content("
                                "%s"
                                ","
                                "%s->value.file.stream);",
                                right->var->name, var->name);
                }
        } else if (right->type == VARIABLE_TYPE &&
                   right->var->type == FILE_PATH_TYPE) {
                if (var->type != FILE_PATH_TYPE) {
                        LogError("File addition must be stored in a file");
                        return false;
                }

                fprintf(output,
                        "copy_file_content("
                        "%s->value.file.stream"
                        ","
                        "%s->value.file.stream);",
                        right->var->name, var->name);

                if (left->type == EXPRESSION_GRAMMAR_CONSTANT_TYPE) {
                        fprintf(output,
                                "copy_buffer_content("
                                "%s"
                                ","
                                "%s->value.file.stream);",
                                left->var->value.string, var->name);
                } else if (left->type ==
                           VARIABLE_TYPE) { // es ID pero no de tipo file
                        fprintf(output,
                                "copy_buffer_content("
                                "%s"
                                ","
                                "%s->value.file.stream);",
                                left->var->name, var->name);
                }

        } else if (left->type == VARIABLE_TYPE) {
                LogDebug(
                        "Not implemented: string addition with left of type %ld",
                        left->type);
        } else if (right->type == VARIABLE_TYPE) {
                LogDebug(
                        "Not implemented: string addition with right of type %ld",
                        right->type);
        } else {
                LogError(
                        "String addition not posible for types: %ld and %ld\n",
                        left->type, right->type);

                return false;
        }

        return true;
}

static bool generate_string_arithmetic_sub_expression(node_expression *left,
                                                      node_expression *right,
                                                      variable *var,
                                                      FILE *const output)
{
        return true;
}

static bool generate_variable_assignment(FILE *const output, variable *var,
                                         node_expression *expr)
{
        if (output == NULL || var == NULL || expr == NULL)
                return false;

        switch (expr->type) {
        case VARIABLE_TYPE: // ID -> ID.
                if (!generate_variable_assignment_to_variable(output, var,
                                                              expr->var)) {
                        return false;
                }
                break;
        case EXPRESSION_GRAMMAR_CONSTANT_TYPE: // ie: True -> ID.
                if (!generate_variable_assignment_to_constant(output, var,
                                                              expr->var)) {
                        return false;
                }
                break;
        case EXPRESSION_NUMBER_ARITHMETIC_ADD:
        case EXPRESSION_NUMBER_ARITHMETIC_SUB:
        case EXPRESSION_NUMBER_ARITHMETIC_MUL:
        case EXPRESSION_NUMBER_ARITHMETIC_DIV:
        case EXPRESSION_NUMBER_ARITHMETIC_MOD:
                if (!generate_variable_assignment_from_number_arithmetic(
                            output, var, expr)) {
                        return false;
                }
                break;
        case EXPRESSION_STR_ARITHMETIC_ADD:
        case EXPRESSION_STR_ARITHMETIC_SUB:
                if (!generate_string_arithmetic_expression(output, var,
                                                           expr)) {
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

        if (dest->type == FILE_PATH_TYPE && source->type == FILE_PATH_TYPE) {
                fprintf(output, "copy_file_content_texler(%s, %s);",
                        source->name, dest->name);
        } else if (source->type == LOOP_VARIABLE_TYPE) {
                switch (dest->type) {
                case FILE_PATH_TYPE:
                        fprintf(output,
                                "copy_buffer_content("
                                "%s"
                                ","
                                "%s->value.file.stream"
                                ");",
                                source->name, dest->name);
                        break;

                default:
                        fprintf(output,
                                "memcpy(%s, %s, sizeof(TexlerObject));",
                                dest->name, source->name);
                        break;
                }
        } else {
                fprintf(output, "memcpy(%s, %s, sizeof(TexlerObject));",
                        dest->name, source->name);
        }

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

static bool generate_variable_assignment_from_number_arithmetic(
        FILE *const output, variable *var, node_expression *operation)
{
        const char *c_variable_assign_name;
        switch (operation->type) {
        case EXPRESSION_NUMBER_ARITHMETIC_ADD:
                fputc('{', output);
                c_variable_assign_name = generate_number_arithmetic_add(
                        output, operation->left, operation->right, var->name);
                break;
        case EXPRESSION_NUMBER_ARITHMETIC_MUL:
                fputc('{', output);
                c_variable_assign_name = generate_number_arithmetic_mul(
                        output, operation->left, operation->right, var->name);
                break;
        default:
                c_variable_assign_name = NULL;
                break;
        }

        if (c_variable_assign_name != NULL)
                fprintf(output,
                        "fprintf("
                        "%s->value.file.stream, \"%%f\", %s);",
                        var->name, c_variable_assign_name);

        fputc('}', output);
        return true;
}

static bool generate_expressions_list_with_file(FILE *const output,
                                                node_file_block *fhandler)
{
        if (output == NULL || fhandler == NULL)
                return false;

        if (fhandler->var == NULL || fhandler->var->name == NULL ||
            fhandler->var->type != FILE_PATH_TYPE) {
                error_invalid_node_file_handler(__func__);
                return false;
        }

        return generate_expressions_list(output, fhandler->exprs_list,
                                         fhandler->var->name);
}

static bool generate_loop_expression(FILE *const output, node_loop *loop,
                                     free_function_call_array *frees_stack,
                                     const char *working_filename)
{
        if (output == NULL || loop == NULL || loop->iterable == NULL ||
            loop->action == NULL || loop->var == NULL)
                return false;

        switch (loop->iterable->type) {
        case EXPRESSION_LIST:
                if (loop->iterable->list_expr->type == LIST_RANGE_TYPE) {
                // loop->iterable->list_expr
                fprintf(output,
                        "for ("
                        "long %s = %ld - 1;"
                        "%s < %ld;"
                        "%s++)"
                        "{",
                        loop->var->name, loop->iterable->list_expr->from,
                        loop->var->name, loop->iterable->list_expr->to,
                        loop->var->name);

                generate_loop_action(output, loop, frees_stack,
                                     working_filename);

                fprintf(output, "}");
                }
                else {
                        error_loop_iterable_of_invalid_type();
                        return false;
                }

                break;
        case EXPRESSION_FUNCTION_CALL:
                if (!generate_loop_function_calls_expression(
                            output, loop, frees_stack, working_filename))
                        return false;

                break;
        default:
                LogDebug("Got expression of type: %d\n"
                         "\tFunction: %s",
                         loop->iterable->type, __func__);
                break;
        }

        return true;
}

static bool
generate_loop_function_calls_expression(FILE *const output, node_loop *loop,
                                        free_function_call_array *frees_stack,
                                        const char *working_filename)
{
        if (output == NULL || loop == NULL)
                return false;

        bool loop_action_generated =
                false; // Can be called from 'inside' an iterable generation

        size_t closing_braces = 0;
        size_t concat_functions = 1;
        node_function_call *fn_calls = loop->iterable->fun_call; // Alias

        while (fn_calls != NULL && fn_calls->next != NULL) {
                fn_calls = fn_calls->next;
                concat_functions++;
        }

        while (concat_functions > 0) {
                if (strcmp(fn_calls->id->name, "lines") == 0) {
                        fprintf(output, "long _line_len_implementation"
                                        "="
                                        "BUFFER_SIZE;");
                        fprintf(output,
                                "char * %s = "
                                "(char *)"
                                "calloc("
                                "_line_len_implementation,"
                                "sizeof(char)"
                                ");",
                                loop->var->name);
                        generate_allocation_error_msg(output, loop->var->name);

                        if (fn_calls->next != NULL &&
                            strcmp(fn_calls->next->id->name, "byIndex") == 0) {
                                switch (fn_calls->next->args->exprs[0]->type) {
                                case EXPRESSION_GRAMMAR_CONSTANT_TYPE:
                                        fprintf(output,
                                                "_line_len_implementation ="
                                                "line_by_number(input"
                                                ","
                                                "&%s"
                                                ","
                                                "%ld"
                                                ");",
                                                loop->var->name,
                                                (long)fn_calls->next->args
                                                        ->exprs[0]
                                                        ->var->value.number);
                                        fprintf(output,
                                                "if ("
                                                "_line_len_implementation <= 0"
                                                "||"
                                                "%s == NULL"
                                                ")"
                                                "{"
                                                "return;"
                                                "}",
                                                loop->var->name);
                                        break;
                                case LIST_RANGE_TYPE:
                                        fprintf(output,
                                                "_line_len_implementation = "
                                                "lines(%s, &%s);",
                                                working_filename,
                                                loop->var->name);
                                        fprintf(output,
                                                "if ("
                                                "_line_len_implementation <= 0"
                                                "||"
                                                "%s == NULL"
                                                ")"
                                                "{"
                                                "break;"
                                                "}",
                                                loop->var->name);

                                        break;
                                default:
                                        error_invalid_byIndex_argument();
                                        return false;
                                }

                        } else if (fn_calls->next != NULL &&
                                   fn_calls->next->args != NULL &&
                                   strcmp(fn_calls->next->id->name,
                                          "filter") == 0) {
                                fprintf(output,
                                        "rewind(%s->"
                                        "value.file.stream);"
                                        "while (_line_len_implementation > 0)"
                                        "{",
                                        working_filename);
                                fprintf(output,
                                        "_line_len_implementation = "
                                        "lines(%s, &%s);",
                                        working_filename, loop->var->name);

                                closing_braces++;

                                fprintf(output, "if (is_in_string(%s, %s)) {",
                                        fn_calls->next->args->exprs[0]
                                                ->var->value
                                                .string, // "palabra"
                                        loop->var->name

                                );
                                closing_braces++;
                        }
                } else if (strcmp(fn_calls->id->name, "columns") == 0) {
                        if (fn_calls->next == NULL) {
                                LogError("columns() not fully implemented");
                        } else if (fn_calls->next != NULL &&
                                   strcmp(fn_calls->next->id->name, "lines") ==
                                           0) {
                                char *original_loop_var_name =
                                        strdup(loop->var->name);

                                long new_loop_var_name_len =
                                        1 + strlen(original_loop_var_name) +
                                        strlen("_") +
                                        strlen("_columns_implementation");
                                loop->var->name = (char *)realloc(
                                        loop->var->name,
                                        sizeof(char) * new_loop_var_name_len);
                                if (loop->var->name == NULL) {
                                        error_no_memory();
                                        exit(1);
                                }

                                strncpy(loop->var->name, "_",
                                        new_loop_var_name_len);
                                strncat(loop->var->name,
                                        original_loop_var_name,
                                        new_loop_var_name_len);
                                strncat(loop->var->name,
                                        "_columns_implementation",
                                        new_loop_var_name_len);

                                fprintf(output,
                                        "char *_columns_remaining_implementation"
                                        "= %s;"
                                        "long _columns_len_implementation = "
                                        "BUFFER_SIZE;"
                                        "char *%s = "
                                        "(char *)"
                                        "calloc(_columns_len_implementation,"
                                        "sizeof(char));",
                                        original_loop_var_name,
                                        loop->var->name);
                                generate_allocation_error_msg(output,
                                                              loop->var->name);

                                fprintf(output,
                                        "while ("
                                        "_columns_remaining_implementation != NULL"
                                        ")"
                                        "{");
                                closing_braces++;

                                fprintf(output,
                                        "long"
                                        " "
                                        "_columns_separator_char_implementation"
                                        "= 0;");
                                fprintf(output,
                                        "_columns_len_implementation ="
                                        "columns("
                                        "&_columns_remaining_implementation"
                                        ",");

                                fprintf(output,
                                        "NULL" // TODO: Separators
                                        ",");

                                fprintf(output,
                                        "&%s"
                                        ","
                                        "&_columns_separator_char_implementation"
                                        ");",
                                        loop->var->name);

                                fprintf(output,
                                        "if ("
                                        "%s != NULL"
                                        "&&"
                                        "_columns_len_implementation > 0"
                                        ")"
                                        "{",
                                        loop->var->name);
                                closing_braces++;

                                generate_loop_action(output, loop, frees_stack,
                                                     working_filename);
                                loop_action_generated = true;

                                fprintf(output,
                                        "if ("
                                        "_columns_separator_char_implementation"
                                        ")"
                                        "{"
                                        "fputc("
                                        "_columns_separator_char_implementation"
                                        ","
                                        "output->value.file.stream" // TODO: output
                                        ");"
                                        "}");

                                // Restore variable name
                                loop->var->name = (char *)realloc(
                                        loop->var->name,
                                        sizeof(char) *
                                                (1 +
                                                 strlen(original_loop_var_name)));
                                if (loop->var->name == NULL) {
                                        error_no_memory();
                                        exit(1);
                                }

                                strncpy(loop->var->name,
                                        original_loop_var_name,
                                        1 + strlen(original_loop_var_name));

                                free(original_loop_var_name);
                        }
                } else if (strcmp(fn_calls->id->name, "byIndex") == 0) {
                        switch (fn_calls->args->exprs[0]->type) {
                        case EXPRESSION_GRAMMAR_CONSTANT_TYPE:
                                if (fn_calls->args->exprs[0]->var->type !=
                                    NUMBER_TYPE) {
                                        error_invalid_byIndex_argument();
                                        return false;
                                }

                                break;
                        case LIST_RANGE_TYPE:
                                fprintf(output,
                                        "for (long %s = %ld - 1;"
                                        "%s < %ld;"
                                        "%s++)"
                                        "{",
                                        "_byIndex_implementation",
                                        fn_calls->args->exprs[0]
                                                ->list_expr->from,
                                        "_byIndex_implementation",
                                        fn_calls->args->exprs[0]->list_expr->to,
                                        "_byIndex_implementation");

                                closing_braces++;
                                break;
                        default:
                                error_invalid_byIndex_argument();
                                return false;
                        }
                } else if (strcmp(fn_calls->id->name, "filter") == 0) {
                        // fprintf(output,
                        //         "// rewind(el nombre de input flechita "
                        //         "value.file.stream);\n"
                        //         "while (_line_len_implementation > 0)"
                        //         "{");

                        // closing_braces++;
                } else {
                        LogError("Not implemented for function: %s\n"
                                 "\tFunction: %s",
                                 fn_calls->id->name, __func__);
                        return false;
                }

                fn_calls = fn_calls->prev;
                concat_functions--;
        }

        if (loop_action_generated == false)
                generate_loop_action(output, loop, frees_stack,
                                     working_filename);

        while (closing_braces > 0) {
                fprintf(output, "}");
                closing_braces--;
        }

        return true;
}

static bool generate_loop_action(FILE *const output, node_loop *loop,
                                 free_function_call_array *frees_stack,
                                 const char *working_filename)
{
        generate_expression(output, frees_stack, loop->action,
                            working_filename);

        return true;
}

static bool generate_conditional(FILE *const output,
                                 node_conditional *conditional,
                                 const char *working_filename)
{
        switch (conditional->condition->type) {
        case EXPRESSION_VARIABLE_TYPE_COMPARISON:
                switch (conditional->condition->compare_type) {
                case NUMBER_TYPE:
                        fprintf(output,
                                "IS_NUMBER_RETURN _isnum ="
                                "is_number("
                                "%s"
                                ","
                                "strlen(%s)"
                                ");",
                                conditional->condition->var->name,
                                conditional->condition->var->name);

                        fprintf(output, "if ("
                                        "_isnum == IS_NUMBER_RETURN_FLOATING"
                                        "||"
                                        "_isnum == IS_NUMBER_RETURN_INTEGER"
                                        ")"
                                        "{");
                        generate_expression(output, NULL,
                                            conditional->true_condition,
                                            working_filename);
                        fputc('}', output);
                        break;
                case BOOL_TYPE: /* Fallsthrough */
                case STRING_TYPE:
                case FILE_PATH_TYPE:
                case LOOP_VARIABLE_TYPE:
                case VARIABLE_TYPE:
                case FUNCTION_TYPE:
                case CONSTANT_TYPE:
                        LogError("Type comparison not implemented for "
                                 "this type (%ld)",
                                 conditional->condition->compare_type);

                        return false;
                default:
                        LogError("Comparison condition of type: %ld",
                                 conditional->condition->compare_type);
                        break;
                }
                break;
        default:
                break;
        }

        fprintf(output, "else"
                        "{");
        generate_expression(output, NULL, conditional->else_condition,
                            working_filename);
        fputc('}', output);

        return true;
}

static const char *
generate_number_arithmetic_add(FILE *const output, node_expression *left,
                               node_expression *right,
                               const char *assign_variable_name)
{
        const char *var_name = "_add_arithm_n";
        fprintf(output, "double %s = 0;", var_name);

        switch (left->type) {
        case EXPRESSION_GRAMMAR_CONSTANT_TYPE:
                fprintf(output, "%s += %f;", var_name,
                        left->var->value.number);
                break;
        case VARIABLE_TYPE:
                fprintf(output, "%s += atof(%s);", var_name, left->var->name);
                break;
        default:
                LogError("Arithmetic addition not implemented for type: %ld",
                         left->type);
                break;
        }

        switch (right->type) {
        case EXPRESSION_GRAMMAR_CONSTANT_TYPE:
                fprintf(output, "%s += %f;", var_name,
                        right->var->value.number);
                break;
        case VARIABLE_TYPE:
                fprintf(output, "%s += atof(%s);", var_name, right->var->name);
                break;
        default:
                LogError("Arithmetic addition not implemented for type: "
                         "%ld",
                         right->type);
                break;
        }

        return var_name;
}

static const char *
generate_number_arithmetic_mul(FILE *const output, node_expression *left,
                               node_expression *right,
                               const char *assign_variable_name)
{
        if ((left->type == VARIABLE_TYPE &&
             left->var->type == FILE_PATH_TYPE) ||
            (right->type == VARIABLE_TYPE &&
             right->var->type == FILE_PATH_TYPE)) {
                return generate_number_arithmetic_mul_with_file(
                        output, left, right, assign_variable_name);
        }

        const char *var_name = "_mul_arithm_n";

        fprintf(output, "double %s = 1;", var_name);

        switch (left->type) {
        case EXPRESSION_GRAMMAR_CONSTANT_TYPE:
                fprintf(output, "%s *= %f;", var_name,
                        left->var->value.number);
                break;
        case VARIABLE_TYPE:
                fprintf(output, "%s *= atof(%s);", var_name, left->var->name);
                break;
        default:
                LogError("Arithmetic multiplication not implemented for type: "
                         "%ld",
                         left->type);
                break;
        }

        switch (right->type) {
        case EXPRESSION_GRAMMAR_CONSTANT_TYPE:
                fprintf(output, "%s *= %f;", var_name,
                        right->var->value.number);
                break;
        case VARIABLE_TYPE:
                fprintf(output, "%s *= atof(%s);", var_name, right->var->name);
                break;
        default:
                LogError("Arithmetic addition not implemented for type: %ld",
                         right->type);
                break;
        }

        return var_name;
}

static const char *generate_number_arithmetic_mul_with_file(
        FILE *const output, node_expression *left, node_expression *right,
        const char *assign_variable_name)
{
        const char *var_name = "_mul_arithm_n";
        node_expression *file_id = NULL;
        node_expression *variable_id = NULL;

        if (left->var->type == FILE_PATH_TYPE) {
                file_id = left;
                variable_id = right;
        } else {
                file_id = right;
                variable_id = left;
        }

        if (variable_id->type == VARIABLE_TYPE) {
                fprintf(output,
                        "if("
                        "%s->type != TYPE_T_REAL"
                        "&&"
                        "%s->type != TYPE_T_INTEGER"
                        ")"
                        "{"
                        "fprintf(stderr, \"Invalid object type in %s.\\n\");"
                        "return;"
                        "}",
                        variable_id->var->name, variable_id->var->name,
                        variable_id->var->name);
        }

        fprintf(output, "long %s = 0;", var_name);

        if (variable_id->type == VARIABLE_TYPE) {
                fprintf(output, "%s = %s->value.real;", var_name,
                        variable_id->var->name);
        } else if (variable_id->type == EXPRESSION_GRAMMAR_CONSTANT_TYPE &&
                   variable_id->var->type == NUMBER_TYPE) {
                fprintf(output, "%s = %f;", var_name,
                        variable_id->var->value.number);
        } else {
                error_invalid_multiplication_type();
                return false;
        }

        fprintf(output,
                "while (%s > 0)"
                "{"
                "copy_file_content("
                "%s->value.file.stream"
                ","
                "%s->value.file.stream"
                ");"
                "%s--;"
                "}",
                var_name, file_id->var->name, assign_variable_name, var_name);

        return NULL;
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
