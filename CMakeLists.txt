cmake_minimum_required(VERSION 3.20)

project(CDSA C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cdsa
    src/vector.c
    src/kstring.c
    src/linkedlist.c
)

target_include_directories(cdsa PUBLIC include)

add_executable(test_vector
    tests/test_vector.c
)

target_link_libraries(test_vector cdsa)

add_executable(test_kstring
    tests/test_kstring.c
)

target_link_libraries(test_kstring
    PRIVATE cdsa
)

add_executable(test_linkedlist
    tests/test_linkedlist.c
)

target_link_libraries(test_linkedlist
    PRIVATE cdsa
)

target_compile_options(cdsa PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
