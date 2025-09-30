#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef HOST_TEST_BUILD
    #include "unity.h"
    #define TEST_CASE(name, tags) void test_##name(void)
#else
    #include "unity.h"
#endif

#include "simplet_dictionary.h"

TEST_CASE(stunt_dict_creates_and_destroys_correctly, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);
    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_adds_and_retrieves_values, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);

    TEST_ASSERT_EQUAL_INT(SUCCESS, simplet_dictionary_set(dict, "key1", "value1"));
    TEST_ASSERT_EQUAL_INT(SUCCESS, simplet_dictionary_set(dict, "key2", "value2"));

    const char* value1 = simplet_dictionary_get(dict, "key1");
    TEST_ASSERT_NOT_NULL(value1);
    TEST_ASSERT_EQUAL_STRING("value1", value1);

    const char* value2 = simplet_dictionary_get(dict, "key2");
    TEST_ASSERT_NOT_NULL(value2);
    TEST_ASSERT_EQUAL_STRING("value2", value2);

    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_handles_non_existent_keys, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);

    const char* value = simplet_dictionary_get(dict, "non_existent");
    TEST_ASSERT_NULL(value);

    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_handles_duplicate_keys, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);

    TEST_ASSERT_EQUAL_INT(SUCCESS, simplet_dictionary_set(dict, "key", "original"));
    TEST_ASSERT_EQUAL_INT(SUCCESS, simplet_dictionary_set(dict, "key", "updated"));

    const char* value = simplet_dictionary_get(dict, "key");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("updated", value);

    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_handles_empty_values, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);

    TEST_ASSERT_EQUAL_INT(SUCCESS, simplet_dictionary_set(dict, "empty", ""));

    const char* value = simplet_dictionary_get(dict, "empty");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("", value);

    destroy_simplet_dictionary(dict);
}

TEST_CASE(stunt_dict_handles_special_characters_in_values, "[stunt_dict]") {
    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);

    const char* special_value = "<script>alert('test')</script>";
    TEST_ASSERT_EQUAL_INT(SUCCESS, simplet_dictionary_set(dict, "special", special_value));

    const char* value = simplet_dictionary_get(dict, "special");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(special_value, value);

    destroy_simplet_dictionary(dict);
}
