#ifndef SHARED_HEADER
#define SHARED_HEADER

#include <stdio.h>
#include <stdbool.h>

#include "../frontend/syntactic-analysis/node.h"        /* node_* types */
#include "../backend/logger.h"                          /* LogError */

/* Structures and types */
union YYSTYPE {
        // No terminales
        struct program* program;
        
        struct node_function* fun;
        struct node_function_call* fun_call;
        
        struct node_expression* expr;
        struct node_expression_list* expression_list;

        struct node_file_block* file_block;
        struct node_list* list;

        struct variable* var;

        // Terminales
        union variable_value value;
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
extern void yyerror(program_t* root_node, const char* string);

// Función global del analizador léxico Flex.
extern int yylex(void);

// Función global del analizador sintáctico Bison.
extern int yyparse(program_t* root_node);

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

#endif
