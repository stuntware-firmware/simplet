#ifndef SIMPLET_DICTIONARY_H
#define SIMPLET_DICTIONARY_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// C version feature detection
#if __STDC_VERSION__ >= 201112L
    #define HAS_C11 1
#else
    #define HAS_C11 0
#endif

#if __STDC_VERSION__ >= 201710L
    #define HAS_C17 1
#else
    #define HAS_C17 0
#endif

// Compile-time assertions for structure validation
#if HAS_C11
    _Static_assert(sizeof(size_t) >= 4, "size_t must be at least 32 bits");
    _Static_assert(sizeof(void*) >= 4, "Pointer size must be at least 32 bits");
#endif

#define TERMINATOR 1

/**
 * Safely check if string is null-terminated within max_len bytes
 * @param str String to check
 * @param max_len Maximum bytes to check
 * @return Length if null-terminated within bounds, SIZE_MAX otherwise
 */
static inline size_t safe_strlen(const char *str, size_t max_len) {
    if (!str) return SIZE_MAX;
    for (size_t i = 0; i < max_len; i++) {
        if (str[i] == '\0') return i;
    }
    return SIZE_MAX; // Not null-terminated within bounds
}

// Maximum key size (including null terminator)
#define MAX_KEY_SIZE 64 + TERMINATOR
#define MAX_VALUE_SIZE 1024 + TERMINATOR

// Error codes enumeration
typedef enum {
    SUCCESS = 0,
    ERROR_NULL_PARAM = -1,
    ERROR_NO_MEMORY = -2,
    ERROR_KEY_EXISTS = -3,
    ERROR_KEY_NOT_FOUND = -4,
    ERROR_INVALID_SIZE = -5,
    ERROR_RESIZE_FAILED = -6,
    ERROR_KEY_TOO_LONG = -7
} simplet_dictionary_error_t;

// Predefined dictionary sizes (must be prime numbers for better hash distribution)
typedef enum {
    SIZE_TINY = 17,      // Small embedded applications
    SIZE_SMALL = 127,    // Typical small dictionaries
    SIZE_MEDIUM = 509,   // Medium-sized applications
    SIZE_LARGE = 2039,   // Large dictionaries
    SIZE_HUGE = 8191     // Very large applications
} simplet_dictionary_size_t;

// Load factor thresholds
#define LOAD_FACTOR_MAX 0.75f
#define LOAD_FACTOR_MIN 0.25f

// Helper macro for creating empty dictionaries
#define EMPTY_DICTIONARY() create_simplet_dictionary(SIZE_TINY, false)

// Forward declarations
typedef struct entry entry_t;
typedef struct simplet_dictionary simplet_dictionary_t;

// Optimized entry structure with better cache alignment
struct entry {
    char *key;           // Owned by the dictionary (duplicated)
    char *value;         // Owned by the dictionary (duplicated)
    entry_t *next;       // Next entry in chain
    uint32_t hash;       // Cached hash value for faster comparisons
};

// Main dictionary structure
struct simplet_dictionary {
    entry_t **buckets;          // Array of bucket pointers
    size_t bucket_count;        // Number of buckets
    size_t entry_count;         // Number of entries
    size_t resize_threshold;    // Threshold for automatic resize
    size_t total_allocated;     // Total bytes allocated for keys and values
    bool auto_resize;           // Enable automatic resizing
};

// Compile-time validation of structure sizes
#if HAS_C11
    _Static_assert(sizeof(entry_t) <= 64, "entry_t should fit in a cache line");
#endif

/**
 * FNV-1a hash function - better distribution than DJB2
 * @param key The string to hash (must not be NULL)
 * @return 32-bit hash value
 */
static inline uint32_t hash_key(const char *key) {
    uint32_t hash = 2166136261U;  // FNV offset basis

    while (*key) {
        hash ^= (uint32_t)(unsigned char)*key++;
        hash *= 16777619U;  // FNV prime
    }

    return hash;
}

/**
 * Find next prime number (for bucket sizing)
 * @param n Starting number
 * @return Next prime >= n
 */
