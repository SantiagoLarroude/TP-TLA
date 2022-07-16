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
                              free_function_call_array *frees_stack);

static bool generate_variable_assignment(FILE *const output, variable *var,
                                         node_expression *expr);
static bool generate_variable_assignment_to_variable(FILE *const output,
                                                     variable *dest,
                                                     variable *source);
static bool generate_variable_assignment_to_constant(FILE *const output,
                                                     variable *dest,
                                                     variable *source);
static bool generate_variable_file(FILE *const output, variable *var,
                                   free_function_call_array *frees_stack);
static bool generate_expressions_list_with_file(FILE *const output,
                                                node_file_block *fhandler);
static bool generate_loop_expression(FILE *const output, node_loop *loop,
                                     free_function_call_array *frees_stack,
                                     const char *working_filename);

static bool
generate_loop_function_calls_expression(FILE *const output, node_loop *loop,
                                        free_function_call_array *frees_stack,
                                        const char *working_filename);

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
                if (!generate_variable(output, expr->var, frees_stack)) {
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
                // case EXPRESSION_STR_ARITHMETIC_ADD:
                //         if (!generate_string_arithmetic_expression(
                //                     output, EXPRESSION_STR_ARITHMETIC_ADD, expr->left,
                //                     expr->right)) {
                //                 return false;
                //         }
                //         break;
                // case EXPRESSION_STR_ARITHMETIC_SUB:
                //         if (!generate_string_arithmetic_expression(
                //                     output, EXPRESSION_STR_ARITHMETIC_SUB, expr->left,
                //                     expr->right)) {
                //                 return false;
                //         }
                //         break;

        default:
                LogDebug("Got expression of type: %d\n"
                         "\tFunction:",
                         expr->type, __func__);
                break;
        }

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
                generate_variable_file(output, var, frees_stack);
                break;
        default:
                LogDebug("Got variable type: %d\n"
                         "\tFunction:",
                         var->type, __func__);
        }

        return true;
}

static bool generate_variable_file(FILE *const output, variable *var,
                                   free_function_call_array *frees_stack)
{
        if (output == NULL || var == NULL || frees_stack == NULL)
                return false;

        char *frees_string =
                generate_complete_free_function_call_array(frees_stack);

        if (strstr(var->name, "input") == var->name) {
                fprintf(output,
                        "if (open_file(%s, \"r\", %s) == false)"
                        "{",
                        var->value.string, var->name);

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
                                "if (open_file(%s, \"w+\", %s) == false)"
                                "{",
                                var->value.string, var->name);

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

        return true;
}

static bool generate_string_arithmetic_expression(FILE *const output,
                                                  int operation,
                                                  node_expression *left,
                                                  node_expression *right)
{
        if (output == NULL || left == NULL || right == NULL)
                return false;

        // chequear si van en el switch, creo que no
        // case EXPRESSION_VARIABLE:
        //      grammar_concat_list_args_with_id
        //      grammar_new_list_args_from_id
        //      grammar_concat_list_args_with_id
        // case EXPRESSION_VARIABLE_DECLARATION:
        // case EXPRESSION_VARIABLE_TYPE_COMPARISON:
        // case EXPRESSION_VARIABLE_ASSIGNMENT:

        if ((left->type == VARIABLE_TYPE ||
             left->type == EXPRESSION_GRAMMAR_CONSTANT_TYPE) &&
            (right->type == VARIABLE_TYPE ||
             right->type == EXPRESSION_GRAMMAR_CONSTANT_TYPE)) {
                if (operation == EXPRESSION_STR_ARITHMETIC_SUB) {
                        fprintf(output, "string_substract(%s, %s);",
                                left->var->name, right->var->name);
                } else {
                        fprintf(output, "");
                }
        }

        switch (left->type) {
        case VARIABLE_TYPE: // ID ++
        case EXPRESSION_GRAMMAR_CONSTANT_TYPE: // string_constant ++

                break;
        default:
                break;
        }
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
        case EXPRESSION_STR_ARITHMETIC_ADD:
                if (!generate_string_arithmetic_expression(
                            output, EXPRESSION_STR_ARITHMETIC_ADD, expr->left,
                            expr->right)) {
                        return false;
                }
                break;
        case EXPRESSION_STR_ARITHMETIC_SUB:
                if (!generate_string_arithmetic_expression(
                            output, EXPRESSION_STR_ARITHMETIC_SUB, expr->left,
                            expr->right)) {
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
                fprintf(output,
                        "copy_file_content("
                        "%s->value.file.stream"
                        ","
                        "%s->value.file.stream"
                        ");",
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
        case LIST_RANGE_TYPE:
                // TODO
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

        size_t closing_braces = 0;
        size_t concat_functions = 1;
        node_function_call *fn_calls = loop->iterable->fun_call; // Alias

        while (fn_calls->next != NULL) {
                fn_calls = fn_calls->next;
                concat_functions++;
        }

        // while (concat_functions > 0) {
        //         if (concat_functions >= 2 &&
        //                 strcmp(fn_calls->id->name, "lines") == 0) {
        //                 fprintf(output, "long _line_len_implementation"
        //                                 "="
        //                                 "BUFFER_SIZE;");
        //                 fprintf(output,
        //                         "char * %s = "
        //                         "(char *)"
        //                         "calloc("
        //                         "_line_len_implementation,"
        //                         "sizeof(char)"
        //                         ");",
        //                         loop->var->name);
        //                 generate_allocation_error_msg(output, loop->var->name);
        //                 fprintf(output,
        //                         "_line_len_implementation = "
        //                         "lines(%s, &%s);",
        //                         working_filename, loop->var->name);
        //         }else if (concat_functions == 1 && 0 == strcmp()) {}

        //         fn_calls = fn_calls->prev;
        //         concat_functions--;
        // }

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
                        // fprintf(output,
                        //         "_line_len_implementation = "
                        //         "lines(%s, &%s);",
                        //         working_filename, loop->var->name);

                        if (fn_calls->next != NULL &&
                            strcmp(fn_calls->next->id->name, "byIndex") == 0) {
                                fprintf(output,
                                        "_line_len_implementation = "
                                        "lines(%s, &%s);",
                                        working_filename, loop->var->name);

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
                        }

                        if (fn_calls->next != NULL &&
                            fn_calls->next->args != NULL &&
                            strcmp(fn_calls->next->id->name, "filter") == 0) {
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
                        //
                } else if (strcmp(fn_calls->id->name, "byIndex") == 0) {
                        fprintf(output,
                                "for (long %s = %ld - 1;"
                                "%s < %ld;"
                                "%s++)"
                                "{",
                                "_byIndex_implementation",
                                fn_calls->args->exprs[0]->list_expr->from,
                                "_byIndex_implementation",
                                fn_calls->args->exprs[0]->list_expr->to,
                                "_byIndex_implementation");

                        closing_braces++;
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

        generate_expression(output, frees_stack, loop->action,
                            working_filename);

        while (closing_braces > 0) {
                fprintf(output, "}");
                closing_braces--;
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
