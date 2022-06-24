# Texler

Transformador de archivos de texto

### Integrantes
- Borracci, A.
- Hinojo Toré, N.
- Larroude Alvarez, S.
- Zahnd, M. E.

## Compilación
### Requisitos

- CMake (>= 3.16.0)
- GCC (>= 4.9)
- GNU libc (>= 2.16)
- GNU Make (>= 4.0)
- GNU Bison (>= 3.7.0)
- Flex (>= 2.6.4)
- Bash (>= 4.4 para script de testing; >= 3.0 para script de compilación)

### Instrucciones

Para compilar Texler
```bash
https://github.com/SSanti32/TP-TLA texler
cd texler/
chmod +x compile_compiler.sh
# Opcional: modificar CMAKE_BUILD_TYPE en CMakeLists.txt
# Default: Debug
#
./compile_compiler.sh
# Se creará un ejecutable en 'bin/Debug/texler'
# O en 'bin/Release/texler' si se modificó el CMakeLists.txt
```

Para ejecutar los tests, se debe compilar Texler estableciendo 
`CMAKE_BUILD_TYPE` en CMakeLists.txt como `Debug` y luego
```bash
cd test/
chmod +x runTest.sh
./runTest.sh
# Se muestra en pantalla el resultado de los test, y se crea una subcarpeta
# 'logs/' con información de cada uno.
```

## Otros documentos

- [Checklist basado en las correcciones de la cátedra](CORRECCIONES.md)
- [Estructura del código](STRUCTURE.md)

## Recursos utilizados

Lots of help from:
[Writing Your Own Toy Compiler Using Flex, Bison and LLVM by Loren Segai](https://gnuu.org/2009/09/18/writing-your-own-toy-compiler/)

[Flex-Bison-Compiler by Agustin Golmar](https://github.com/agustin-golmar/Flex-Bison-Compiler)

## Licencia

MIT License

Copyright (c) 2022 Borracci, A.; Hinojo, N.; Larroudé Álvarez S.; Zahnd, M. E.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