static inline size_t next_prime(size_t n) {
    if (n <= 2) return 2;
    if (n % 2 == 0) n++;

    while (1) {
        bool is_prime = true;
        for (size_t i = 3; i * i <= n; i += 2) {
            if (n % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) return n;
        n += 2;
    }
}

/**
 * Create a new dictionary with specified initial capacity
 * @param initial_size Initial number of buckets (will be rounded to next prime)
 * @param auto_resize Enable automatic resizing when load factor exceeds threshold
 * @return Pointer to new dictionary or NULL on failure
 */
static inline simplet_dictionary_t* create_simplet_dictionary(size_t initial_size, bool auto_resize) {
    if (initial_size == 0) {
        initial_size = SIZE_TINY;
    }

    // Ensure prime number of buckets for better distribution
    initial_size = next_prime(initial_size);

    simplet_dictionary_t *dict = calloc(1, sizeof(simplet_dictionary_t));
    if (!dict) return NULL;

    dict->buckets = calloc(initial_size, sizeof(entry_t*));
    if (!dict->buckets) {
        free(dict);
        return NULL;
    }

    dict->bucket_count = initial_size;
    dict->entry_count = 0;
    dict->total_allocated = 0;
    dict->auto_resize = auto_resize;
    dict->resize_threshold = (size_t)(initial_size * LOAD_FACTOR_MAX);

    return dict;
}

/**
 * Resize dictionary to new bucket count
 * @param dict Dictionary to resize
 * @param new_bucket_count New number of buckets
 * @return Error code
 */
static simplet_dictionary_error_t resize_simplet_dictionary(simplet_dictionary_t *dict, size_t new_bucket_count) {
    if (!dict) return ERROR_NULL_PARAM;

    new_bucket_count = next_prime(new_bucket_count);
    if (new_bucket_count == dict->bucket_count) return SUCCESS;

    // Allocate new bucket array
    entry_t **new_buckets = calloc(new_bucket_count, sizeof(entry_t*));
    if (!new_buckets) return ERROR_NO_MEMORY;

    // Rehash all entries
    for (size_t i = 0; i < dict->bucket_count; i++) {
        entry_t *entry = dict->buckets[i];
        while (entry) {
            entry_t *next = entry->next;
            size_t new_index = entry->hash % new_bucket_count;
            entry->next = new_buckets[new_index];
            new_buckets[new_index] = entry;
            entry = next;
        }
    }

    // Replace old buckets
    free(dict->buckets);
    dict->buckets = new_buckets;
    dict->bucket_count = new_bucket_count;
    dict->resize_threshold = (size_t)(new_bucket_count * LOAD_FACTOR_MAX);

    return SUCCESS;
}

/**
 * Add or update a key-value pair in the dictionary
 * @param dict Dictionary to modify
 * @param key Key string (will be duplicated internally)
 * @param value Value string (will be duplicated internally)
 * @return Error code
 */
static inline simplet_dictionary_error_t simplet_dictionary_set(simplet_dictionary_t *dict, const char *key, const char *value) {
    if (!dict || !key || !value) return ERROR_NULL_PARAM;

    // Check key length with bounds checking
    size_t key_len = safe_strlen(key, MAX_KEY_SIZE);
    if (key_len == SIZE_MAX || key_len >= MAX_KEY_SIZE) return ERROR_KEY_TOO_LONG;

    uint32_t hash = hash_key(key);
    size_t index = hash % dict->bucket_count;

    // Check if key already exists
    entry_t *entry = dict->buckets[index];
    while (entry) {
        if (entry->hash == hash && strcmp(entry->key, key) == 0) {
            // Update existing entry - validate value length first
            size_t value_len = safe_strlen(value, MAX_VALUE_SIZE);
            if (value_len == SIZE_MAX || value_len >= MAX_VALUE_SIZE) return ERROR_INVALID_SIZE;

            char *new_value = strdup(value);
            if (!new_value) return ERROR_NO_MEMORY;

            // Update allocated size tracking
            size_t old_value_len = safe_strlen(entry->value, MAX_VALUE_SIZE);
            if (old_value_len == SIZE_MAX) old_value_len = 0;  // Defensive fallback
            old_value_len += 1;
            size_t new_value_len = value_len + 1;
            dict->total_allocated = dict->total_allocated - old_value_len + new_value_len;

            free(entry->value);
            entry->value = new_value;
            return SUCCESS;
        }
        entry = entry->next;
    }

    // Check if resize is needed
    if (dict->auto_resize && dict->entry_count >= dict->resize_threshold) {
        simplet_dictionary_error_t err = resize_simplet_dictionary(dict, dict->bucket_count * 2);
        if (err != SUCCESS) return err;
        // Recalculate index after resize
        index = hash % dict->bucket_count;
    }

    // Validate value length before creating entry
    size_t value_len = safe_strlen(value, MAX_VALUE_SIZE);
    if (value_len == SIZE_MAX || value_len >= MAX_VALUE_SIZE) return ERROR_INVALID_SIZE;

    // Create new entry
    entry_t *new_entry = malloc(sizeof(entry_t));
    if (!new_entry) return ERROR_NO_MEMORY;

    new_entry->key = strdup(key);
    if (!new_entry->key) {
        free(new_entry);
        return ERROR_NO_MEMORY;
    }

    new_entry->value = strdup(value);
    if (!new_entry->value) {
        free(new_entry->key);
        free(new_entry);
        return ERROR_NO_MEMORY;
    }

    new_entry->hash = hash;
    new_entry->next = dict->buckets[index];
    dict->buckets[index] = new_entry;
    dict->entry_count++;

    // Track allocated memory (key + value strings including null terminators)
    // key_len already validated above, value_len just validated
    dict->total_allocated += key_len + 1 + value_len + 1;

    return SUCCESS;
}

/**
 * Get value associated with a key
 * @param dictionary Dictionary to search
 * @param key Key to look up
 * @return Value string or NULL if not found
 */
static inline const char* simplet_dictionary_get(const simplet_dictionary_t *dictionary, const char *key) {
    if (!dictionary || !key) return NULL;

    uint32_t hash = hash_key(key);
    size_t index = hash % dictionary->bucket_count;

    const entry_t *entry = dictionary->buckets[index];
    while (entry) {
        if (entry->hash == hash && strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

/**
 * Check if a key exists in the dictionary
 * @param dictionary Dictionary to search
 * @param key Key to check
 * @return true if key exists, false otherwise
 */
static inline bool simplet_dictionary_contains(const simplet_dictionary_t *dictionary, const char *key) {
    return simplet_dictionary_get(dictionary, key) != NULL;
}

/**
 * Remove a key-value pair from the dictionary
 * @param dictionary Dictionary to modify
 * @param key Key to remove
 * @return Error code
 */
static inline simplet_dictionary_error_t simplet_dictionary_remove(simplet_dictionary_t *dictionary, const char *key) {
    if (!dictionary || !key) return ERROR_NULL_PARAM;

    uint32_t hash = hash_key(key);
    size_t index = hash % dictionary->bucket_count;

    entry_t *entry = dictionary->buckets[index];
    entry_t *prev = NULL;

    while (entry) {
        if (entry->hash == hash && strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                dictionary->buckets[index] = entry->next;
            }

            // Update allocated size tracking with safe string length checks
            size_t key_len = safe_strlen(entry->key, MAX_KEY_SIZE);
            size_t val_len = safe_strlen(entry->value, MAX_VALUE_SIZE);
            if (key_len != SIZE_MAX && val_len != SIZE_MAX) {
                dictionary->total_allocated -= (key_len + 1 + val_len + 1);
            }

            free(entry->key);
            free(entry->value);
            free(entry);
            dictionary->entry_count--;

            // Check if dictionary should shrink
            if (dictionary->auto_resize &&
                dictionary->bucket_count > SIZE_SMALL &&
                dictionary->entry_count < (size_t)(dictionary->bucket_count * LOAD_FACTOR_MIN)) {
                resize_simplet_dictionary(dictionary, dictionary->bucket_count / 2);
            }

            return SUCCESS;
        }
        prev = entry;
        entry = entry->next;
    }

    return ERROR_KEY_NOT_FOUND;
}

/**
 * Clear all entries from the dictionary
 * @param dictionary Dictionary to clear
 */
static inline void clear_simplet_dictionary(simplet_dictionary_t *dictionary) {
    if (!dictionary) return;

    for (size_t i = 0; i < dictionary->bucket_count; i++) {
        entry_t *entry = dictionary->buckets[i];
        while (entry) {
            entry_t *next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
        dictionary->buckets[i] = NULL;
    }

    dictionary->entry_count = 0;
    dictionary->total_allocated = 0;
}

/**
 * Destroy dictionary and free all memory
 * @param dict Dictionary to destroy
 */
static inline void destroy_simplet_dictionary(simplet_dictionary_t *dict) {
    if (!dict) return;

    clear_simplet_dictionary(dict);
    free(dict->buckets);
    free(dict);
}

/**
 * Get current load factor of the dictionary
 * @param dictionary Dictionary to analyze
 * @return Load factor (entries/buckets ratio) or 0 on error
 */
static inline float stunt_dict_load_factor(const simplet_dictionary_t *dictionary) {
    if (!dictionary || dictionary->bucket_count == 0) return 0.0f;
    return (float)dictionary->entry_count / (float)dictionary->bucket_count;
}

/**
 * Get number of entries in the dictionary
 * @param dictionary Dictionary to query
 * @return Number of entries or 0 if dict is NULL
 */
static inline size_t simplet_dictionary_count(const simplet_dictionary_t *dictionary) {
    return dictionary ? dictionary->entry_count : 0;
}

/**
 * Get total allocated memory for keys and values
 * @param dictionary Dictionary to query
 * @return Total bytes allocated for all keys and values or 0 if dict is NULL
 */
static inline size_t simplet_dictionary_allocated_size(const simplet_dictionary_t *dictionary) {
    return dictionary ? dictionary->total_allocated : 0;
}

/**
 * Check if dictionary is empty
 * @param dictionary Dictionary to check
 * @return true if empty or NULL, false otherwise
 */
static inline bool simplet_dictionary_is_empty(const simplet_dictionary_t *dictionary) {
    return !dictionary || dictionary->entry_count == 0;
}


#endif // SIMPLET_DICTIONARY_H
