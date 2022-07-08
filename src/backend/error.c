#include <stdlib.h>

#include "symbols.h"

#include "logger.h"

void error_no_memory()
{
        LogError("Could not allocate memory.\n\tFunction: %s\n", __func__);
}

void error_multiple_declaration(const char *name)
{
        LogError("Multiple declaration of variable %s.", name);
}

void error_variable_not_found(const char *name)
{
        LogError("Variable %s not found.", name);
}

void error_dangling_variable_found()
{
        LogError("Found %d variables with no value assigned.",
                 count_dangling());
}

void error_function_not_found(const char *name)
{
        LogError("Function %s not found.", name);
}

void error_invalid_range(const char *from, const char *to)
{
        LogError("Invalid range detected.\nFrom: %s\nTo: %s\n", from, to);
}
