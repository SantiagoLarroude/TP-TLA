#ifndef FREE_FUNCTION_CALL_H
#define FREE_FUNCTION_CALL_H

#include <stdbool.h>

typedef struct free_function_call {
        char *name;
        char *fun;
} free_function_call;

typedef struct free_function_call_array {
        free_function_call **data;
        size_t size;
} free_function_call_array;

free_function_call_array *create_free_function_call_array();

bool push_free_function_call(free_function_call_array *ffca, const char *name,
                             const char *function);
free_function_call *pop_free_function_call(free_function_call_array **ffca);

char *generate_complete_free_function_call_array(
        free_function_call_array *frees_stack);

void free_struct_free_function_call(free_function_call **ffc);
void free_struct_free_function_call_array(free_function_call_array **ffca);

#endif /* FREE_FUNCTION_CALL_H */
