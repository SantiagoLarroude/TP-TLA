#include <stdlib.h>

#include "logger.h"

void error_no_memory()
{
        LogError("Could not allocate memory.\n\tFunction: %s\n", __func__);
}

void error_multiple_declaration(const char* name)
{
        LogError("Multiple declaration of variable %s.", name);
}

void error_variable_not_found(const char* name)
{
        LogError("Variable %s not found.", name);
}
