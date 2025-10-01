cmake_minimum_required(VERSION 3.16)

project(simplet C)

# Enable testing with CTest
enable_testing()

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define HOST_TEST_BUILD for host-based testing
add_definitions(-DHOST_TEST_BUILD)

# Include directories
include_directories(
        src/include
        simplet-tests
)

# Build Unity testing framework from source
add_library(unity STATIC
        simplet-tests/unity/unity.c
)

target_include_directories(unity PUBLIC
        simplet-tests/unity
)

# Set Unity include directory
set(UNITY_INCLUDE_DIR simplet-tests/unity)
include_directories(${UNITY_INCLUDE_DIR})

# Build the simplet library
add_library(simplet STATIC
        src/simplet.c
)

target_include_directories(simplet PUBLIC
        src/include
)

# Create individual test executables using Unity RUN_TEST macros

# test_hello_world executable
add_executable(test_hello_world_unit
        simplet-tests/test_hello_world.c
        simplet-tests/test_hello_world_main.c
)

target_link_libraries(test_hello_world_unit simplet unity)

target_include_directories(test_hello_world_unit PRIVATE
    ${UNITY_INCLUDE_DIR}
        src/include
        simplet-tests
)

# test_simplet_dictionary executable
add_executable(test_simplet_dictionary_unit
        simplet-tests/test_simplet_dictionary.c
        simplet-tests/test_simplet_dictionary_main.c
)

target_link_libraries(test_simplet_dictionary_unit simplet unity)

target_include_directories(test_simplet_dictionary_unit PRIVATE
    ${UNITY_INCLUDE_DIR}
        src/include
        simplet-tests
)

# Add the individual tests to CTest
add_test(NAME test_hello_world COMMAND test_hello_world_unit)
add_test(NAME test_simplet_dictionary COMMAND test_simplet_dictionary_unit)

# Custom target to run tests
add_custom_target(simplet-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_hello_world_unit test_simplet_dictionary_unit
    COMMENT "Running simplet tests"
)

# Custom target to create distribution package
add_custom_target(simplet-dist
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/dist/simplet/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/include ${CMAKE_SOURCE_DIR}/dist/simplet/include
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/simplet.c ${CMAKE_SOURCE_DIR}/dist/simplet/simplet.c
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/CMakeLists.txt ${CMAKE_SOURCE_DIR}/dist/simplet/CMakeLists.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/idf_component.yml ${CMAKE_SOURCE_DIR}/dist/simplet/idf_component.yml
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/README.md ${CMAKE_SOURCE_DIR}/dist/simplet/README.md
    COMMENT "Creating simplet distribution package in dist/simplet/"
)

# Custom target to run tests and create distribution if tests pass
add_custom_target(simplet-component
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/dist
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target simplet-dist
    DEPENDS test_hello_world_unit test_simplet_dictionary_unit
    COMMENT "Cleaning dist, running tests, and creating distribution package if tests pass"
)

# Custom target to clean distribution directory
add_custom_target(simplet-clean-dist
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/dist
    COMMENT "Removing dist directory"
)

