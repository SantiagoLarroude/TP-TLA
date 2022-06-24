#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../backend/logger.h"
#include "../../backend/shared.h"

/**
 * Se definen los diferentes IDs de cada token disponible para el scanner Flex.
 */
typedef enum TokenID {
        // Por defecto, el valor "0" hace fallar el analizador sintáctico.
        UNKNOWN = 0,

        // Código de error de Bison, que permite abortar el escaneo de lexemas
        // cuando se presente un patrón desconocido. El número "257" coincide
        //  con el valor que Bison le otorga por defecto, pero además permite
        // que el resto de tokens continúen desde el valor "258" lo que
        // permite proteger los IDs internos que Bison reserva para crear
        // "tokens literales":
        YYUNDEF = 257,

        /* Identifier */
        ID,

        /* Functions */
        FUNCTION,
        RETURN,
        END,

        /* Comma (,); Colon (:); Dot (.) */
        COMMA,
        COLON,
        DOT,

        /* Brackets */
        OPEN_BRACKETS,
        CLOSE_BRACKETS,

        /* Parentheses */
        OPEN_PARENTHESIS,
        CLOSE_PARENTHESIS,

        /* Assignment */
        ASSIGN,
        FSTREAM_OVERWRITE,

        /* Logic */
        NOT,
        AND,
        OR,

        /* Comparison */
        EQUALS,
        NOT_EQUALS,

        /* Relational*/
        GREATER_THAN,
        GREATER_EQUAL,
        LESS_EQUAL,
        LESS_THAN,

        /* Arithmetic (numeric) */
        ADD,
        SUB,
        MOD,
        MUL,
        DIV,

        /* Arithmetic (string) */
        STR_SUB,
        STR_ADD,

        /* Variables */
        LET,
        BE,

        /* Conditional */
        IF,
        THEN,
        ELSE,

        /* Loop */
        FOR,
        IN,
        DO,

        /* File management */
        AS,
        WITH,

        /* STDOUT keyword */
        FSTREAM_STDOUT,

        /* Number and string constants*/
        NUMBER,
        STRING,

        /* "True" and "False" keywords */
        TTRUE,
        TFALSE,

        /* Data types */
        IS,
        TYPE_FILE,
        TYPE_NUMBER,
        TYPE_STRING,
        TYPE_BOOLEAN,

        /* End of Line */
        TEOL,
} TokenID;

/**
 * Se definen las acciones a ejecutar sobre cada patrón hallado mediante el
 * analizador léxico Flex. Este analizador solo puede identificar
 * construcciones regulares, ya que utiliza un autómata finito determinístico
 * (a.k.a. DFA), como mecanismo de escaneo y reconocimiento.
 */

TokenID pattern_id(const char* lexeme);

TokenID all_type_of_pattern(const char* lexeme, TokenID token);

TokenID pattern_string(const char* lexeme);

TokenID pattern_number(const char* lexeme);

TokenID pattern_bool(const bool lexeme);

void pattern_ignored(const char* lexeme);

TokenID pattern_unknown(const char* lexeme);

void free_yylval();

#endif
