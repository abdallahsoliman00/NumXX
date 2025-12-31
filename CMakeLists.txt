cmake_minimum_required(VERSION 3.16)

project(NumXX
    VERSION 0.0.1
    LANGUAGES CXX
)

if (NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 17)
    set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(NumXX INTERFACE)

target_include_directories(
    NumXX INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/NumXX
)

# To build tests, add the following flag to the `cmake` command: -DBUILD_TESTS=ON
option(BUILD_TESTS "Build the test suite" OFF)

function(build_test file_name)
    add_executable(${file_name} unit_tests/${file_name}.cpp)
    target_link_libraries(${file_name} PUBLIC NumXX)
endfunction()

if(BUILD_TESTS)
    build_test(general_testing)
endif()