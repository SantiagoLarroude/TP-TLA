#ifndef ERROR_H
#define ERROR_H

void error_no_memory();
void error_multiple_declaration(const char* name);
void error_variable_not_found(const char* name);

#endif