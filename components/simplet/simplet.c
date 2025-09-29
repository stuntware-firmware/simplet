
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/simplet.h"

/* This function should take a normal string and stunt_dict_t
 * and replace the matching key name between the {{ }} in the html
 * string with the value for the key in the stunt_dict_t.
 * example function call: char* simplet_render_html(char *html, stunt_dict_t *stunt_dict)
*/
char* simplet_render_html(char *html, stunt_dict_t *stunt_dict) {
    if (!html || !stunt_dict) return NULL;

    size_t html_len = strlen(html);
    size_t buffer_size = html_len * 2;
    char *result = malloc(buffer_size);
    if (!result) return NULL;

    size_t result_pos = 0;
    size_t i = 0;

    while (i < html_len) {
        if (i + 1 < html_len && html[i] == '{' && html[i + 1] == '{') {
            size_t start = i + 2;
            while (start < html_len && (html[start] == ' ' || html[start] == '\t')) {
                start++;
            }

            size_t end = start;
            while (end < html_len - 1 && !(html[end] == '}' && html[end + 1] == '}')) {
                end++;
            }

            if (end < html_len - 1 && html[end] == '}' && html[end + 1] == '}') {
                size_t key_end = end;
                while (key_end > start && (html[key_end - 1] == ' ' || html[key_end - 1] == '\t')) {
                    key_end--;
                }

                size_t key_len = key_end - start;
                char *key = malloc(key_len + 1);
                if (key) {
                    strncpy(key, html + start, key_len);
                    key[key_len] = '\0';

                    const char *value = stunt_dict_find(stunt_dict, key);

                    if (value) {
                        size_t value_len = strlen(value);

                        while (result_pos + value_len >= buffer_size) {
                            buffer_size *= 2;
                            char *new_result = realloc(result, buffer_size);
                            if (!new_result) {
                                free(key);
                                free(result);
                                return NULL;
                            }
                            result = new_result;
                        }

                        strcpy(result + result_pos, value);
                        result_pos += value_len;
                    } else {
                        size_t template_len = end + 2 - i;
                        while (result_pos + template_len >= buffer_size) {
                            buffer_size *= 2;
                            char *new_result = realloc(result, buffer_size);
                            if (!new_result) {
                                free(key);
                                free(result);
                                return NULL;
                            }
                            result = new_result;
                        }

                        strncpy(result + result_pos, html + i, template_len);
                        result_pos += template_len;
                    }

                    free(key);
                    i = end + 2;
                } else {
                    free(result);
                    return NULL;
                }
            } else {
                if (result_pos >= buffer_size - 1) {
                    buffer_size *= 2;
                    char *new_result = realloc(result, buffer_size);
                    if (!new_result) {
                        free(result);
                        return NULL;
                    }
                    result = new_result;
                }
                result[result_pos++] = html[i++];
            }
        } else {
            if (result_pos >= buffer_size - 1) {
                buffer_size *= 2;
                char *new_result = realloc(result, buffer_size);
                if (!new_result) {
                    free(result);
                    return NULL;
                }
                result = new_result;
            }
            result[result_pos++] = html[i++];
        }
    }

    result[result_pos] = '\0';

    char *final_result = realloc(result, result_pos + 1);
    return final_result ? final_result : result;
}
