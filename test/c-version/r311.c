#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 256

const char *DEFAULT_SEPARATORS = " ,";

typedef enum {
        TYPE_T_NONE = 0,
        TYPE_T_BOOLEAN,
        TYPE_T_STRING,
        TYPE_T_REAL,
        TYPE_T_INTEGER,
        TYPE_T_FILEPTR,
        TYPE_T_FILE_LIST,
        N_TYPE_T
} type_t;

typedef enum {
        IS_NUMBER_RETURN_NAN = 0,
        IS_NUMBER_RETURN_INTEGER,
        IS_NUMBER_RETURN_FLOATING,
        N_IS_NUMBER_RETURN
} IS_NUMBER_RETURN;

typedef enum {
        IS_SEPARATOR_RETURN_NOT_MATCH = 0,
        IS_SEPARATOR_RETURN_MATCH,
} IS_SEPARATOR_RETURN;

typedef struct TexlerObject TexlerObject;

struct TexlerObject {
        union {
                bool boolean;
                struct {
                        char *string;
                        size_t length;
                };
                double real;
                long integer;
                struct {
                        FILE *stream;
                        fpos_t pos;

                        char **path_list;
                        union {
                                size_t n_line;
                                size_t n_files;
                        };
                } file;
        } value;
        type_t type;
};

void free_texlerobject(TexlerObject *tex_obj)
{
        if (tex_obj == NULL) {
                return;
        }

        switch (tex_obj->type) {
        case TYPE_T_FILEPTR:
                if (tex_obj->value.file.stream != NULL &&
                    tex_obj->value.file.stream != stdout &&
                    tex_obj->value.file.stream != stderr &&
                    tex_obj->value.file.stream != stdin) {
                        fclose(tex_obj->value.file.stream);
                }
                break;
        case TYPE_T_FILE_LIST:
                if (tex_obj->value.file.path_list != NULL) {
                        while (tex_obj->value.file.n_files > 0) {
                                if (tex_obj->value.file.path_list
                                            [tex_obj->value.file.n_files - 1] !=
                                    NULL) {
                                        free(tex_obj->value.file.path_list
                                                     [tex_obj->value.file.n_files -
                                                      1]);
                                }

                                tex_obj->value.file.n_files--;
                        }
                        free(tex_obj->value.file.path_list);
                }
                break;
        case TYPE_T_STRING:
                if (tex_obj->value.string != NULL) {
                        free(tex_obj->value.string);
                }
                break;
        case TYPE_T_BOOLEAN: /* Fallsthrough*/
        case TYPE_T_REAL:
        case TYPE_T_INTEGER:
                break;
        default:
                break;
        }

        memset(tex_obj, 0, sizeof(TexlerObject));
        free(tex_obj);
}

