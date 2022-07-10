#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 256

typedef enum {
        BOOLEAN = 0,
        STRING,
        REAL,
        INTEGER,
        FILEPTR,
        BUFFER_STDOUT,
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
                FILE *file;
        } value;
        type_t type;
} TexlerObject;

void r30(void)
{
        TexlerObject *abc = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        abc->type = BOOLEAN;
        abc->value.boolean = true;

        abc->value.boolean = false;

        free(abc);
        return;
}

void r31(void)
{
        char buffer[BUFFER_SIZE] = { 0 };

        TexlerObject *input = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        input->type = FILEPTR;
        input->value.file = fopen("path.txt", "r");
        if (input->value.file == NULL) {
                perror("Error when trying to open file 'path.txt'");
                free(input);
                return;
        }

        TexlerObject *output = (TexlerObject *)calloc(1, sizeof(TexlerObject));
        output->type = FILEPTR;
        output->value.file = stdout;

        while (!feof(input->value.file)) {
                if (fgets(buffer, BUFFER_SIZE, input->value.file) == NULL)
                        break;

                fputs(buffer, output->value.file);
        }

        free(input);
        free(output);
}

int main(void)
{
        r30();
        r31();
        return 0;
}
