#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
        BOOLEAN = 0,
        STRING,
        REAL,
        INTEGER,
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

int main(void)
{
        r30();
        return 0;
}
