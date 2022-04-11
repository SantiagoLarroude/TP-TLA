#include <stdbool.h>            /* bool type */
#include <stdlib.h>             /* atof */
#include <string.h>             /* strcpy */

/* Backend */
#include "../../backend/logger.h"

/* This file */
#include "flex-actions.h"


/* Macros and constants */
// Debug

// Log function call with prototype: function(char* val)
#define log_debug_single_value(val)                                         \
        LogDebug("%s: '%s'\n\tFile: %s", __func__, val, __FILE__);


/* Prototypes */
static bool save_token(const char* lexeme);

/**
 * "flex-rules.h" implementation
 */

TokenID pattern_id(const char* lexeme)
{
        log_debug_single_value(lexeme);

        if (save_token(lexeme))
                return ID;


        return YYUNDEF;
}

TokenID pattern_string(const char* lexeme)
{
        log_debug_single_value(lexeme);

        if (save_token(lexeme))
                return STRING;


        return YYUNDEF;
}

TokenID pattern_number(const char* lexeme)
{
        log_debug_single_value(lexeme);
        if (save_token(lexeme))
                return NUMBER;


        return YYUNDEF;
}

void pattern_ignored(const char* lexeme)
{
        log_debug_single_value(lexeme);
}

TokenID pattern_unknown(const char* lexeme)
{
        log_debug_single_value(lexeme);

        return YYUNDEF;
}

static bool save_token(const char* lexeme)
{
        size_t len = strlen(lexeme);

        yylval.string = malloc(sizeof(char) * len);
        if (yylval.string == NULL) {
                log_error_no_mem();
                return false;
        }

        yylval.length = len;
        return true;
}