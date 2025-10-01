#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef HOST_TEST_BUILD
    #include "unity.h"
    #define TEST_CASE(name, tags) void test_##name(void)
#else
    #include "unity.h"
#endif

#include "simplet.h"
#include "simplet_dictionary.h"

TEST_CASE(simplet_renders_hello_world_template, "[simplet]") {
    const char* template_html = "<div><p>{{ hello-world }}</p></div>";
    const char* expected_output = "<div><p>Hello, World!</p></div>";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL_MESSAGE(dict, "Failed to create stunt_dictionary");

    simplet_dictionary_error_t result = simplet_dictionary_set(dict, "hello-world", "Hello, World!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(SUCCESS, result, "Failed to add key-value pair to dictionary");

    char* rendered_html = simplet_render_html((char*)template_html, dict);
    TEST_ASSERT_NOT_NULL_MESSAGE(rendered_html, "Failed to render HTML");

    TEST_ASSERT_EQUAL_STRING(expected_output, rendered_html);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}

TEST_CASE(simplet_handles_multiple_placeholders, "[simplet]") {
    const char* template_html = "<h1>{{ title }}</h1><p>{{ content }}</p>";
    const char* expected_output = "<h1>Test Title</h1><p>Test Content</p>";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);

    TEST_ASSERT_EQUAL_INT(SUCCESS, simplet_dictionary_set(dict, "title", "Test Title"));
    TEST_ASSERT_EQUAL_INT(SUCCESS, simplet_dictionary_set(dict, "content", "Test Content"));

    char* rendered_html = simplet_render_html((char*)template_html, dict);
    TEST_ASSERT_NOT_NULL(rendered_html);
    TEST_ASSERT_EQUAL_STRING(expected_output, rendered_html);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}

TEST_CASE(simplet_handles_missing_placeholder_gracefully, "[simplet]") {
    const char* template_html = "<div>{{ missing }}</div>";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);

    char* rendered_html = simplet_render_html((char*)template_html, dict);
    TEST_ASSERT_NOT_NULL(rendered_html);

    // When placeholder not found, it should remain as-is
    TEST_ASSERT_EQUAL_STRING("<div>{{ missing }}</div>", rendered_html);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}

TEST_CASE(simplet_handles_empty_template, "[simplet]") {
    const char* template_html = "";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    TEST_ASSERT_NOT_NULL(dict);

    char* rendered_html = simplet_render_html((char*)template_html, dict);
    TEST_ASSERT_NOT_NULL(rendered_html);
    TEST_ASSERT_EQUAL_STRING("", rendered_html);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}

TEST_CASE(simplet_handles_NULL_dictionary, "[simplet]") {
    const char* template_html = "<div>Test</div>";

    char* rendered_html = simplet_render_html((char*)template_html, NULL);

    if (rendered_html) {
        TEST_ASSERT_EQUAL_STRING("<div>Test</div>", rendered_html);
        free(rendered_html);
    }
}
