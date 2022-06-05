# Archivos
Dentro de la carpeta `src/` se encuenta todo el código fuente del compilador.

```
src/
├── backend
│   ├── logger.c
│   ├── logger.h
│   └── shared.h
├── frontend
│   ├── lexical-analysis
│   │   ├── flex-actions.c
│   │   ├── flex-actions.h
│   │   ├── flex-patterns.l
│   └── syntactic-analysis
│       ├── bison-actions.c
│       ├── bison-actions.h
│       ├── bison-grammar.y
│       └── node.h
├── CMakeLists.txt
└── main.c
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