bool open_file(const char *name, const char *mode, TexlerObject *tex_obj)
{
        if (name == NULL || mode == NULL || tex_obj == NULL)
                return false;

        size_t error_msg_len =
                strlen("Error while opening file ''") + strlen(name);
        char *error_msg = (char *)calloc(1 + error_msg_len, sizeof(char));
        if (error_msg == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        strcpy(error_msg, "Error while opening file '");
        strcat(error_msg, name);
        strcat(error_msg, "'");

        FILE *fptr = fopen(name, mode);
        if (fptr == NULL) {
                perror(error_msg);
                free(error_msg);
                return false;
        }
        free(error_msg);

        rewind(fptr);

        tex_obj->type = TYPE_T_FILEPTR;
        tex_obj->value.file.stream = fptr;
        if (fgetpos(tex_obj->value.file.stream, &tex_obj->value.file.pos)) {
                perror("Error while getting file position");
                return false;
        }
        tex_obj->value.file.n_line = 1;

        return true;
}

void copy_buffer_content(char *from, FILE *to)
{
        fputs(from, to);
}

void copy_file_content(FILE *from, FILE *to)
{
        char buffer[BUFFER_SIZE] = { 0 };

        rewind(from);

        while (!feof(from)) {
                if (fgets(buffer, BUFFER_SIZE, from) == NULL)
                        break;

                fputs(buffer, to);
        }
}

/* Devuelve la cantidad de archivos en la carpeta path
 * Files es un arreglo con el path de cada archivo
 */
long get_list_of_files_in_dir(char ***files, const char *path)
{
        if (files == NULL)
                return -1;

        char **files_list = *files;

        DIR *dir_ptr = NULL;
        struct dirent *dir = NULL;
        long count_files = 0;

        dir_ptr = opendir(path);

        if (dir_ptr) {
                while ((dir = readdir(dir_ptr)) != NULL) {
                        // Condition to check regular file.
                        if (dir->d_type == DT_REG) {
                                count_files++;
                                files_list = (char **)realloc(
                                        files_list,
                                        count_files * sizeof(char *));
                                files_list[count_files - 1] = (char *)calloc(
                                        strlen(path) + 1 + strlen(dir->d_name) +
                                                1,
                                        sizeof(char));
                                sprintf(files_list[count_files - 1], "%s/%s",
                                        path, dir->d_name);
                        }
                }
                closedir(dir_ptr);
        }

        *files = files_list;
        return count_files;
}

// Substract str2 from the end of str1
char *string_substract(char *str1, char *str2)
{
        int str1_len = strlen(str1);
        int str2_len = strlen(str2);

        if (str1_len < str2_len)
                return str1;

        int i = 0;
        int j = str2_len;
        while (str2[i] == str1[str1_len - j] && str2_len > i) {
                i++;
                j--;
        }

        if (str2[i] == str1[str1_len - j] && j < 1) {
                memset(str1 + str1_len - str2_len, 0, str2_len);

                int aux_len = 1 + strlen(str1);
                char *aux = (char *)realloc(str1, aux_len * sizeof(char));
                if (aux == NULL) {
                        perror("Aborting due to");
                        exit(1);
                }

                aux[aux_len - 1] = '\0';

                str1 = aux;
        }

        return str1;
}

/*
 * Return: NAN if is not a number
 *         TYPE_T_INTEGER if it is an integer
 *         FLOATING if it is a floating point number
 */
IS_NUMBER_RETURN is_number(char *str, long int n)
{
        if (str == NULL)
                return IS_NUMBER_RETURN_NAN;

        IS_NUMBER_RETURN ret_value = IS_NUMBER_RETURN_INTEGER;
        int c = 0;
        for (int i = 0; i < n; i++) {
                c = str[i];
                if (c == '.') {
                        ret_value = IS_NUMBER_RETURN_FLOATING;
                } else if (isdigit(c) == 0) {
                        ret_value = IS_NUMBER_RETURN_NAN;
                        break;
                }
        }

        return ret_value;
}

long int line_length(FILE *stream, const fpos_t *pos)
{
        long int start = ftell(stream);
        long int end = 0;
        int c = 0;
        do {
                c = fgetc(stream);
        } while (c != EOF && c != '\n');
        end = ftell(stream);

        if (fsetpos(stream, pos)) {
                perror("Error while getting file position");
                return false;
        }

        if (end == 0)
                return 0;

        return end - start;
}

/*
 * Set file position in tex_obj to the next line
 * Return current line in buffer (uses realloc internally) and its new size 
 */
long int lines(TexlerObject *tex_obj, char **buffer)
{
        if (tex_obj == NULL || buffer == NULL || *buffer == NULL)
                return 0;

        if (tex_obj->type != TYPE_T_FILEPTR ||
            tex_obj->value.file.stream == NULL)
                return 0;

        long int new_size = 1 + line_length(tex_obj->value.file.stream,
                                            &tex_obj->value.file.pos);
        // EOF reached
        if (new_size == 1)
                return 0;

        char *new_buffer = (char *)realloc(*buffer, new_size);
        if (new_buffer == NULL) {
                perror("Aborting due to");
                exit(1);
        }
        new_buffer[new_size - 1] = '\0';

        fgets(new_buffer, new_size, tex_obj->value.file.stream);

        if (fgetpos(tex_obj->value.file.stream, &tex_obj->value.file.pos)) {
                perror("Error while getting file position");
                return false;
        }
        tex_obj->value.file.n_line++;

        *buffer = new_buffer;

        return new_size;
}

/*
 * str: String to find column
 * separators: String with column separators (if NULL DEFAULT_SEPARATORS are 
 *              used)
 */
long int columns(char **str, char *separators, char **buffer, int *separator)
{
        char *seps = separators;
        if (separators == NULL)
                seps = (char *)DEFAULT_SEPARATORS;

        char *column_last_character = strpbrk(*str, seps);

        // Value to return in *str
        char *next_column = column_last_character;

        long int new_size = 1; // '\0'
        if (column_last_character == NULL) {
                new_size += strlen(*str);
        } else {
                new_size += (column_last_character - *str);
        }

        char *new_buffer = (char *)realloc(*buffer, new_size);
        if (new_buffer == NULL) {
                perror("Aborting due to");
                exit(1);
        }
        new_buffer[new_size - 1] = '\0';

        strncpy(new_buffer, *str, new_size - 1);

        // buffer contains '\0'
        if (new_size == 1)
                new_size = 0;

        *buffer = new_buffer;

        if (next_column != NULL && *next_column != '\0' &&
            strchr(seps, *next_column) != NULL) {
                // if(*separators == '\\') *separators++;
                *separator = *next_column++;
        }

        *str = (next_column == NULL || *next_column == '\0') ? NULL :
                                                               next_column;

        return new_size;
}

bool is_in_string(char *str, char *line)
{
        char *aux = strstr(line, str);

        return (aux == NULL) ? false : true;
}

void r30(void)
{
        TexlerObject *abc = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        abc->type = TYPE_T_BOOLEAN;
        abc->value.boolean = true;

        abc->value.boolean = false;

        free_texlerobject(abc);
        return;
}

void r31(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path.txt", "r", input) == false) {
                free_texlerobject(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        output->type = TYPE_T_FILEPTR;
        output->value.file.stream = stdout;

        copy_file_content(input->value.file.stream, output->value.file.stream);

        free_texlerobject(input);
        free_texlerobject(output);
}

TexlerObject *r32(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path.txt", "r", input) == false) {
                free_texlerobject(input);
                return NULL;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        output->type = TYPE_T_FILEPTR;
        output->value.file.stream = tmpfile();
        if (output->value.file.stream == NULL) {
                perror("An error ocurred");
                return NULL;
        }

        long int line_len = BUFFER_SIZE;
        char *line = (char *)calloc(line_len, sizeof(char));
        if (output == NULL) {
                perror("Aborting due to");
                exit(1);
        }
        // byIndex:
        for (long i = 1 - 1; i < 3; i++) {
                line_len = lines(input, &line);
                if (line_len <= 0 || line == NULL)
                        break;

                copy_buffer_content(line, output->value.file.stream);
        }

        free_texlerobject(input);
        free(line);

        return output;
}

void r33(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path.txt", "r", input) == false) {
                free_texlerobject(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        if (open_file("new_r33.txt", "w+", output) == false) {
                free_texlerobject(input);
                free_texlerobject(output);
                return;
        }

        long int line_len = BUFFER_SIZE;
        char *line = (char *)calloc(line_len, sizeof(char));
        if (output == NULL) {
                free_texlerobject(input);
                free_texlerobject(output);
                perror("Aborting due to");
                exit(1);
        }

        rewind(input->value.file.stream);

        while (line_len > 0) {
                line_len = lines(input, &line);
                if (is_in_string("palabra", line)) {
                        copy_buffer_content(line, output->value.file.stream);
                }
        }

        free_texlerobject(input);
        free_texlerobject(output);
        free(line);
}

void r34(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path_columns.txt", "r", input) == false) {
                free_texlerobject(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        if (open_file("new_r34.txt", "w+", output) == false) {
                free_texlerobject(input);
                free_texlerobject(output);
                return;
        }

        long int line_len = BUFFER_SIZE;
        char *line = (char *)calloc(line_len, sizeof(char));
        if (line == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                free_texlerobject(output);
                exit(1);
        }

        // byIndex:
        for (long i = 1 - 1; i < 2; i++) {
                line_len = lines(input, &line);
                if (line_len <= 0 || line == NULL)
                        break;

                char *remaining = line;
                long int col_len = BUFFER_SIZE;
                char *column = (char *)calloc(col_len, sizeof(char));
                if (column == NULL) {
                        perror("Aborting due to");
                        free_texlerobject(input);
                        free_texlerobject(output);
                        free(line);
                        exit(1);
                }

                while (remaining != NULL) {
                        int separator_char = 0;
                        col_len = columns(&remaining, NULL, &column,
                                          &separator_char);
                        if (column != NULL && col_len > 0) {
                                IS_NUMBER_RETURN isnum =
                                        is_number(column, strlen(column));
                                if (isnum == IS_NUMBER_RETURN_FLOATING) {
                                        double n = atof(column);
                                        n *= 2;
                                        fprintf(output->value.file.stream, "%f",
                                                n);
                                } else if (isnum == IS_NUMBER_RETURN_INTEGER) {
                                        long n = atol(column);
                                        n *= 2;
                                        fprintf(output->value.file.stream,
                                                "%ld", n);
                                } else {
                                        copy_buffer_content(
                                                column,
                                                output->value.file.stream);
                                }
                        }

                        if (separator_char) {
                                fputc(separator_char,
                                      output->value.file.stream);
                        }
                }
                free(column);
        }

        free(line);
        free_texlerobject(input);
        free_texlerobject(output);
}

void r35(void)
{
        TexlerObject *input1 = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input1 == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path1.txt", "r", input1) == false) {
                free_texlerobject(input1);
                return;
        }

        TexlerObject *input2 = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input2 == NULL) {
                perror("Aborting due to");
                free_texlerobject(input1);
                exit(1);
        }

        if (open_file("path2.txt", "r", input2) == false) {
                free_texlerobject(input1);
                free_texlerobject(input2);
                return;
        }

        TexlerObject *input3 = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input3 == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path3.txt", "r", input3) == false) {
                free_texlerobject(input1);
                free_texlerobject(input2);
                free_texlerobject(input3);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input1);
                free_texlerobject(input2);
                free_texlerobject(input3);
                exit(1);
        }

        if (open_file("new_r35.txt", "w+", output) == false) {
                free_texlerobject(input1);
                free_texlerobject(input2);
                free_texlerobject(input3);
                free_texlerobject(output);
                return;
        }

        copy_file_content(input1->value.file.stream, output->value.file.stream);
        copy_file_content(input2->value.file.stream, output->value.file.stream);

        copy_file_content(input3->value.file.stream, output->value.file.stream);

        free_texlerobject(input1);
        free_texlerobject(input2);
        free_texlerobject(input3);
        free_texlerobject(output);
}

