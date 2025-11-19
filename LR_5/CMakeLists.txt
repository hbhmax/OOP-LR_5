cmake_minimum_required(VERSION 3.10)
project(SinglyLinkedList)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.0
  TLS_VERIFY false
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(singly_linked_list_lib 
    include/singly_linked_list.h
    src/fixed_block_memory_resource.cpp
)

target_include_directories(singly_linked_list_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME}_exe main.cpp)
target_link_libraries(${PROJECT_NAME}_exe PRIVATE singly_linked_list_lib)

enable_testing()

add_executable(tests test/tests.cpp)
target_link_libraries(tests PRIVATE singly_linked_list_lib gtest_main)

add_test(NAME SinglyLinkedListTests COMMAND tests)