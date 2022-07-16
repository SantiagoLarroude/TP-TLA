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

void error_invalid_function_arguments()
{
        LogError("Invalid name or arguments for function");
}

void error_in_function(const char *name)
{
        LogError("Error in function: %s", name);
}

void error_invalid_file_variable_name(const char *name)
{
        LogError("'%s' is not a valid name for a variable of type File.\n"
                 "Valid names start with 'input' or 'output'.",
                 name);
}

void error_invalid_node_file_handler(const char *function)
{
        LogError("Invalid node_file_block.\tFunction: %s", function);
}

void error_invalid_byIndex_argument()
{
        LogError("A call to byIndex() has an invalid argument type."
                 "Valid types are:\n"
                 "\t- Constants. ie: byIndex(5)\n"
                 "\t- Ranges. ie: byIndex([1..6])\n");
}

void error_invalid_multiplication_type()
{
        LogError("Trying to multiply with an invalid type.\n"
        "You can only use variable names and numbers.\n");
}