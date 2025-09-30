#include "unity.h"

// Unity requires these functions to be defined
void setUp(void) {
    // Set up code here
}

void tearDown(void) {
    // Tear down code here
}

// Forward declare the test functions that are defined in test_hello_world.c
void test_simplet_renders_hello_world_template(void);
void test_simplet_handles_multiple_placeholders(void);
void test_simplet_handles_missing_placeholder_gracefully(void);
void test_simplet_handles_empty_template(void);
void test_simplet_handles_NULL_dictionary(void);

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_simplet_renders_hello_world_template);
    RUN_TEST(test_simplet_handles_multiple_placeholders);
    RUN_TEST(test_simplet_handles_missing_placeholder_gracefully);
    RUN_TEST(test_simplet_handles_empty_template);
    RUN_TEST(test_simplet_handles_NULL_dictionary);

    return UNITY_END();
}