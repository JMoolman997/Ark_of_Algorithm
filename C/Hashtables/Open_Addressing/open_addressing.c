/**
 * @file    open_addressing.c
 * @brief   A modular open addressing hash table implementation for
 *          testing and benchmarking.
 * @author  J.W Moolman
 * @date    2024-10-23
 */

#include <stdio.h>
#include <stdlib.h>
#include "open_addressing.h"

#define PRINT_BUFFER_SIZE 1024

#define DEFAULT_LOAD_FACTOR 0.75
#define DEFAULT_SIZE_MAX 1024
/* NOTE should be a power of 2 */
#define DEFAULT_SIZE_INITIAL 16

/** an entry in the hash table **/
struct htentry {
    int flag; /* 0: empty, 1: occupied, 2: deleted */
    KEY_TYPE key;
    VALUE_TYPE value;
};

/** a hash table container */
struct hashtab {    
    /** a pointer to the underlying table                              */
    HTentry *table;
	/** the current size of the underlying table                       */
    unsigned int size; 
	/** the maximum size the hashmap is allowed to take                */
    unsigned int max_size;
    /** the current number of entries                                  */
    unsigned int num_entries;
	/** the maximum load factor before the underlying table is resized */
    float load_factor;
	/** a pointer to the hash function                                 */
    unsigned int (*hash)(KEY_TYPE key, unsigned int size);
	/** a pointer to the comparison function                           */
	int (*match)(KEY_TYPE key1, KEY_TYPE key2);
    /** a enum for the probing method used in the hashtable            */
	ProbingMethod probing_method;
};

/* --- function prototypes -------------------------------------------------- */

static unsigned int default_hash_function(KEY_TYPE key, unsigned int size);
static int default_key_compare(KEY_TYPE a, KEY_TYPE b);

static int rehash(HashTab *ht);
static unsigned int next2pow(unsigned int n);

/* --- probing prototypes --------------------------------------------------- */

static int linear_probe_insert(HashTab *ht, KEY_TYPE key, VALUE_TYPE value);
static int linear_probe_search(HashTab *ht, KEY_TYPE key);

/* --- hash table interface ------------------------------------------------- */

HashTab *init_ht(
		unsigned int max_size,
		float load_factor,
        unsigned int (*hash)(KEY_TYPE key, unsigned int size),
		int (*match)(KEY_TYPE key1, KEY_TYPE key2),
		ProbingMethod probing_method
) {
    HashTab *self;
    unsigned int i;

    self = (HashTab *)malloc(sizeof(HashTab));
    if (self == NULL) {
        fprintf(stderr, "Hashtable allocation failed");
        exit(EXIT_FAILURE);
    }

    self->size = DEFAULT_SIZE_INITIAL;
    self->max_size = max_size;
    self->load_factor = load_factor;
    self->num_entries = 0;
    self->hash = hash;
    self->match = match;
    self->probing_method = probing_method;

    self->table = (HTentry *)calloc(self->size, sizeof(HTentry));
	if (self->table == NULL) {
		fprintf(stderr, "Hashtable allocation failed");
		exit(EXIT_FAILURE);
	}

	return self;
}

int search_ht(
        HashTab *self,
        KEY_TYPE key
) {
    switch (self->probing_method) {
        case LINEAR:
            return linear_probe_search(self, key);
            break;
        default:
            fprintf(stderr, "Unsupported probing method");
            exit(EXIT_FAILURE);
    }
    /* Should never reach this point */
    return -1;
    
}

VALUE_TYPE fetch_ht(
        HashTab *self,
        unsigned int index
) {
   /** TODO:
    * - Add index validation
    * - Handel err
    */
    return self->table[index].value;
}

