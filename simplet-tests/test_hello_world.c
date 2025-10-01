#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define TEST_CASE(name, tags) void test_##name(void)
#define ASSERT_NULL_TERMINATED(str) assert((str)[strlen(str)] == '\0')

#include "simplet.h"
#include "simplet_dictionary.h"

TEST_CASE(simplet_renders_hello_world_template, "[simplet]") {
    const char* template_html = "<div><p>{{ hello-world }}</p></div>";
    const char* expected_output = "<div><p>Hello, World!</p></div>";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL && "Failed to create stunt_dictionary");

    simplet_dictionary_error_t result = simplet_dictionary_set(dict, "hello-world", "Hello, World!");
    assert(result == SUCCESS && "Failed to add key-value pair to dictionary");

    char* rendered_html = simplet_render_html(template_html, dict);
    assert(rendered_html != NULL && "Failed to render HTML");
    ASSERT_NULL_TERMINATED(rendered_html);

    assert(strcmp(expected_output, rendered_html) == 0);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}

TEST_CASE(simplet_handles_multiple_placeholders, "[simplet]") {
    const char* template_html = "<h1>{{ title }}</h1><p>{{ content }}</p>";
    const char* expected_output = "<h1>Test Title</h1><p>Test Content</p>";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    assert(simplet_dictionary_set(dict, "title", "Test Title") == SUCCESS);
    assert(simplet_dictionary_set(dict, "content", "Test Content") == SUCCESS);

    char* rendered_html = simplet_render_html(template_html, dict);
    assert(rendered_html != NULL);
    ASSERT_NULL_TERMINATED(rendered_html);
    assert(strcmp(expected_output, rendered_html) == 0);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}

TEST_CASE(simplet_handles_missing_placeholder_gracefully, "[simplet]") {
    const char* template_html = "<div>{{ missing }}</div>";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    char* rendered_html = simplet_render_html(template_html, dict);
    assert(rendered_html != NULL);
    ASSERT_NULL_TERMINATED(rendered_html);

    // When placeholder not found, render nothing (not even the key)
    assert(strcmp("<div></div>", rendered_html) == 0);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}

TEST_CASE(simplet_handles_empty_template, "[simplet]") {
    const char* template_html = "";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    char* rendered_html = simplet_render_html(template_html, dict);
    assert(rendered_html != NULL);
    ASSERT_NULL_TERMINATED(rendered_html);
    assert(strcmp("", rendered_html) == 0);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}

TEST_CASE(simplet_handles_NULL_dictionary, "[simplet]") {
    const char* template_html = "<div>Test</div>";

    char* rendered_html = simplet_render_html(template_html, NULL);

    if (rendered_html) {
        ASSERT_NULL_TERMINATED(rendered_html);
        assert(strcmp("<div>Test</div>", rendered_html) == 0);
        free(rendered_html);
    }
}

TEST_CASE(simplet_handles_empty_string_value, "[simplet]") {
    const char* template_html = "<div>{{ empty }}</div>";

    simplet_dictionary_t* dict = create_simplet_dictionary(SIZE_SMALL, false);
    assert(dict != NULL);

    assert(simplet_dictionary_set(dict, "empty", "") == SUCCESS);

    char* rendered_html = simplet_render_html(template_html, dict);
    assert(rendered_html != NULL);
    ASSERT_NULL_TERMINATED(rendered_html);

    // When value is empty string, render nothing (not even the key)
    assert(strcmp("<div></div>", rendered_html) == 0);

    destroy_simplet_dictionary(dict);
    free(rendered_html);
}
