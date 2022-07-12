#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "../../frontend/syntactic-analysis/node.h"

bool generate_code(program_t *ast, const char *filename);
void generate_allocation_error_msg(FILE *const output, char *ptr_name);

#endif
