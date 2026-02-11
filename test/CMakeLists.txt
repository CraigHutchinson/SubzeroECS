cmake_minimum_required(VERSION 3.14...3.22)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/..")

project(SubzeroECSTests LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 20)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# --- Import tools ----

include(tools)

# ---- Dependencies ----

include(CPM)


find_package(GTest)
# TODO: Change to DocTest as looks faster!
#CPMAddPackage("gh:doctest/doctest@2.4.9")

CPMAddPackage("gh:TheLartians/Format.cmake@1.7.3")

if ( NOT TARGET SubzeroECS )
    if(TEST_INSTALLED_VERSION)
      find_package(SubzeroECS REQUIRED)
    else()
      CPMAddPackage(NAME SubzeroECS SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
    endif()
endif()

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
add_executable(${PROJECT_NAME})
target_sources( ${PROJECT_NAME} PRIVATE ${sources} )

target_link_libraries(${PROJECT_NAME} 
    #doctest::doctest 
    GTest::Main
    SubzeroECS::SubzeroECS
)
set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    CXX_STANDARD 20
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 16
)

# enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(SubzeroECS PUBLIC -Wall -Wpedantic -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(SubzeroECS PUBLIC /W4 /WX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
  endif()
endif()

# ---- Add SubzeroECSTests ----

enable_testing()

# Note: doctest and similar testing frameworks can automatically configure CMake tests. For other
# testing frameworks add the tests target instead: add_test(NAME ${PROJECT_NAME} COMMAND
# ${PROJECT_NAME})

#include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
#doctest_discover_tests(${PROJECT_NAME})

gtest_discover_tests(${PROJECT_NAME})

# ---- code coverage ----

if(ENABLE_TEST_COVERAGE)
  target_compile_options(SubzeroECS PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(SubzeroECS PUBLIC -fprofile-arcs -ftest-coverage)
endif()
