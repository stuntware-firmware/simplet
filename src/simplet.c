
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/simplet.h"
#include "include/simplet_dictionary.h"

// Template delimiters
#define DELIMITER_START "{{"
#define DELIMITER_END "}}"
#define DELIMITER_LENGTH 2

// Maximum template size (including null terminator)
#define MAX_TEMPLATE_SIZE 8192 + TERMINATOR

// Helper macro for allocating empty strings
#define EMPTY_STRING() ({ char *s = malloc(1); if (s) s[0] = '\0'; s; })

// Compile-time assertions for assumptions
_Static_assert(sizeof(char) == 1, "char must be 1 byte");
_Static_assert(DELIMITER_LENGTH == 2, "Delimiter length mismatch");

/* Helper function to skip whitespace characters
 * Returns: position after whitespace
 */
static inline size_t skip_whitespace(const char *str, size_t pos, size_t max_len) {
    while (pos < max_len && (str[pos] == ' ' || str[pos] == '\t')) {
        pos++;
    }
    return pos;
}

/* Helper function to skip trailing whitespace backwards
 * Returns: position of last non-whitespace character + 1
 */
static inline size_t skip_trailing_whitespace(const char *str, size_t start, size_t end) {
    while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t')) {
        end--;
    }
    return end;
}

/* Renders HTML template with dictionary substitutions
 * Replaces {{key}} placeholders with corresponding dictionary values
 * Parameters:
 *   html_template: input template string (not modified)
 *   dictionary: key-value pairs for substitution
 * Returns: newly allocated string with substitutions, never returns NULL
 */
char* simplet_render_html(const char *html_template, simplet_dictionary_t *dictionary) {

    if (!html_template) {
        return EMPTY_STRING();
    }

    const size_t html_length = safe_strlen(html_template, MAX_TEMPLATE_SIZE);

    if (html_length == SIZE_MAX || html_length == 0) {
        return EMPTY_STRING();
    }

    // Create empty dictionary if none provided
    bool empty_dictionary = false;
    if (!dictionary) {
        dictionary = EMPTY_DICTIONARY();
        empty_dictionary = true;
    }

    // Initial buffer allocation based on template size plus dictionary content
    // Use total_allocated which tracks all malloc'd strings (keys + values)
    size_t total_dict_size = simplet_dictionary_allocated_size(dictionary);
    size_t buffer_capacity = html_length + total_dict_size;
    if (buffer_capacity < html_length || buffer_capacity < total_dict_size) { // Overflow check
        buffer_capacity = SIZE_MAX / 2;
    }

    char *output_buffer = malloc(buffer_capacity);
    if (!output_buffer) {
        return EMPTY_STRING();
    }

    size_t output_length = 0;
    size_t position = 0;

    // Stack-allocated buffer for small keys (common case optimization)
    _Alignas(16) char key_stack_buffer[MAX_KEY_SIZE];

    while (position < html_length) {
        // Check for template delimiter start
        if (position + DELIMITER_LENGTH <= html_length && memcmp(html_template + position, DELIMITER_START, DELIMITER_LENGTH) == 0) {

            // Skip opening delimiter and whitespace
            size_t key_start = skip_whitespace(html_template, position + DELIMITER_LENGTH, html_length);

            // Find closing delimiter
            size_t search_pos = key_start;
            size_t key_end = 0;

            while (search_pos < html_length - 1) {
                if (memcmp(html_template + search_pos, DELIMITER_END, DELIMITER_LENGTH) == 0) {
                    key_end = search_pos;
                    break;
                }
                search_pos++;
             }

            if (key_end > 0) {
                // Trim trailing whitespace from key
                const size_t key_actual_end = skip_trailing_whitespace(html_template, key_start, key_end);
                const size_t key_length = key_actual_end - key_start;

                if (key_length > 0) {
                    // Use stack buffer for small keys, heap for large ones
                    char *key_buffer;
                    int heap_allocated = 0;

                    if (key_length < MAX_KEY_SIZE) {
                        key_buffer = key_stack_buffer;
                    } else {
                        key_buffer = malloc(key_length + 1);
                        if (!key_buffer) {
                            free(output_buffer);
                            if (empty_dictionary) destroy_simplet_dictionary(dictionary);
                            return EMPTY_STRING();
                        }
                        heap_allocated = 1;
                    }

                    // Copy and null-terminate key
                    memcpy(key_buffer, html_template + key_start, key_length);
                    key_buffer[key_length] = '\0';

                    // Look up value in dictionary
                    const char *value = simplet_dictionary_get(dictionary, key_buffer);

                    if (value && value[0] != '\0') {
                        // Substitute with value (only if non-empty) - validate length
                        const size_t value_length = safe_strlen(value, MAX_VALUE_SIZE);
                        if (value_length != SIZE_MAX && value_length > 0) {
                            memcpy(output_buffer + output_length, value, value_length);
                            output_length += value_length;
                        }
                    }
                    // If value is null or empty, render nothing (no key, no value)

                    if (heap_allocated) {
                        free(key_buffer);
                    }

                    position = key_end + DELIMITER_LENGTH;
                    continue;
                }
            }
        }

        // Copy regular character
        output_buffer[output_length++] = html_template[position++];
    }

    // Null-terminate the result
    output_buffer[output_length] = '\0';

    // Clean up temporary dictionary if we created it
    if (empty_dictionary) {
        destroy_simplet_dictionary(dictionary);
    }

    // Shrink buffer to actual size (optional optimization)
    char *final_buffer = realloc(output_buffer, output_length + 1);
    if (final_buffer) {
        return final_buffer;
    }

    // If realloc fails, return the original buffer (still valid)
    return output_buffer;
}
