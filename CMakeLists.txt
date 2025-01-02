cmake_minimum_required(VERSION 3.20)
project(fsh)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)
add_executable(fsh "src/main.c" "src/repl.c" "src/eval.c" "src/handler.c")
