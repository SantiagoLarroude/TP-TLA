#include <stdbool.h> /* bool type */
#include <stdlib.h> /* atof */
#include <string.h> /* strcpy */

/* Backend */
#include "../../backend/logger.h" /* LogDebug */

/* This file */
#include "flex-actions.h"

/* Macros and constants */
// Debug

// Log function call with prototype: function(char* val)
#define log_debug_single_value(val) \
        LogDebug("%s: '%s'\n\tFile: %s", __func__, val, __FILE__);

/* Prototypes */
static bool save_token(const char *lexeme);

/**
 * "flex-rules.h" implementation
 */

TokenID pattern(const char *lexeme, TokenID token)
{
        log_debug_single_value(lexeme);

        if (save_token(lexeme))
                return token;

        return YYUNDEF;
}


TokenID pattern_id(const char *lexeme)
{
        log_debug_single_value(lexeme);

        if (save_token_id(lexeme))
                return ID;

        return YYUNDEF;
}

static bool save_token_id(const char *lexeme)
{
        yylval.id =
                realloc(yylval.id, sizeof(char) * strlen(lexeme));
        if (yylval.id == NULL) {
                log_error_no_mem();
                return false;
        }

        strcpy(yylval.id, lexeme);

        return true;
}

TokenID pattern_string(const char *lexeme)
{
        log_debug_single_value(lexeme);

        if (save_token_string(lexeme))
                return STRING;

        return YYUNDEF;
}

static bool save_token_string(const char *lexeme)
{
        yylval.string =
                realloc(yylval.string, sizeof(char) * strlen(lexeme));
        if (yylval.string == NULL) {
                log_error_no_mem();
                return false;
        }

        strcpy(yylval.string, lexeme);

        return true;
}

TokenID pattern_number(const char *lexeme)
{
        log_debug_single_value(lexeme);
        if (save_token_number(lexeme))
                return NUMBER;

        return YYUNDEF;
}

static bool save_token_number(const char *lexeme)
{
        yylval.number =
                realloc(yylval.number, sizeof(char) * strlen(lexeme));
        if (yylval.number == NULL) {
                log_error_no_mem();
                return false;
        }

        strcpy(yylval.number, lexeme);

        return true;
}

TokenID pattern_bool(const bool lexeme)
{
        LogDebug("%s: '%d'\n\tFile: %s", __func__, lexeme, __FILE__);
        if (lexeme == true)
                return TTRUE;
        else
                return TFALSE;

        return YYUNDEF;
}

void pattern_ignored(const char *lexeme)
{
        /* Easier to read in the terminal */
        switch (lexeme[0]) {
        case '\t':
                log_debug_single_value("\\t");
                break;
        case '\v':
                log_debug_single_value("\\v");
                break;
        case '\f':
                log_debug_single_value("\\f");
                break;
        case '\n':
                log_debug_single_value("\\n");
                break;
        case '\r':
                log_debug_single_value("\\r");
                break;
        default:
                log_debug_single_value(lexeme);
                break;
        }
}

TokenID pattern_unknown(const char *lexeme)
{
        log_debug_single_value(lexeme);

        return YYUNDEF;
}

void free_yylval()
{
        free(yylval.value.string);
}

static bool save_token(const char *lexeme)
{
        yylval.token =
                realloc(yylval.token, sizeof(char) * strlen(lexeme));
        if (yylval.token == NULL) {
                log_error_no_mem();
                return false;
        }

        strcpy(yylval.token, lexeme);

        return true;
}
