#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "../../frontend/syntactic-analysis/node.h"

void generate_code(program_t * ast);
void generate_allocation_error_msg(char* ptr_name);

#endif
