# MIT License
# This file is part of texler
# 
# Copyright (c) 2022 Borracci, A.; Hinojo Toré, N.; Larroudé Álvarez S.; Zahnd, M. E.
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


cmake_minimum_required(VERSION 3.16.0)

# Compilar el parser con Bison:
add_custom_command(
        OUTPUT  ${CMAKE_SOURCE_DIR}/src/frontend/syntactic-analysis/bison-parser.c
                ${CMAKE_SOURCE_DIR}/src/frontend/syntactic-analysis/bison-parser.h
        COMMAND bison   -d ${CMAKE_SOURCE_DIR}/src/frontend/syntactic-analysis/bison-grammar.y
                        -o ${CMAKE_SOURCE_DIR}/src/frontend/syntactic-analysis/bison-parser.c
                        -Wcounterexamples
        )

message(STATUS "Bison built.")

# Seleccionar estrategia según el compilador de C disponible en el sistema:
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        message(NOTICE "Using GCC C compiler.")

        # Opciones para GCC:
        add_compile_options(-static-libgcc)

        # Compilar el scanner con Flex:
        add_custom_command(
                OUTPUT ${CMAKE_SOURCE_DIR}/src/frontend/lexical-analysis/flex-scanner.c
                COMMAND flex -o ${CMAKE_SOURCE_DIR}/src/frontend/lexical-analysis/flex-scanner.c
                        ${CMAKE_SOURCE_DIR}/src/frontend/lexical-analysis/flex-patterns.l
                DEPENDS ${CMAKE_SOURCE_DIR}/src/frontend/syntactic-analysis/bison-parser.c 
                ${CMAKE_SOURCE_DIR}/src/frontend/syntactic-analysis/bison-parser.h)
        
        message(STATUS "Flex built.")

elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        message(NOTICE "Using Microsoft Visual Studio C compiler.")

        # Opciones para MSVC:
        # add_compile_options(...)

        # Compilar el scanner con Flex, usando compatibilidad con Microsoft Windows:
        add_custom_command(
                OUTPUT ${CMAKE_SOURCE_DIR}/src/frontend/lexical-analysis/flex-scanner.c
                COMMAND flex --wincompat -o ${CMAKE_SOURCE_DIR}/src/frontend/lexical-analysis/flex-scanner.c
                                            ${CMAKE_SOURCE_DIR}/src/frontend/lexical-analysis/flex-patterns.l
                DEPENDS ${CMAKE_SOURCE_DIR}/src/frontend/syntactic-analysis/bison-parser.c
                        ${CMAKE_SOURCE_DIR}/src/frontend/syntactic-analysis/bison-parser.h)

        message(NOTICE "Flex built.")
else ()
        message(SEND_ERROR "Unknown C compiler.
Flex scanner could not be generated.")
endif ()


add_compile_options(
        -Wall
        # -pedantic
)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
        add_compile_definitions(
                DEBUG
        )
endif()

add_executable(${PROJECT_NAME}
        main.c
        frontend/lexical-analysis/flex-actions.c
        frontend/lexical-analysis/flex-scanner.c
        frontend/syntactic-analysis/bison-actions.c
        frontend/syntactic-analysis/bison-parser.c
        frontend/syntactic-analysis/node.h
        backend/error.c
        backend/logger.c
        backend/symbols.c
)

target_link_libraries(${PROJECT_NAME})