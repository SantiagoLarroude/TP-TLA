#include <ctype.h>
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
        N_TYPE_T
} type_t;

typedef enum {
        IS_NUMBER_RETURN_NAN = 0,
        IS_NUMBER_RETURN_INTEGER,
        IS_NUMBER_RETURN_FLOATING,
        N_IS_NUMBER_RETURN
} IS_NUMBER_RETURN;

typedef struct TexlerObject {
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
                } file;
        } value;
        type_t type;
} TexlerObject;

void free_texlerobject(TexlerObject *tex_obj)
{
        if (tex_obj == NULL)
                return;

        switch (tex_obj->type) {
        case TYPE_T_FILEPTR:
                if (tex_obj->value.file.stream != NULL &&
                    tex_obj->value.file.stream != stdout &&
                    tex_obj->value.file.stream != stderr &&
                    tex_obj->value.file.stream != stdin)
                        fclose(tex_obj->value.file.stream);
                break;
        case TYPE_T_BOOLEAN: /* Fallsthrough*/
        case TYPE_T_STRING:
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
        char *error_msg = (char *)calloc(error_msg_len, sizeof(char));
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

        rewind(fptr);

        tex_obj->type = TYPE_T_FILEPTR;
        tex_obj->value.file.stream = fptr;
        if (fgetpos(tex_obj->value.file.stream, &tex_obj->value.file.pos)) {
                perror("Error while getting file position");
                return false;
        }
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

        copy_file_content(input1->value.file.stream,
                          output->value.file.stream);
        copy_file_content(input2->value.file.stream,
                          output->value.file.stream);


        copy_file_content(input3->value.file.stream,
                          output->value.file.stream);

        free_texlerobject(input1);
        free_texlerobject(input2);
        free_texlerobject(input3);
        free_texlerobject(output);
}

void r36(int n){
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

        while (n > 0)
        {
                copy_file_content(input->value.file.stream, 
                        output->value.file.stream);
                n--;
        }

        free_texlerobject(input);
        free_texlerobject(output);
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

        return 0;
}
