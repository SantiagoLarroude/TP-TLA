#include <stdlib.h>

#include "logger.h"

static void debug_information();

void error_no_memory()
{
        LogError("Could not allocate memory.\n\tFunction: %s\n", __func__);

        debug_information();
}

void error_multiple_declaration(const char* name)
{
        LogError("Multiple declaration of variable %s.", name);

        debug_information();
}

void error_variable_not_found(const char* name)
{
        LogError("Variable %s not found.", name);

        debug_information();
}

static void debug_information()
{
        LogDebug("Function: %s\n\tFile: %s\n\tLine: %s",
                 __func__, __FILE__, __LINE__);
}