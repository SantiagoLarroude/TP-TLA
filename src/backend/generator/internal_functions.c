#include <stdio.h>

static void generate_internal_function_open_file();
static void generate_internal_function_copy_buffer_content();
static void generate_internal_function_copy_file_content();
static void generate_internal_function_get_list_of_files_in_dir();
static void generate_internal_function_string_substract();

extern void generate_allocation_error_msg(char* ptr_name);

void generate_internal_functions() 
{
        generate_internal_function_open_file();
        generate_internal_function_copy_buffer_content();
        generate_internal_function_copy_file_content();
        generate_internal_function_get_list_of_files_in_dir();
        generate_internal_function_string_substract();
}

void generate_internal_functions_headers()
{
        printf("bool open_file(const char *name, const char *mode," 
                        "TexlerObject *tex_obj);");
        printf("void copy_buffer_content(char *from, FILE *to);");
        printf("void copy_file_content(FILE *from, FILE *to);");
        printf("long get_list_of_files_in_dir(char ***files, const char *path);");
        printf("char *string_substract(char *str1, char *str2);");
}


static void generate_internal_function_open_file() 
{
        printf("bool open_file("
                        "const char *name," 
                        "const char *mode," 
                        "TexlerObject *tex_obj)"
        "{"
        "if (name == NULL || mode == NULL || tex_obj == NULL)"
        "{"
                "return false;"
        "}"

        "size_t error_msg_len =" 
                "strlen(\"Error while opening file ''\") + strlen(name);"
        "char *error_msg = (char *)calloc(1 + error_msg_len, sizeof(char));");
        generate_allocation_error_msg("error_msg");

        printf(
        "strcpy(error_msg, \"Error while opening file '\");"
        "strcat(error_msg, name);"
        "strcat(error_msg, \"'\");"

        "FILE *fptr = fopen(name, mode);"
        "if (fptr == NULL) {"
                "perror(error_msg);"
                "free(error_msg);"
                "return false;"
        "}"
        "free(error_msg);"

        "rewind(fptr);"

        "tex_obj->type = TYPE_T_FILEPTR;"
        "tex_obj->value.file.stream = fptr;"
        "if (fgetpos(tex_obj->value.file.stream, &tex_obj->value.file.pos)) {"
                "perror(\"Error while getting file position\");"
                "return false;"
        "}"
        "tex_obj->value.file.n_line = 1;"

        "return true;"
        "}");
}

static void generate_internal_function_copy_buffer_content()
{
        printf("void copy_buffer_content(char *from, FILE *to)"
        "{"
                "fputs(from, to);"
        "}");
}


static void generate_internal_function_copy_file_content()
{

        printf("void copy_file_content(FILE *from, FILE *to)"
        "{"
                "char buffer[BUFFER_SIZE] = { 0 };"
                "rewind(from);"
                "while (!feof(from)) {"
                        "if (fgets(buffer, BUFFER_SIZE, from) == NULL)"
                        "{"
                                "break;"
                        "}"
                        "fputs(buffer, to);"
                "}"
        "}");
}

static void generate_internal_function_get_list_of_files_in_dir()
{
        printf("long get_list_of_files_in_dir(char ***files, const char *path)"
        "{"
                "if (files == NULL)"
                "{"
                        "return -1;"
                "}"
        
                "char **files_list = *files;"
        
                "DIR *dir_ptr = NULL;"
                "struct dirent *dir = NULL;"
                "long count_files = 0;"
        
                "dir_ptr = opendir(path);"
        
                "if (dir_ptr)" 
                "{"
                        "while ((dir = readdir(dir_ptr)) != NULL)" 
                        "{"
                                "if (dir->d_type == DT_REG)" 
                                "{"
                                        "count_files++;"
                                        "files_list = "
                                        "(char **)realloc("
                                                "files_list,"
                                                "count_files * sizeof(char *)"
                                        ");"
                                        "files_list[count_files - 1] =" 
                                                "(char *)calloc("
                                                "strlen(path)"
                                                "+ 1"
                                                "+ strlen(dir->d_name)"
                                                "+ 1" 
                                                ","
                                                "sizeof(char));"
                                        "sprintf("
                                                "files_list[count_files - 1]"
                                                ","
                                                "\"%%s/%%s\""
                                                ","
                                                "path, dir->d_name"
                                                ");"
                                "}"
                        "}"
                        "closedir(dir_ptr);"
                "}"
                "*files = files_list;"
                "return count_files;"
        "}");
}


static void generate_internal_function_string_substract()
{
        printf("char *string_substract(char *str1, char *str2)"
        "{"
                "int str1_len = strlen(str1);"
                "int str2_len = strlen(str2);"
        
                "if (str1_len < str2_len)"
                "{"
                        "return str1;"
                "}"
        
                "int i = 0;"
                "int j = str2_len;"
                "while (str2[i] == str1[str1_len - j] && str2_len > i)"
                "{"
                        "i++;"
                        "j--;"
                "}"
        
                "if (str2[i] == str1[str1_len - j] && j < 1)"
                "{"
                        "memset(str1 + str1_len - str2_len, 0, str2_len);"
                        "int aux_len = 1 + strlen(str1);"
                        "char *aux = (char *)realloc(str1, aux_len * sizeof(char));");
                        generate_allocation_error_msg("aux");
                        printf(
                        "aux[aux_len - 1] = '\\0';"
                        "str1 = aux;"
                "}"
                "return str1;"
        "}");
}


