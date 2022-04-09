#include <stdlib.h>                 /* atof */

#include "flex-actions.h"
#include "../../backend/logger.h"


/**
 * Implementación de "flex-rules.h".
 */

TokenID StringPatternAction(const char * lexeme) {
    LogDebug("StringPatternAction: '%s'.", lexeme);
    // yylval = lexeme; // TODO: Cómo se guarda?
    return STRING;
}

TokenID NumberPatternAction(const char * lexeme) {
    LogDebug("NumberPatternAction: '%s'.", lexeme);
    yylval = atof(lexeme);
    return NUMBER;
}

void IgnoredPatternAction(const char * lexeme) {
    LogDebug("IgnoredPatternAction: '%s'.", lexeme);
}

TokenID UnknownPatternAction(const char * lexeme) {
    LogDebug("UnknownPatternAction: '%s'.", lexeme);
    return YYUNDEF;
}
