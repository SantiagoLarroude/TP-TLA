#ifndef SHARED_HEADER
#define SHARED_HEADER

#include <stdio.h>
#include <stdbool.h>

typedef unsigned int token_t;  // Must be before including node.h

#include "../frontend/syntactic-analysis/node.h"        /* node_* types */
#include "../backend/logger.h"                          /* LogError */

/* Macros and constants */
// Error

// Log "out of memory error".
#define log_error_no_mem()                                                  \
        LogError("Could not allocate memory.\n\tFunction: %s\n\tFile: %s\n\tLine: %s",  \
                __func__, __FILE__, __LINE__);


/* Structures and types */

union YYSTYPE {
        struct node_block* block;
        struct node_expression* expr;
        struct node_statement* stmt;
        struct node_identifier* id;
        struct node_file_type* file_type;

        char* string;

        token_t token;
};

// Descriptor del archivo de entrada que utiliza Bison.
extern FILE* yyin;

// Descriptor del archivo de salida que utiliza Bison.
extern FILE* yyout;

// Variable global que contiene el número escaneado.
extern union YYSTYPE yylval;

// Variable global que contiene el número de la línea analizada.
extern int yylineno;

// Token actual en el tope de la pila del analizador Bison.
extern char* yytext;

// Función global de manejo de errores en Bison.
extern void yyerror(const char* string);

// Función global del analizador léxico Flex.
extern int yylex(void);

// Función global del analizador sintáctico Bison.
extern int yyparse(void);

// Estado global de toda la aplicación.
typedef struct {

        // Indica si la compilación tuvo problemas hasta el momento:
        bool succeed;

        // Indica el resultado de la compilación:
        int result;

        // Agregar una pila para almacenar tokens/nodos.
        // Agregar un nodo hacia la raíz del árbol de sintaxis abstracta.
        // Agregar una tabla de símbolos.
        // ...

} CompilerState;

// El estado se define e inicializa en el archivo "main.c":
extern CompilerState state;

extern struct node_block* programblock;

extern void free_programblock(struct node_block* program);

#endif
