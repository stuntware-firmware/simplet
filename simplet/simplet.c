
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include "include/simplet.h"

// Template delimiters
#define DELIMITER_START "{{"
#define DELIMITER_END "}}"
#define DELIMITER_LENGTH 2

// Buffer sizing constants
#define INITIAL_BUFFER_MULTIPLIER 2
#define GROWTH_FACTOR 2
#define MAX_STACK_KEY_SIZE 256

// Compile-time assertions for assumptions
_Static_assert(sizeof(char) == 1, "char must be 1 byte");
_Static_assert(DELIMITER_LENGTH == 2, "Delimiter length mismatch");

/* Helper function to ensure buffer has required capacity
 * Returns: pointer to buffer on success, NULL on failure (original buffer is freed)
 */
static inline char* ensure_buffer_capacity(char *buffer, size_t *capacity,
                                          size_t required_size) {
    if (required_size >= *capacity) {
        // Calculate new capacity with overflow check
        size_t new_capacity = *capacity;
        while (new_capacity < required_size) {
            if (new_capacity > SIZE_MAX / GROWTH_FACTOR) {
                free(buffer);
                return NULL; // Overflow protection
            }
            new_capacity *= GROWTH_FACTOR;
        }

        char *new_buffer = realloc(buffer, new_capacity);
        if (!new_buffer) {
            free(buffer);
            return NULL;
        }
        *capacity = new_capacity;
        return new_buffer;
    }
    return buffer;
}

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
 * Returns: newly allocated string with substitutions, or NULL on error
 */
char* simplet_render_html(const char * restrict html_template,
                          const simplet_dictionary_t * restrict dictionary) {
    // Input validation
    if (!html_template || !dictionary) {
        return NULL;
    }

    const size_t html_length = strlen(html_template);
    if (html_length == 0) {
        // Return empty string for empty input
        char *empty = malloc(1);
        if (empty) empty[0] = '\0';
        return empty;
    }

    // Initial buffer allocation with reasonable size estimate
    size_t buffer_capacity = html_length * INITIAL_BUFFER_MULTIPLIER;
    if (buffer_capacity < html_length) { // Overflow check
        buffer_capacity = SIZE_MAX / 2;
    }

    char *output_buffer = malloc(buffer_capacity);
    if (!output_buffer) {
        return NULL;
    }

    size_t output_length = 0;
    size_t position = 0;

    // Stack-allocated buffer for small keys (common case optimization)
    _Alignas(16) char key_stack_buffer[MAX_STACK_KEY_SIZE];

    while (position < html_length) {
        // Check for template delimiter start
        if (position + DELIMITER_LENGTH <= html_length &&
            memcmp(html_template + position, DELIMITER_START, DELIMITER_LENGTH) == 0) {

            // Skip opening delimiter and whitespace
            size_t key_start = skip_whitespace(html_template,
                                              position + DELIMITER_LENGTH,
                                              html_length);

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
                size_t key_actual_end = skip_trailing_whitespace(html_template,
                                                                key_start,
                                                                key_end);
                size_t key_length = key_actual_end - key_start;

                if (key_length > 0) {
                    // Use stack buffer for small keys, heap for large ones
                    char *key_buffer;
                    int heap_allocated = 0;

                    if (key_length < MAX_STACK_KEY_SIZE) {
                        key_buffer = key_stack_buffer;
                    } else {
                        key_buffer = malloc(key_length + 1);
                        if (!key_buffer) {
                            free(output_buffer);
                            return NULL;
                        }
                        heap_allocated = 1;
                    }

                    // Copy and null-terminate key
                    memcpy(key_buffer, html_template + key_start, key_length);
                    key_buffer[key_length] = '\0';

                    // Look up value in dictionary
                    const char *value = simplet_dictionary_get(dictionary, key_buffer);

                    if (value) {
                        // Substitute with value
                        const size_t value_length = strlen(value);

                        output_buffer = ensure_buffer_capacity(output_buffer,
                                                              &buffer_capacity,
                                                              output_length + value_length + 1);
                        if (!output_buffer) {
                            if (heap_allocated) free(key_buffer);
                            return NULL;
                        }

                        memcpy(output_buffer + output_length, value, value_length);
                        output_length += value_length;
                    } else {
                        // No substitution found, copy original template text
                        size_t template_len = key_end + DELIMITER_LENGTH - position;

                        output_buffer = ensure_buffer_capacity(output_buffer,
                                                              &buffer_capacity,
                                                              output_length + template_len + 1);
                        if (!output_buffer) {
                            if (heap_allocated) free(key_buffer);
                            return NULL;
                        }

                        memcpy(output_buffer + output_length,
                               html_template + position,
                               template_len);
                        output_length += template_len;
                    }

                    if (heap_allocated) {
                        free(key_buffer);
                    }

                    position = key_end + DELIMITER_LENGTH;
                    continue;
                }
            }
        }

        // Copy regular character
        output_buffer = ensure_buffer_capacity(output_buffer,
                                              &buffer_capacity,
                                              output_length + 2);
        if (!output_buffer) {
            return NULL;
        }

        output_buffer[output_length++] = html_template[position++];
    }

    // Null-terminate the result
    output_buffer[output_length] = '\0';

    // Shrink buffer to actual size (optional optimization)
    char *final_buffer = realloc(output_buffer, output_length + 1);
    if (final_buffer) {
        return final_buffer;
    }

    // If realloc fails, return the original buffer (still valid)
    return output_buffer;
}
