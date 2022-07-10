#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 256

typedef enum {
        NONE = 0,
        BOOLEAN,
        STRING,
        REAL,
        INTEGER,
        FILEPTR,
} type_t;

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
        case FILEPTR:
                if (tex_obj->value.file.stream != NULL &&
                    tex_obj->value.file.stream != stdout &&
                    tex_obj->value.file.stream != stderr &&
                    tex_obj->value.file.stream != stdin)
                        fclose(tex_obj->value.file.stream);
                break;
        case BOOLEAN: /* Fallsthrough*/
        case STRING:
        case REAL:
        case INTEGER:
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

        tex_obj->type = FILEPTR;
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
long int lines(TexlerObject *tex_obj, char **buffer, long int size)
{
        if (tex_obj == NULL || buffer == NULL || *buffer == NULL || size < 0)
                return 0;

        if (tex_obj->type != FILEPTR || tex_obj->value.file.stream == NULL)
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

void r30(void)
{
        TexlerObject *abc = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        abc->type = BOOLEAN;
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

        output->type = FILEPTR;
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
                exit(1);
        }

        output->type = FILEPTR;
        output->value.file.stream = tmpfile();
        if (output->value.file.stream == NULL) {
                perror("An error ocurred");
                return NULL;
        }

        long int length = BUFFER_SIZE;
        char *line = (char *)calloc(length, sizeof(char));
        if (output == NULL) {
                perror("Aborting due to");
                exit(1);
        }
        for (long i = 0; i < 3 + 1; i++) {
                length = lines(input, &line, length);
                if (length <= 0 || line == NULL)
                        break;

                copy_buffer_content(line, output->value.file.stream);
        }

        free_texlerobject(input);
        return output;
}

int main(void)
{
        r30();

        r31();
        r31();

        TexlerObject *r32_obj = r32();
        // Para checkear que funciona
        if (r32_obj != NULL) {
                printf("### r32 output ###\n");
                copy_file_content(r32_obj->value.file.stream, stdout);
                free_texlerobject(r32_obj);
        }

        return 0;
}
