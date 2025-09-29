#include "unity.h"

// Unity requires these functions to be defined
void setUp(void) {
    // Set up code here
}

void tearDown(void) {
    // Tear down code here
}

// Forward declare the test functions that are defined in test_stunt_dict.c
void test_stunt_dict_creates_and_destroys_correctly(void);
void test_stunt_dict_adds_and_retrieves_values(void);
void test_stunt_dict_handles_non_existent_keys(void);
void test_stunt_dict_handles_duplicate_keys(void);
void test_stunt_dict_handles_empty_values(void);
void test_stunt_dict_handles_special_characters_in_values(void);

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_stunt_dict_creates_and_destroys_correctly);
    RUN_TEST(test_stunt_dict_adds_and_retrieves_values);
    RUN_TEST(test_stunt_dict_handles_non_existent_keys);
    RUN_TEST(test_stunt_dict_handles_duplicate_keys);
    RUN_TEST(test_stunt_dict_handles_empty_values);
    RUN_TEST(test_stunt_dict_handles_special_characters_in_values);

    return UNITY_END();
}