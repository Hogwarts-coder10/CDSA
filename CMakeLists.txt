cmake_minimum_required(VERSION 3.20)

project(CDSA C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cdsa
    src/vector.c
    src/kstring.c
    src/linkedlist.c
    src/stack.c
    src/hashmap.c
    src/ringbuffer.c
    src/queue.c
    src/deque.c
    src/skiplist.c
    src/art.c
    src/priority_queue.c
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

add_executable(test_stack
    tests/test_stack.c
)

target_link_libraries(test_stack
  PRIVATE cdsa
)

add_executable(test_hashmap
  tests/test_hashmap.c
)
target_link_libraries(test_hashmap
  PRIVATE cdsa
)

add_executable(test_ringbuffer
  tests/test_ringbuffer.c
)

target_link_libraries(test_ringbuffer
  PRIVATE cdsa
)

add_executable(test_queue
  tests/test_queue.c
)
target_link_libraries(test_queue
  PRIVATE  cdsa
)

add_executable(test_deque
  tests/test_deque.c
)

target_link_libraries(test_deque
  PRIVATE cdsa
)

add_executable(test_skiplist
  tests/test_skiplist.c
)

target_link_libraries(test_skiplist
  PRIVATE cdsa
)

add_executable(test_art
  tests/test_art.c
)

target_link_libraries(test_art
  PRIVATE cdsa
)

add_executable(test_priority_queue
  tests/test_priority_queue.c
)

target_link_libraries(test_priority_queue
  PRIVATE cdsa
)


target_compile_options(cdsa PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
