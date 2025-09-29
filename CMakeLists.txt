cmake_minimum_required(VERSION 3.16)

project(simplet_tests C)

# Enable testing with CTest
enable_testing()

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define HOST_TEST_BUILD for host-based testing
add_definitions(-DHOST_TEST_BUILD)

# Include directories
include_directories(
    components/simplet/include
    components/simplet/test
)

# Build Unity testing framework from source
add_library(unity STATIC
    components/simplet/test/unity/unity.c
)

target_include_directories(unity PUBLIC
    components/simplet/test/unity
)

# Set Unity include directory
set(UNITY_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/components/simplet/test/unity)
include_directories(${UNITY_INCLUDE_DIR})

# Build the simplet library
add_library(simplet STATIC
    components/simplet/simplet.c
)

target_include_directories(simplet PUBLIC
    components/simplet/include
)

# Create individual test executables using Unity RUN_TEST macros

# test_hello_world executable
add_executable(test_hello_world_exe
    components/simplet/test/test_hello_world.c
    components/simplet/test/test_hello_world_main.c
)

target_link_libraries(test_hello_world_exe simplet unity)

target_include_directories(test_hello_world_exe PRIVATE
    ${UNITY_INCLUDE_DIR}
    components/simplet/include
    components/simplet/test
)

# test_stunt_dict executable
add_executable(test_stunt_dict_exe
    components/simplet/test/test_stunt_dict.c
    components/simplet/test/test_stunt_dict_main.c
)

target_link_libraries(test_stunt_dict_exe simplet unity)

target_include_directories(test_stunt_dict_exe PRIVATE
    ${UNITY_INCLUDE_DIR}
    components/simplet/include
    components/simplet/test
)

# Add the individual tests to CTest
add_test(NAME test_hello_world COMMAND test_hello_world_exe)
add_test(NAME test_stunt_dict COMMAND test_stunt_dict_exe)

