#include <stdio.h>
#include <assert.h>

#define TEST_CASE(name, tags) void test_##name(void)

#include "simplet_dictionary.h"

TEST_CASE(stunt_dict_creates_and_destroys_correctly, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);
    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_adds_and_retrieves_values, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    assert(simplet_dictionary_set(dict, "key1", "value1") == SUCCESS);
    assert(simplet_dictionary_set(dict, "key2", "value2") == SUCCESS);

    const char* value1 = simplet_dictionary_get(dict, "key1");
    assert(value1 != NULL);
    assert(strcmp("value1", value1) == 0);

    const char* value2 = simplet_dictionary_get(dict, "key2");
    assert(value2 != NULL);
    assert(strcmp("value2", value2) == 0);

    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_handles_non_existent_keys, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    const char* value = simplet_dictionary_get(dict, "non_existent");
    assert(value == NULL);

    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_handles_duplicate_keys, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    assert(simplet_dictionary_set(dict, "key", "original") == SUCCESS);
    assert(simplet_dictionary_set(dict, "key", "updated") == SUCCESS);

    const char* value = simplet_dictionary_get(dict, "key");
    assert(value != NULL);
    assert(strcmp("updated", value) == 0);

    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_handles_empty_values, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    assert(simplet_dictionary_set(dict, "empty", "") == SUCCESS);

    const char* value = simplet_dictionary_get(dict, "empty");
    assert(value != NULL);
    assert(strcmp("", value) == 0);

    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_handles_special_characters_in_values, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    const char* special_value = "<script>alert('test')</script>";
    assert(simplet_dictionary_set(dict, "special", special_value) == SUCCESS);

    const char* value = simplet_dictionary_get(dict, "special");
    assert(value != NULL);
    assert(strcmp(special_value, value) == 0);

    destroy_simplet_dictionary(dict);
}
