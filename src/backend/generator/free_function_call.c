#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "free_function_call.h"

void free_struct_free_function_call(free_function_call **ffc)
{
        if (ffc == NULL || *ffc == NULL)
                return;

        if ((*ffc)->name != NULL)
                free((*ffc)->name);
        if ((*ffc)->fun != NULL)
                free((*ffc)->fun);

        free(*ffc);
        *ffc = NULL;
}

void free_struct_free_function_call_array(free_function_call_array **ffca)
{
        if (ffca == NULL || *ffca == NULL)
                return;

        while ((*ffca)->size > 0) {
                free_struct_free_function_call(
                        &((*ffca)->data[(*ffca)->size - 1]));
                (*ffca)->size--;
        }
        free(*ffca);
        *ffca = NULL;
}

free_function_call_array *create_free_function_call_array()
{
        free_function_call_array *ffca = (free_function_call_array *)malloc(
                sizeof(free_function_call_array));
        if (ffca == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        ffca->data = NULL;
        ffca->size = 0;

        return ffca;
}

bool push_free_function_call(free_function_call_array *ffca, const char *name,
                             const char *function)
{
        if (ffca == NULL)
                return false;

        free_function_call *ffc =
                (free_function_call *)calloc(1, sizeof(free_function_call));
        if (ffc == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        if (name != NULL)
                ffc->name = strdup(name);

        if (function != NULL)
                ffc->fun = strdup(function);

        free_function_call **ffc_data_new = (free_function_call **)realloc(
                ffca->data, (1 + ffca->size) + sizeof(free_function_call *));
        if (ffc_data_new == NULL) {
                perror("Aborting due to");
                exit(1);
        }

        ffc_data_new[ffca->size] = ffc;

        ffca->data = ffc_data_new;
        ffca->size++;

        return true;
}

free_function_call *pop_free_function_call(free_function_call_array **ffca)
{
        if (ffca == NULL)
                return NULL;

        if ((*ffca)->size == 0)
                return NULL;

        free_function_call *to_return = (*ffca)->data[(*ffca)->size - 1];
        (*ffca)->size--;

        return to_return;
}

char *generate_complete_free_function_call_array(
        free_function_call_array *frees_stack)
{
        if (frees_stack == NULL)
                return NULL;

        /* Chain from last to first */
        char *to_return = NULL;
        for (long i = frees_stack->size - 1; i >= 0; i--) {
                char *tmp = NULL;
                if (frees_stack->data[i]->name != NULL &&
                    frees_stack->data[i]->fun != NULL) {
                        // 4 = '(' + ')' + ';' + '\0'
                        size_t new_len = strlen(frees_stack->data[i]->fun) +
                                         strlen(frees_stack->data[i]->name) +
                                         4;
                        if (to_return != NULL)
                                new_len += strlen(to_return);

                        tmp = (char *)calloc(new_len, sizeof(char));
                        if (tmp == NULL) {
                                perror("Aborting due to");
                                exit(1);
                        }

                        if (to_return != NULL)
                                strncpy(tmp, to_return, new_len);

                        strncat(tmp, frees_stack->data[i]->fun, new_len);
                        strncat(tmp, "(", new_len);
                        strncat(tmp, frees_stack->data[i]->name, new_len);
                        strncat(tmp, ");", new_len);

                        if (to_return != NULL)
                                free(to_return);

                        to_return = tmp;
                }
        }

        return to_return;
}
