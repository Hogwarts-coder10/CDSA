cmake_minimum_required(VERSION 3.20)

project(CDSA C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==========================================
# ====== SANITIZERS (Phase 3) ==============
# ==========================================
option(CDSA_USE_SANITIZERS "Enable Address and Undefined Behavior Sanitizers" OFF)

if(CDSA_USE_SANITIZERS)
    message(STATUS "CDSA: Building with ASan and UBSan enabled")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g -O1)
    add_link_options(-fsanitize=address,undefined)
endif()

# ==========================================
# ====== BENCHMARKS ========================
# ==========================================
option(CDSA_BUILD_BENCHMARKS "Build the CDSA benchmark suite" OFF)

if(CDSA_BUILD_BENCHMARKS)
    message(STATUS "CDSA: Building benchmarks")
    
    # We want max optimization for benchmarks, turn off sanitizers if benchmarking
    # Usually, we compile benchmarks in Release mode (-O3)
    add_executable(bench_collections bench/bench_collections.c)
    target_link_libraries(bench_collections PRIVATE cdsa)
    target_compile_options(bench_collections PRIVATE -O3)

    # Added Advanced Workload Simulator
    add_executable(bench_workloads bench/bench_workloads.c)
    target_link_libraries(bench_workloads PRIVATE cdsa)
    target_compile_options(bench_workloads PRIVATE -O3)

    # Added STL Baseline Comparison (C++)
    add_executable(bench_stl_baseline bench/bench_stl_baseline.cpp)
    target_link_libraries(bench_stl_baseline PRIVATE cdsa)
    target_compile_options(bench_stl_baseline PRIVATE -O3)

    add_executable(bench_fuzzer bench/bench_fuzzer.c)
    target_link_libraries(bench_fuzzer PRIVATE cdsa)
    target_compile_options(bench_fuzzer PRIVATE -O3)



endif()

# ==========================================
# ====== CORE LIBRARY ======================
# ==========================================
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
    src/allocator.c  # <--- Added global memory tracker
)

target_include_directories(cdsa PUBLIC include)

target_compile_options(cdsa PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# ==========================================
# ====== OPTIONAL TEST SUITE ===============
# ==========================================
# Check if CDSA is the top-level project, or a submodule in Kedis-C
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CDSA_IS_TOP_LEVEL TRUE)
else()
    set(CDSA_IS_TOP_LEVEL FALSE)
endif()

option(CDSA_BUILD_TESTS "Build the CDSA test suite" ${CDSA_IS_TOP_LEVEL})

if(CDSA_BUILD_TESTS)
    enable_testing()

    add_executable(test_vector tests/test_vector.c)
    target_link_libraries(test_vector PRIVATE cdsa)

    add_executable(test_kstring tests/test_kstring.c)
    target_link_libraries(test_kstring PRIVATE cdsa)

    add_executable(test_linkedlist tests/test_linkedlist.c)
    target_link_libraries(test_linkedlist PRIVATE cdsa)

    add_executable(test_stack tests/test_stack.c)
    target_link_libraries(test_stack PRIVATE cdsa)

    add_executable(test_hashmap tests/test_hashmap.c)
    target_link_libraries(test_hashmap PRIVATE cdsa)

    add_executable(test_ringbuffer tests/test_ringbuffer.c)
    target_link_libraries(test_ringbuffer PRIVATE cdsa)

    add_executable(test_queue tests/test_queue.c)
    target_link_libraries(test_queue PRIVATE cdsa)

    add_executable(test_deque tests/test_deque.c)
    target_link_libraries(test_deque PRIVATE cdsa)

    add_executable(test_skiplist tests/test_skiplist.c)
    target_link_libraries(test_skiplist PRIVATE cdsa)

    add_executable(test_art tests/test_art.c)
    target_link_libraries(test_art PRIVATE cdsa)

    add_executable(test_priority_queue tests/test_priority_queue.c)
    target_link_libraries(test_priority_queue PRIVATE cdsa)

    add_executable(test_allocator tests/test_allocator.c)
    target_link_libraries(test_allocator PRIVATE cdsa)


    add_executable(test_hashmap_iterator tests/test_hashmap_iterator.c)
    target_link_libraries(test_hashmap_iterator PRIVATE cdsa)

    add_executable(test_skiplist_iterator tests/test_skiplist_iterator.c)
    target_link_libraries(test_skiplist_iterator PRIVATE cdsa)

    add_executable(test_vector_iterator tests/test_vector_iterator.c)
    target_link_libraries(test_vector_iterator PRIVATE cdsa)

    add_executable(test_ringbuffer_iterator tests/test_ringbuffer_iterator.c)
    target_link_libraries(test_ringbuffer_iterator PRIVATE cdsa)

    add_executable(test_linkedlist_iterator tests/test_linkedlist_iterator.c)
    target_link_libraries(test_linkedlist_iterator PRIVATE cdsa)

    add_executable(test_wrappers_iterators tests/test_wrappers_iterators.c)
    target_link_libraries(test_wrappers_iterators PRIVATE cdsa)

    add_executable(test_art_iterator tests/test_art_iterator.c)
    target_link_libraries(test_art_iterator PRIVATE cdsa)
endif()

# ==========================================
# ====== SYSTEM-WIDE INSTALLATION ==========
# ==========================================
include(GNUInstallDirs)

install(TARGETS cdsa
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/CDSA
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)
