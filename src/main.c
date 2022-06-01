// #include "backend/code-generation/generator.h"
#include <stdio.h>

#include "backend/logger.h"
#include "backend/shared.h"

#include "frontend/syntactic-analysis/bison-parser.h"

//Estado de la aplicación.
CompilerState state;

extern void free_yylval();

// Punto de entrada principal del compilador.
const int main(const int argumentCount, const char** arguments)
{
        // Inicializar estado de la aplicación.
        state.result = 0;
        state.succeed = false;

        // Mostrar parámetros recibidos por consola.
        for (int i = 0; i < argumentCount; ++i) {
                LogInfo("Argumento %d: '%s'", i, arguments[i]);
        }

        // Compilar el programa de entrada.
        LogInfo("Compilando...\n");
        const int result = yyparse();
        
        switch (result) {
        case 0:
                if (state.succeed) {
                        LogInfo("La compilación fue exitosa.");
                        // Generator(state.result);
                }
                else {
                        LogError("Se produjo un error en la aplicacion.");
                        // free_programblock(programblock);
                        free_yylval();
                        return -1;
                }
                break;
        case 1:
                LogError("Bison finalizó debido a un error de sintaxis.");
                break;
        case 2:
                LogError("Bison finalizó abruptamente debido a que ya no hay memoria disponible.");
                break;
        default:
                LogError("Error desconocido mientras se ejecutaba el analizador Bison (codigo %d).", result);
        }
        LogInfo("Fin.");

        // free_programblock(programblock);
        free_yylval();

        return result;
}
