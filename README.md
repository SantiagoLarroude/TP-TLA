# Texler

Transformador de archivos de texto

### Integrantes
- Borracci, A.
- Hinojo Toré, N.
- Larroude Alvarez, S.
- Zahnd, M. E.

### Informe
El informe del trabajo es un PDF que se encuentra en la carpeta de
documentación:
[doc/Informe_Segunda-Entrega_Grupo-3.pdf](doc/Informe_Segunda-Entrega_Grupo-3.pdf)

## Compilación
### Requisitos

- CMake (>= 3.16.0)
- GCC (>= 4.9)
- GNU libc (>= 2.16)
- GNU Make (>= 4.0)
- GNU Bison (>= 3.7.0)
- Flex (>= 2.6.4)
- Bash (>= 4.4 para script de testing; >= 3.0 para scripts de compilación)

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
# Se crearán dos ejecutable en 'bin/Debug/'
# O en 'bin/Release/' si se modificó el CMakeLists.txt
# 'texler_compiler' es el transpilador programado con Flex y Bison; mientras
# que 'texler' es un script (utils/texler.sh) que realiza también 
# la compilación en C
```

Para ejecutar los tests, se debe compilar Texler estableciendo 
`CMAKE_BUILD_TYPE` en CMakeLists.txt como `Debug` y luego
```bash
cd test/
chmod +x runTest.sh
./runTest.sh
# Se muestra en pantalla el resultado de los test, se crea una subcarpeta
# 'logs/' con información de cada uno, y otra 'results/' con el código
# y archivos intermedios y de salida.
```

Para ejecutar el compilador se debe proveer como primer argumento nombre del
archivo con el código fuente de Texler.

Por ejmplo:
```bash
cd test/
../bin/Release/texler r31.texler
```
Generará el archivo temporal `r31_XXX.c`, donde cada X es un caracter
cualquiera, y el ejecutable `r31.elf`.

El código intermedio carece de formato, por lo que es ilegible.
Para solucionar esto, podemos utilizar 
[ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)
del siguiente modo:
```bash
clang-format -i r31_XXX.c # Reemplazar las X según corresponda
```
que utilizará el mismo 
[archivo de formato](.clang-format)
que el proyecto.

## Otros documentos

- [Estructura del código](STRUCTURE.md)

## Recursos utilizados

Se utilizaron principalmente los siguientes dos recursos:
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

