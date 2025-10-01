#include <stdio.h>

// Forward declare the test functions that are defined in test_simplet_dictionary.c
void test_stunt_dict_creates_and_destroys_correctly(void);
void test_stunt_dict_adds_and_retrieves_values(void);
void test_stunt_dict_handles_non_existent_keys(void);
void test_stunt_dict_handles_duplicate_keys(void);
void test_stunt_dict_handles_empty_values(void);
void test_stunt_dict_handles_special_characters_in_values(void);

int main(void) {
    printf("Running simplet_dictionary tests...\n");

    test_stunt_dict_creates_and_destroys_correctly();
    printf("✓ test_stunt_dict_creates_and_destroys_correctly\n");

    test_stunt_dict_adds_and_retrieves_values();
    printf("✓ test_stunt_dict_adds_and_retrieves_values\n");

    test_stunt_dict_handles_non_existent_keys();
    printf("✓ test_stunt_dict_handles_non_existent_keys\n");

    test_stunt_dict_handles_duplicate_keys();
    printf("✓ test_stunt_dict_handles_duplicate_keys\n");

    test_stunt_dict_handles_empty_values();
    printf("✓ test_stunt_dict_handles_empty_values\n");

    test_stunt_dict_handles_special_characters_in_values();
    printf("✓ test_stunt_dict_handles_special_characters_in_values\n");

    printf("\nAll tests passed!\n");
    return 0;
}