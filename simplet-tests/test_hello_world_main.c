#include <stdio.h>

// Forward declare the test functions that are defined in test_hello_world.c
void test_simplet_renders_hello_world_template(void);
void test_simplet_handles_multiple_placeholders(void);
void test_simplet_handles_missing_placeholder_gracefully(void);
void test_simplet_handles_empty_template(void);
void test_simplet_handles_NULL_dictionary(void);
void test_simplet_handles_empty_string_value(void);

int main(void) {
    printf("Running simplet tests...\n");

    test_simplet_renders_hello_world_template();
    printf("✓ test_simplet_renders_hello_world_template\n");

    test_simplet_handles_multiple_placeholders();
    printf("✓ test_simplet_handles_multiple_placeholders\n");

    test_simplet_handles_missing_placeholder_gracefully();
    printf("✓ test_simplet_handles_missing_placeholder_gracefully\n");

    test_simplet_handles_empty_template();
    printf("✓ test_simplet_handles_empty_template\n");

    test_simplet_handles_NULL_dictionary();
    printf("✓ test_simplet_handles_NULL_dictionary\n");

    test_simplet_handles_empty_string_value();
    printf("✓ test_simplet_handles_empty_string_value\n");

    printf("\nAll tests passed!\n");
    return 0;
}