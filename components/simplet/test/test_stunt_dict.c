#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef HOST_TEST_BUILD
    #include "unity.h"
    #define TEST_CASE(name, tags) void test_##name(void)
#else
    #include "unity.h"
#endif

#include "stunt_dict.h"

TEST_CASE(stunt_dict_creates_and_destroys_correctly, "[stunt_dict]") {
    stunt_dict_t* dict = create_stunt_dict(ABRIDGED);
    TEST_ASSERT_NOT_NULL(dict);
    destroy_stunt_dict(dict);
}

TEST_CASE(stunt_dict_adds_and_retrieves_values, "[stunt_dict]") {
    stunt_dict_t* dict = create_stunt_dict(ABRIDGED);
    TEST_ASSERT_NOT_NULL(dict);

    TEST_ASSERT_TRUE(stunt_dict_add(dict, "key1", "value1"));
    TEST_ASSERT_TRUE(stunt_dict_add(dict, "key2", "value2"));

    const char* value1 = stunt_dict_find(dict, "key1");
    TEST_ASSERT_NOT_NULL(value1);
    TEST_ASSERT_EQUAL_STRING("value1", value1);

    const char* value2 = stunt_dict_find(dict, "key2");
    TEST_ASSERT_NOT_NULL(value2);
    TEST_ASSERT_EQUAL_STRING("value2", value2);

    destroy_stunt_dict(dict);
}

TEST_CASE(stunt_dict_handles_non_existent_keys, "[stunt_dict]") {
    stunt_dict_t* dict = create_stunt_dict(ABRIDGED);
    TEST_ASSERT_NOT_NULL(dict);

    const char* value = stunt_dict_find(dict, "non_existent");
    TEST_ASSERT_NULL(value);

    destroy_stunt_dict(dict);
}

TEST_CASE(stunt_dict_handles_duplicate_keys, "[stunt_dict]") {
    stunt_dict_t* dict = create_stunt_dict(ABRIDGED);
    TEST_ASSERT_NOT_NULL(dict);

    TEST_ASSERT_TRUE(stunt_dict_add(dict, "key", "original"));
    TEST_ASSERT_TRUE(stunt_dict_add(dict, "key", "updated"));

    const char* value = stunt_dict_find(dict, "key");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("updated", value);

    destroy_stunt_dict(dict);
}

TEST_CASE(stunt_dict_handles_empty_values, "[stunt_dict]") {
    stunt_dict_t* dict = create_stunt_dict(ABRIDGED);
    TEST_ASSERT_NOT_NULL(dict);

    TEST_ASSERT_TRUE(stunt_dict_add(dict, "empty", ""));

    const char* value = stunt_dict_find(dict, "empty");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("", value);

    destroy_stunt_dict(dict);
}

TEST_CASE(stunt_dict_handles_special_characters_in_values, "[stunt_dict]") {
    stunt_dict_t* dict = create_stunt_dict(ABRIDGED);
    TEST_ASSERT_NOT_NULL(dict);

    const char* special_value = "<script>alert('test')</script>";
    TEST_ASSERT_TRUE(stunt_dict_add(dict, "special", special_value));

    const char* value = stunt_dict_find(dict, "special");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING(special_value, value);

    destroy_stunt_dict(dict);
}