void r36(int n)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path_r36.txt", "r", input) == false) {
                free_texlerobject(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        if (open_file("new_r36.txt", "w+", output) == false) {
                free_texlerobject(input);
                free_texlerobject(output);
                return;
        }

        output->type = TYPE_T_FILEPTR;
        output->value.file.stream = stdout;

        while (n > 0) {
                copy_file_content(input->value.file.stream,
                                  output->value.file.stream);
                n--;
        }

        free_texlerobject(input);
        free_texlerobject(output);
}

void r37(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path_r37.txt", "r", input) == false) {
                free_texlerobject(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        if (open_file("new_r37.txt", "w+", output) == false) {
                free_texlerobject(input);
                free_texlerobject(output);
                return;
        }

        long int line_len = BUFFER_SIZE;
        char *line = (char *)calloc(line_len, sizeof(char));
        if (line == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                free_texlerobject(output);
                exit(1);
        }

        // byIndex:
        while (line_len > 0) {
                line_len = lines(input, &line);
                if (line_len <= 0 || line == NULL)
                        break;

                char *remaining = line;
                long int col_len = BUFFER_SIZE;
                char *column = (char *)calloc(col_len, sizeof(char));
                if (column == NULL) {
                        perror("Aborting due to");
                        free_texlerobject(input);
                        free_texlerobject(output);
                        free(line);
                        exit(1);
                }

                while (remaining != NULL) {
                        int separator_char = 0;
                        col_len = columns(&remaining, ",\t", &column,
                                          &separator_char);
                        if (column != NULL && col_len > 0) {
                                copy_buffer_content(column,
                                                    output->value.file.stream);
                        }
                        if (separator_char) {
                                fputs("\n", output->value.file.stream);
                        }
                }
                free(column);
        }

        free_texlerobject(input);
        free_texlerobject(output);
        free(line);
}

void r38(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path.txt", "r", input) == false) {
                free_texlerobject(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        if (open_file("new_r38.txt", "w+", output) == false) {
                free_texlerobject(input);
                free_texlerobject(output);
                return;
        }

        TexlerObject *accumulator =
                (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (accumulator == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                free_texlerobject(output);
                exit(1);
        }

        accumulator->type = TYPE_T_STRING;
        accumulator->value.string = strdup("");

        long int line_len = BUFFER_SIZE;
        char *line = (char *)calloc(line_len, sizeof(char));
        if (output == NULL) {
                perror("Aborting due to");
                exit(1);
        }
        // idx in [1..5]:
        for (long idx = 1 - 1; idx < 5; idx++) {
                if (idx == 5 - 1) {
                        while (input->value.file.n_line != idx + 2) {
                                line_len = lines(input, &line);
                                if (line_len <= 0 || line == NULL)
                                        break;
                        }
                        if (line_len <= 0 || line == NULL)
                                break;

                        accumulator->value.string = (char *)realloc(
                                accumulator->value.string,
                                sizeof(char) *
                                        (1 + strlen(accumulator->value.string) +
                                         strlen(line)));
                        strcat(accumulator->value.string, line);
                } else {
                        while (input->value.file.n_line != idx + 2) {
                                line_len = lines(input, &line);
                                if (line_len <= 0 || line == NULL)
                                        break;
                        }
                        if (line_len <= 0 || line == NULL)
                                break;

                        line = string_substract(line, "\n");

                        if (line != NULL)
                                line_len = 1 + strlen(line);
                        else
                                line_len = 1;

                        line = (char *)realloc(line, (line_len + strlen(", ")) *
                                                             sizeof(char));
                        strcat(line, ", ");

                        line_len = 1 + strlen(line);

                        accumulator->value.string = (char *)realloc(
                                accumulator->value.string,
                                sizeof(char) *
                                        (1 + strlen(accumulator->value.string) +
                                         strlen(line)));

                        strcat(accumulator->value.string, line);
                }
        }

        fputs(accumulator->value.string, output->value.file.stream);

        free(line);
        free_texlerobject(accumulator);
        free_texlerobject(input);
        free_texlerobject(output);
}

void r39(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (open_file("path_columns.txt", "r", input) == false) {
                free_texlerobject(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        if (open_file("new_r39.txt", "w+", output) == false) {
                free_texlerobject(input);
                free_texlerobject(output);
                return;
        }

        long int line_len = BUFFER_SIZE;
        char *line = (char *)calloc(line_len, sizeof(char));
        if (line == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                free_texlerobject(output);
                exit(1);
        }

        // byIndex:
        while (line_len > 0) {
                line_len = lines(input, &line);
                if (line_len <= 0 || line == NULL)
                        break;

                char *remaining = line;
                long int col_len = BUFFER_SIZE;
                char *column = (char *)calloc(col_len, sizeof(char));
                if (column == NULL) {
                        perror("Aborting due to");
                        free_texlerobject(input);
                        free_texlerobject(output);
                        free(line);
                        exit(1);
                }

                while (remaining != NULL) {
                        int separator_char = 0;
                        col_len = columns(&remaining, ",", &column,
                                          &separator_char);
                        if (column != NULL && col_len > 0) {
                                IS_NUMBER_RETURN isnum =
                                        is_number(column, strlen(column));
                                if (isnum == IS_NUMBER_RETURN_FLOATING) {
                                        double n = atof(column);
                                        n *= 2.5;
                                        fprintf(output->value.file.stream, "%f",
                                                n);
                                } else if (isnum == IS_NUMBER_RETURN_INTEGER) {
                                        double n = atof(column);
                                        n *= 2.5;
                                        fprintf(output->value.file.stream, "%f",
                                                n);
                                } else {
                                        if (column[col_len - 2] != '\n') {
                                                fprintf(output->value.file
                                                                .stream,
                                                        "%s%s", column, " :)");
                                        } else {
                                                for (int i = 0; i < col_len - 2;
                                                     i++) {
                                                        fputc(column[i],
                                                              output->value.file
                                                                      .stream);
                                                }
                                                fprintf(output->value.file
                                                                .stream,
                                                        "%s\n", " :)");
                                        }
                                }
                        }

                        if (separator_char) {
                                fputc(separator_char,
                                      output->value.file.stream);
                        }
                }
                free(column);
        }

        free(line);
        free_texlerobject(input);
        free_texlerobject(output);
}

void r310(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        input->type = TYPE_T_FILE_LIST;
        input->value.file.n_files = get_list_of_files_in_dir(
                &input->value.file.path_list, "r310_folder/");

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        if (open_file("new_r310.txt", "w+", output) == false) {
                free_texlerobject(input);
                free_texlerobject(output);
                return;
        }

        for (int i = 0; i < input->value.file.n_files; i++) {
                TexlerObject *input_file =
                        (TexlerObject *)calloc(1, sizeof(TexlerObject));
                if (input_file == NULL) {
                        perror("Aborting due to");
                        free_texlerobject(input);
                        free_texlerobject(output);
                        exit(1);
                }

                if (open_file(input->value.file.path_list[i], "r",
                              input_file) == false) {
                        free_texlerobject(input_file);
                        free_texlerobject(input);
                        free_texlerobject(output);
                        return;
                }

                long int line_len = BUFFER_SIZE;
                char *line = (char *)calloc(line_len, sizeof(char));
                if (line == NULL) {
                        perror("Aborting due to");
                        free_texlerobject(input_file);
                        free_texlerobject(input);
                        free_texlerobject(output);
                        exit(1);
                }

                rewind(input_file->value.file.stream);

                while (line_len > 0) {
                        line_len = lines(input_file, &line);
                        if (is_in_string("ERROR", line)) {
                                copy_buffer_content(line,
                                                    output->value.file.stream);
                        }
                }

                free_texlerobject(input_file);
                free(line);
        }

        free_texlerobject(input);
        free_texlerobject(output);
}

void r311(void)
{
        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (input == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        // Deberia fallar aca:
        if (open_file("path_r311.txt", "r", input) == false) {
                free_texlerobject(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        if (output == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                exit(1);
        }

        if (open_file("new_r311.txt", "w+", output) == false) {
                free_texlerobject(input);
                free_texlerobject(output);
                return;
        }

        long int line_len = BUFFER_SIZE;
        char *line = (char *)calloc(line_len, sizeof(char));
        if (line == NULL) {
                perror("Aborting due to");
                free_texlerobject(input);
                free_texlerobject(output);
                exit(1);
        }

        // byIndex:
        while (line_len > 0) {
                line_len = lines(input, &line);
                if (line_len <= 0 || line == NULL)
                        break;

                char *remaining = line;
                long int col_len = BUFFER_SIZE;
                char *column = (char *)calloc(col_len, sizeof(char));
                if (column == NULL) {
                        perror("Aborting due to");
                        free_texlerobject(input);
                        free_texlerobject(output);
                        free(line);
                        exit(1);
                }

                while (remaining != NULL) {
                        int separator_char = 0;
                        col_len = columns(&remaining, "\n", &column,
                                          &separator_char);
                        if (column != NULL && col_len > 0) {
                                copy_buffer_content(column,
                                                    output->value.file.stream);
                        }
                        if (separator_char) {
                                fputs("\n", output->value.file.stream);
                        }
                }
                free(column);
        }

        free_texlerobject(input);
        free_texlerobject(output);
        free(line);
}

int main(void)
{
        printf("### r30 ###\n");
        r30();

        printf("### r31 ###\n");
        r31();
        printf("### r31 ###\n");
        r31();

        printf("### r32 ###\n");
        TexlerObject *r32_obj = r32();
        // Para checkear que funciona
        if (r32_obj != NULL) {
                printf("### r32 output ###\n");
                copy_file_content(r32_obj->value.file.stream, stdout);
                free_texlerobject(r32_obj);
        }

        printf("### r33 ###\n");
        r33();

        printf("### r34 ###\n");
        r34();

        printf("### r35 ###\n");
        r35();

        printf("### r36 ###\n");
        r36(3);

        printf("### r37 ###\n");
        r37();

        printf("### r38 ###\n");
        r38();

        printf("### r39 ###\n");
        r39();

        printf("### r310 ###\n");
        r310();

        printf("### r311 ###\n");
        r311();

        return 0;
}
