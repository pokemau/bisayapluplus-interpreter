cmake_minimum_required(VERSION 3.27)

project(bisaya++)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -Wall -Wextra")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    src/main.c
    src/lexer.c
    src/util.c
    src/token.c
    src/parser.c
    src/ast_printer.c
    src/value.c
    src/ast.c
    src/environment.c
    src/interpreter.c

    src/util/dyn_arr.c
    src/util/arena.c
    src/util/hashmap.c
    src/util/error.c
)
