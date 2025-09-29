#ifndef STUNT_DICT_H
#define STUNT_DICT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Todo: Add macros if < C11 or boolean, move to stuntlib-shared.h?
#define TRUE 1
#define FALSE 0

// strcmp Polish
#define SAME_STRING 0

// Friendly Zeroes
#define DEFAULT 0

// Friendly Dictionary Hash Sizes
#define ABRIDGED 16
#define UNABRIDGED 128
#define WEBSTERS 256
#define OXFORD 512

typedef struct dict_entry {
    const char *key;
    const char *value;
    struct dict_entry *next;
} dict_entry_t;

typedef struct {
    dict_entry_t **dict_pages;
    size_t size;
    size_t entries;
} stunt_dict_t;

static inline unsigned long hash_key(const char *key, const size_t entries) {

    unsigned long hash = 5381;
    int character;

    while ((character = *key++)) {
        hash = ((hash << 5) + hash) + character;
    }
    return hash % entries;
}

static inline stunt_dict_t* create_stunt_dict(size_t entries) {

    if (entries < ABRIDGED) entries = ABRIDGED;

    stunt_dict_t *stunt_dict = malloc(sizeof(stunt_dict_t));
    if (!stunt_dict) return NULL;

    stunt_dict->dict_pages = calloc(entries, sizeof(dict_entry_t*));
    if (!stunt_dict->dict_pages) {
        free(stunt_dict);
        return NULL;
    }

    stunt_dict->entries = entries;
    stunt_dict->size = 0;

    return stunt_dict;
}

static inline int stunt_dict_add(stunt_dict_t *stunt_dict, const char *key, const char *value) {

    if (!stunt_dict || !key || !value) return FALSE;

    const unsigned long index = hash_key(key, stunt_dict->entries);
    dict_entry_t *current_entry = stunt_dict->dict_pages[index];

    while (current_entry) {
        if (strcmp(current_entry->key, key) == SAME_STRING) {
            current_entry->value = value;
            return TRUE; // Successfully updated existing entry
        }
        current_entry = current_entry->next;
    }

    if (stunt_dict->size >= stunt_dict->entries) return FALSE;

    dict_entry_t *new_entry = malloc(sizeof(dict_entry_t));
    if (!new_entry) return FALSE;

    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = stunt_dict->dict_pages[index];
    stunt_dict->dict_pages[index] = new_entry;
    stunt_dict->size++;

    return TRUE;
}

static inline int stunt_dict_remove(stunt_dict_t *stunt_dict, const char *key) {

    if (!stunt_dict || !key) return FALSE;

    const unsigned long index = hash_key(key, stunt_dict->entries);
    dict_entry_t *current_entry = stunt_dict->dict_pages[index];
    dict_entry_t *prev = NULL;

    while (current_entry) {
        if (strcmp(current_entry->key, key) == SAME_STRING) {
            if (prev) {
                prev->next = current_entry->next;
            } else {
                stunt_dict->dict_pages[index] = current_entry->next;
            }

            free(current_entry);
            stunt_dict->size--;
            return TRUE;
        }
        prev = current_entry;
        current_entry = current_entry->next;
    }
    return FALSE;
}

static inline int stunt_dict_replace(stunt_dict_t *stunt_dict, const char *key, const char *update_value) {

    if (!stunt_dict || !key) return FALSE;

    const unsigned long index = hash_key(key, stunt_dict->entries);
    dict_entry_t *current_entry = stunt_dict->dict_pages[index];

    while (current_entry) {
        if (strcmp(current_entry->key, key) == SAME_STRING) {
            current_entry->value = update_value;
            return TRUE; // Successfully replaced
        }
        current_entry = current_entry->next;
    }
    return FALSE; // Key not found
}

static inline const char* stunt_dict_find(stunt_dict_t *stunt_dict, const char *key) {

    if (!stunt_dict || !key) return NULL;

    const unsigned long index = hash_key(key, stunt_dict->entries);
    const dict_entry_t *current = stunt_dict->dict_pages[index];

    while (current) {
        if (strcmp(current->key, key) == SAME_STRING) {
            return current->value;
        }
        current = current->next;
    }
    return NULL; // Key not found
}

static inline void destroy_stunt_dict(stunt_dict_t *table) {
    if (!table) return;

    for (size_t i = 0; i < table->entries; i++) {
        dict_entry_t *current = table->dict_pages[i];
        while (current) {
            dict_entry_t *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(table->dict_pages);
    free(table);
}

static inline void print_stunt_dict(stunt_dict_t *stunt_dict) {
    if (!stunt_dict) return;

    printf("Stunt Dict (size: %zu, entries: %zu):\n", stunt_dict->size, stunt_dict->entries);
    for (size_t i = 0; i < stunt_dict->entries; i++) {
        const dict_entry_t *current_bucket = stunt_dict->dict_pages[i];
        if (current_bucket) {
            printf("Bucket %zu: ", i);
            while (current_bucket) {
                printf("(%s: %s) ", current_bucket->key, current_bucket->value);
                current_bucket = current_bucket->next;
            }
            printf("\n");
        }
    }
}

#endif // STUNT_DICT_H