int insert_ht(
        HashTab *self,
        KEY_TYPE key,
        VALUE_TYPE value
) {
    int search_status;
    float lamda;

    /** TODO: 
     * - Add err and handling for out of space
     * - Add Max size check, and check resize allowed
     * - Retry or exit depending on inner function err
     */   
    
    search_status = search_ht(self, key);
    if (search_status != -1) {
        return HASH_TABLE_KEY_VALUE_PAIR_EXISTS;
    }

    /* Rehash if necessary befor inserting */
    lamda = (float)(self->num_entries + 1) / self->size;
    if (lamda >= self->load_factor) {
        rehash(self);
    }

    switch (self->probing_method) {
        case LINEAR:
            if (linear_probe_insert(self, key, value) != EXIT_SUCCESS) {
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        default:
            fprintf(stderr, "Unsupported probing method");
            exit(EXIT_FAILURE);
    }
    /* If this point reached function is not behaving as it should */ 
    return EXIT_FAILURE;
}

int remove_ht(
        HashTab *self,
        KEY_TYPE key
) {
    int key_index;

    key_index = search_ht(self, key);
    if (key_index < 0) {
        return HASH_TABLE_KEY_NOT_FOUND;
    }

    /* if flag 2 whe no longer consider it no need to update actual values */
    self->table[key_index].flag = 2;
    self->num_entries = self->num_entries - 1;
    return EXIT_SUCCESS;
}

int free_ht(
		HashTab *self,
		void (*freekey)(KEY_TYPE k),
		void (*freeval)(VALUE_TYPE v)
) {
    unsigned int i;

	if (self == NULL) {
		return EXIT_FAILURE;
	}
    
    /* If key or value dynamical allocated */
    if (freekey || freeval) {
        for (i = 0; i < self->size; i++) {
            if (self->table[i].flag == 1) {
                if (freekey) {
                    freekey(self->table[i].key);
                }
                if (freeval) {
                    freeval(self->table[i].value);
                }
            }
        }
    }

	free(self->table);
	self->table = NULL;
	self->hash = NULL;
	self->match = NULL;
	free(self);

	return EXIT_SUCCESS;
}

void print_ht(
        HashTab *self,
        void (*keyval2str)(int flag, KEY_TYPE k, VALUE_TYPE v, char *b)
) {
    unsigned int i;
    HTentry p;
    char buffer[PRINT_BUFFER_SIZE];
    /** TODO:
     * - rework to take an function pointer to a function that takes pointer to
     *   HTentry 
     */   
    
    if (self && keyval2str) {
        printf(
                "--- HashTab - size[%d] - entries[%u] - loadfct[%.2f] ---",
                self->size,
                self->num_entries,
                self->load_factor
        );

        for (i = 0; i < self->size; i++) {
            p = self->table[i];
            /* Check how this works with different macros */
            keyval2str(p.flag, p.key, p.value, buffer);
            printf("Index %u: %s\n", i, buffer);
        }
    }

}

/* --- probing functions ---------------------------------------------------- */

/**
 * linear_probe_search - Searches for a key in the hash table using linear probing.
 * 
 * @param self: Pointer to the hash table (HashTab*) where the search is performed.
 * @param key: The key to search for (KEY_TYPE).
 * 
 * @return: 
 *  - The index of the entry where the key is found.
 *  - Returns `-1` if the key is not found.
 */
static int linear_probe_search(
        HashTab *ht,
        KEY_TYPE key
) {
    /** TODO:
     * - Free memory on error */

    int match_found;
    unsigned int hash_index, current_index;

    match_found = 0;
    hash_index = ht->hash(key, ht->size);
    current_index = hash_index;
    do {
        switch(ht->table[current_index].flag) {
            case 0:
                return -1;
            case 2:
                break;
            case 1:
                match_found = ht->match(ht->table[current_index].key, key);
                if (match_found == 0) {
                    return current_index;
                }
                break;
            default:
                fprintf(
                        stderr,
                        "Invalid flag found in entry at index:%d",
                        current_index
                );
                exit(EXIT_FAILURE);

        }
        current_index = (current_index + 1) % ht->size;
    } while (current_index != hash_index);

    return -1;
}

static int linear_probe_insert(
        HashTab *ht,
        KEY_TYPE key,
        VALUE_TYPE value
) {

    unsigned int hash_index, current_index;
    
    /** TODO:
     * - Free memory on error */

    hash_index = ht->hash(key, ht->size);
    current_index = hash_index;
    do {
        switch(ht->table[current_index].flag) {
            case 0:
                goto insert_entry;
            case 2:
                goto insert_entry;
            case 1:
                continue;
            default:
                fprintf(
                        stderr,
                        "Invalid flag found in entry at index:%d",
                        current_index
                );
                exit(EXIT_FAILURE);

        }
        current_index = (current_index + 1) % ht->size;
    } while (current_index != hash_index);

    return -1;

insert_entry:
    ht->table[current_index].flag = 1;
    ht->table[current_index].key = key;
    ht->table[current_index].value = value;
    ht->num_entries = ht->num_entries + 1;
    return 0;
}

/* --- utility functions ---------------------------------------------------- */

static int rehash(
        HashTab *ht
) {
    HTentry *old_table, *new_table, empty_entry, current_entry;
    int insert_status;
    unsigned int i, old_size, new_size;
    KEY_TYPE key;
    VALUE_TYPE value;

    old_size = ht->size;
    old_table = ht->table;

    /* Allocate and populate table with blank entries */
    new_size = next2pow(old_size);
    new_table = (HTentry *)calloc(new_size, sizeof(HTentry));
    empty_entry.flag = 0;
    for (i = 0; i < new_size; i++) {
        new_table[i] = empty_entry;
    }

    /* Update table propetries */
    ht->table = new_table;
    ht->size = new_size;
    ht->num_entries = 0;

    for (i = 0; i < old_size; i++) {
        current_entry = old_table[i];
        if (current_entry.flag == 1) {
            insert_status = insert_ht(
                    ht,
                    current_entry.key,
                    current_entry.value
            );
            /* NOTE: - This is a bad way to do this */
            if (insert_status != EXIT_SUCCESS) {
                fprintf(stderr, "Insert in rehash failed");
            }
        }
    }
    ht->table = new_table;
    free(old_table);
    return EXIT_SUCCESS;
}

/**
 * next2pow - Computes the next power of 2 greater than or equal to `n`.
 * 
 * Process:
 * 1. If `n` is 0, return 1.
 * 2. Subtract 1 from `n` to handle exact powers of 2.
 * 3. Use bit shifts and ORs to fill all bits below the most significant bit.
 * 4. Add 1 to compute the next power of 2.
 * 
 * 
 * Example: For n = 23 (0b10111)
 *    - Subtract 1: n = 22 (0b10110)
 *    - Bit shifts and ORs:
 *      n |= n >> 1 -> 0b10110 | 0b01011 = 0b11111
 *    - Add 1: 11111 + 1 = 0b100000 = 32
 *
 * @param n: The input unsigned integer.
 * @return: The next power of 2 greater than or equal to `n`.
 */
static unsigned int next2pow(
        unsigned int n
) {
    if (n == 0) {
        return 1;
    }

    n--;

    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;

    return n + 1;
}
    
/* --- default functions ---------------------------------------------------- */

/* Default hash function (very bad dont use) */
static unsigned int default_hash_function(KEY_TYPE key, unsigned int size) {
    return key % size;
}

static /* Default key comparison function */
int default_key_compare(KEY_TYPE a, KEY_TYPE b) {
    return a - b;
}

/* --- Test functions ------------------------------------------------------- */

void keyval2str(int flag, KEY_TYPE k, VALUE_TYPE v, char *buffer) {
    if (flag == 1) {
        sprintf(buffer, "Key: %d, Value: %d", (int)k, (int)v);
    } else if (flag == 2) {
        sprintf(buffer, "Deleted");
    } else {
        sprintf(buffer, "Empty");
    }
}

int main() {
    HashTab *ht = init_ht(
        DEFAULT_SIZE_MAX,
        DEFAULT_LOAD_FACTOR,
        default_hash_function,
        default_key_compare,
        LINEAR
    );

    if (ht == NULL) {
        fprintf(stderr, "Failed to initialize hash table\n");
        return EXIT_FAILURE;
    }

    printf("Inserting key-value pairs...\n");
    for (int i = 0; i < 10; i++) {
        int key = i;
        int value = i * 10;
        if (insert_ht(ht, key, value) != EXIT_SUCCESS) {
            fprintf(stderr, "Failed to insert key %d\n", key);
        }
    }

    printf("\nSearching for keys...\n");
    for (int i = 0; i < 12; i++) {
        int index = search_ht(ht, i);
        if (index != -1) {
            int value = fetch_ht(ht, index);
            printf("Key %d found with value %d at index %d\n", i, value, index);
        } else {
            printf("Key %d not found\n", i);
        }
    }

    int key_to_remove = 5;
    printf("\nRemoving key %d...\n", key_to_remove);
    if (remove_ht(ht, key_to_remove) == EXIT_SUCCESS) {
        printf("Key %d removed successfully\n", key_to_remove);
    } else {
        printf("Key %d not found or could not be removed\n", key_to_remove);
    }

    int index = search_ht(ht, key_to_remove);
    if (index != -1) {
        int value = fetch_ht(ht, index);
        printf("Key %d found with value %d at index %d\n", key_to_remove, value, index);
    } else {
        printf("Key %d not found after removal\n", key_to_remove);
    }

    printf("\nCurrent state of the hash table:\n");
    print_ht(ht, keyval2str);

    if (free_ht(ht, NULL, NULL) != EXIT_SUCCESS) {
        fprintf(stderr, "Failed to free hash table\n");
        return EXIT_FAILURE;
    }

    printf("\nHash table tests completed successfully.\n");
    return EXIT_SUCCESS;
}
