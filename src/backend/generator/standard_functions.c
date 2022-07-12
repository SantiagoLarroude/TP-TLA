#include <stdio.h>

static void generate_std_function_is_number();
static void generate_std_function_lines();
static void generate_std_function_columns();
static void generate_std_function_is_in_string();

extern void generate_allocation_error_msg(char* ptr_name);

void generate_standard_functions()
{
        generate_std_function_is_number();
        generate_std_function_lines();
        generate_std_function_columns();
        generate_std_function_is_in_string();
}


void generate_standard_functions_headers()
{
        printf("typedef enum {"
               "IS_NUMBER_RETURN_NAN = 0,"
               "IS_NUMBER_RETURN_INTEGER,"
               "IS_NUMBER_RETURN_FLOATING,"
               "N_IS_NUMBER_RETURN"
               "} IS_NUMBER_RETURN;\n");

        printf("IS_NUMBER_RETURN is_number(char *str, long int n);\n");
        printf("long int line_length(FILE *stream, const fpos_t *pos);\n");
        printf("long int columns(char **str, char *separators, char **buffer, int *separator)\n;");
        printf("bool is_in_string(char *str, char *line);\n");
}

static void generate_std_function_is_number()
{
        /*
         * Return: NAN if is not a number
         *         TYPE_T_INTEGER if it is an integer
         *         FLOATING if it is a floating point number
         */
        printf("IS_NUMBER_RETURN is_number(char *str, long int n)"
        "{"
                "if (str == NULL)"
                "{"
                        "return IS_NUMBER_RETURN_NAN;"
                "}"
        
                "IS_NUMBER_RETURN ret_value = IS_NUMBER_RETURN_INTEGER;"
        
                "for (int i = 0; i < n; i++)"
                "{"
                        "int c = 0;"
                        "c = str[i];"
                        "if (c == '.')"
                        "{"
                                "ret_value = IS_NUMBER_RETURN_FLOATING;"
                        "} "
                        "else if (isdigit(c) == 0)"
                        "{"
                                "ret_value = IS_NUMBER_RETURN_NAN;"
                                "break;"
                        "}"
                "}"
                "return ret_value;"
        "}");
}

static void generate_std_function_lines()
{
        printf("long int line_length(FILE *stream, const fpos_t *pos)"
        "{"
                "long int start = ftell(stream);"
                "long int end = 0;"
                "int c = 0;"
                "do" 
                "{"
                        "c = fgetc(stream);"
                "}" 
                "while (c != EOF && c != '\\n');"
                "end = ftell(stream);"
        
                "if (fsetpos(stream, pos))"
                "{"
                        "perror(\"Error while getting file position\");"
                        "return false;"
                "}"
        
                "if (end == 0)"
                "{"
                        "return 0;"
                "}"
        
                "return end - start;"
        "}");

/*
 * Set file position in tex_obj to the next line
 * Return current line in buffer (uses realloc internally) and its new size 
 */
        printf(
        "long int lines(TexlerObject *tex_obj, char **buffer)"
        "{"
                "if (tex_obj == NULL || buffer == NULL || *buffer == NULL)"
                "{"
                        "return 0;"
                "}"
        
                "if (tex_obj->type != TYPE_T_FILEPTR ||"
                    "tex_obj->value.file.stream == NULL)"
                "{"
                        "return 0;"
                "}"
        
                "long int new_size = 1 + line_length(tex_obj->value.file.stream,"
                                                    "&tex_obj->value.file.pos);"
                "/* EOF reached */"
                "if (new_size == 1)"
                "{"
                        "return 0;"
                "}"
        
                "char *new_buffer = (char *)realloc(*buffer, new_size);");
                generate_allocation_error_msg("new_buffer");
                printf("new_buffer[new_size - 1] = '\\0';"
        
                "fgets(new_buffer, new_size, tex_obj->value.file.stream);"
        
                "if (fgetpos(tex_obj->value.file.stream, &tex_obj->value.file.pos))"
                "{"
                        "perror(\"Error while getting file position\");"
                        "return false;"
                "}"
                "tex_obj->value.file.n_line++;"
        
                "*buffer = new_buffer;"
        
                "return new_size;"
        "}"
        );
}

static void generate_std_function_columns()
{
        /*
         * str: String to find column
         * separators: String with column separators (if NULL DEFAULT_SEPARATORS are 
         *              used)
         */
        printf("long int columns(char **str, char *separators, char **buffer, int *separator)"
        "{"
                "char *seps = separators;"
                "if (separators == NULL)"
                "{"
                        "seps = (char *)DEFAULT_SEPARATORS;"
                "}"
        
                "char *column_last_character = strpbrk(*str, seps);"
        
                "/* Value to return in *str */"
                "char *next_column = column_last_character;"
        
                "long int new_size = 1; /* '\\0' */"
                "if (column_last_character == NULL)"
                "{"
                        "new_size += strlen(*str);"
                "}" 
                "else" 
                "{"
                        "new_size += (column_last_character - *str);"
                "}"
        
                "char *new_buffer = (char *)realloc(*buffer, new_size);");
                generate_allocation_error_msg("new_buffer");
                printf(
                "new_buffer[new_size - 1] = '\\0';"
        
                "strncpy(new_buffer, *str, new_size - 1);"
        
                "/* buffer contains '\\0' */"
                "if (new_size == 1)"
                "{"
                        "new_size = 0;"
                "}"
        
                "*buffer = new_buffer;"
        
                "if (next_column != NULL && *next_column != '\\0' &&"
                    "strchr(seps, *next_column) != NULL)"
                "{"
                        "*separator = *next_column++;"
                "}"
        
                "*str = (next_column == NULL || *next_column == '\\0')" 
                        "? NULL : next_column;"
        
                "return new_size;"
        "}");

}

static void generate_std_function_is_in_string()
{
        printf("bool is_in_string(char *str, char *line)"
        "{"
                "char *aux = strstr(line, str);"
        
                "return (aux == NULL) ? false : true;"
        "}");
}

