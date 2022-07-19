# Archivos
Dentro de la carpeta `src/` se encuenta todo el código fuente del compilador.

```
.
├── CMakeLists.txt
├── compile_compiler.sh
├── src
│   ├── backend
│   │   ├── error.c
│   │   ├── error.h
│   │   ├── generator
│   │   │   ├── code-generator.c
│   │   │   ├── code-generator.h
│   │   │   ├── free_function_call.c
│   │   │   ├── free_function_call.h
│   │   │   ├── internal_functions.c
│   │   │   ├── internal_functions.h
│   │   │   ├── standard_functions.c
│   │   │   └── standard_functions.h
│   │   ├── logger.c
│   │   ├── logger.h
│   │   ├── mem_management.c
│   │   ├── mem_management.h
│   │   ├── shared.h
│   │   ├── symbols.c
│   │   └── symbols.h
│   ├── CMakeLists.txt
│   ├── frontend
│   │   ├── lexical-analysis
│   │   │   ├── flex-actions.c
│   │   │   ├── flex-actions.h
│   │   │   ├── flex-patterns.l
│   │   │   └── flex-scanner.c
│   │   └── syntactic-analysis
│   │       ├── bison-actions.c
│   │       ├── bison-actions.h
│   │       ├── bison-grammar.y
│   │       ├── bison-parser.c
│   │       ├── bison-parser.h
│   │       └── node.h
│   └── main.c
└── utils
    └── texler.sh
```

En `backend/shared.h` se encuentra la estructura `YYSTYPE`, donde se definen 
los distintos tipos nodos que tendrá el AST; es utilizada por Bison para 
generar la gramática.

En `frontend/lexical-analysis/` se encuenta
    - `flex-patterns.l`: Todos los tokens que identifica Flex
    - `flex-actions`: Las funciones que utiliza Flex para interpretar ciertos
        tokens (por ejemplo `pattern_bool`) o regex (como `pattern_string`)

En `frontend/syntactic-analysis/` se encuenta
    - `node.h`: Contiene la estructura de cada tipo de nodo (utilizados en 
    `YYSTYPE`)
    - `bison-grammar.y`: Contiene la gramática del lenguaje. A partir de este
    archivo se genera el autómata que interpreta nuestra gramática y se 
    realizan los llamados a las funciones declaradas en `bison-actions.h`
    - `bison-actions`: Las funciones dentro de este archivo deben interpretar
    la gramática generando los nodos del árbol y, en base a ellos, el AST. 

Dentro de `backend/` se encuentran archivos con el manejo de la memoria, 
la tabla de símbolos utilizada para generar el árbol, mensajes de error y,
particularmente, el transpilador en la subcarpeta `generator/`

Por último, `utils/texler.sh` es el script que se copia junto al compilador
generado con CMake y permite realizar todos los pasos para compilar un 
archivo de Texler.
