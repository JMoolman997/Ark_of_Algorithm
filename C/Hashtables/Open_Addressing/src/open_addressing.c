/**
 * @file    open_addressing.c
 * @brief   A modular open addressing hash table implementation for
 *          testing and benchmarking.
 * @author  J.W Moolman
 * @date    2024-10-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "open_addressing.h"

#define PRINT_BUFFER_SIZE 1024

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
    size_t size;
    size_t used;
    size_t active;
    size_t delta_idx;
    size_t prev_size;
	/** the maximum size the hashmap is allowed to take                */
    size_t max_size;
    size_t min_size;
	/** the maximum load factor before the underlying table is resized */
    float load_factor;
	/** a pointer to the hash function                                 */
    float min_load_factor;
    float inactive_factor;
    unsigned int (*hash)(KEY_TYPE key, unsigned int size);
	/** a pointer to the comparison function                           */
	int (*match)(KEY_TYPE key1, KEY_TYPE key2);
    /** a enum for the probing method used in the hashtable            */
	ProbingMethod probing_method;
};

/* --- prime calculation ---------------------------------------------------- */

/* Use base to make it deterministic for range [0, 2^64] */
uint8_t miller_rabin_base[] = { 2, 3, 5, 7, 11, 13, 17, 19, 31, 37 };

uint32_t delta[] = { 1, 0, 1, 3, 1, 5, 3, 3, 1, 9, 7, 5, 3, 17, 27, 3, 1, 29, 3,
    21, 7, 17, 15, 9, 43, 35, 15, 29, 3, 11 };
        
static uint64_t power_mod(uint64_t base, uint64_t exponent, uint64_t mod);
static uint8_t miller_rabin(uint32_t n);
static uint32_t next_prime_miller_rabin(uint32_t n);
static uint32_t delta_prime(uint32_t n);

/* --- function prototypes -------------------------------------------------- */

static unsigned int default_hash_function(KEY_TYPE key, unsigned int size);
static int default_key_compare(KEY_TYPE a, KEY_TYPE b);

static int rehash(HashTab *ht, uint32_t target);
static int try_downsize(HashTab *ht);

/* --- probing prototypes --------------------------------------------------- */

/** linear probing */
static int linear_probe_insert(HashTab *ht, KEY_TYPE key, VALUE_TYPE value);
static int linear_probe_search(HashTab *ht, KEY_TYPE key);

/** quadratic probing */
static int quadratic_probe_insert(HashTab *ht, KEY_TYPE key, VALUE_TYPE value);
static int quadratic_probe_search(HashTab *ht, KEY_TYPE key);

/** double hashing
 *static int double_hash_insert(HashTab *ht, KEY_TYPE key, VALUE_TYPE value);
 *static int double_hash_search(HashTab *ht, KEY_TYPE);
 */

/* --- hash table interface ------------------------------------------------- */

HashTab *init_ht(
		size_t max_size,
        size_t min_size,
		float load_factor,
        float min_load_factor,
        float inactive_factor,
        unsigned int (*hash)(KEY_TYPE key, unsigned int size),
		int (*match)(KEY_TYPE key1, KEY_TYPE key2),
		ProbingMethod probing_method
) {
    HashTab *self;

    self = (HashTab *)malloc(sizeof(HashTab));
    if (self == NULL) {
        fprintf(stderr, "Hashtable allocation failed");
        exit(EXIT_FAILURE);
    }
    /* Initialize load tracking variables */
    self->used = 0;
    self->active = 0;
    self->delta_idx = 1;  
    self->size = (1 << self->delta_idx) + delta[self->delta_idx];
    self->prev_size = self->size;
    
    /* Initialize HashTable settings */
    if (max_size == 0) {
        self->max_size = DEFAULT_SIZE_MAX;
    } else {
        self->max_size = max_size;
    }
    if (min_size == 0) {
        self->min_size = DEFAULT_SIZE_MIN;
    } else {
        self->min_size = min_size;
    }
    if (load_factor == 0) {
        self->load_factor = DEFAULT_LOAD_FACTOR;
    } else {
        self->load_factor = load_factor;
    }
    if (min_load_factor == 0) {
        self->min_load_factor = DEFAULT_MIN_LOAD_FACTOR;
    } else {
        self->min_load_factor = min_load_factor;
    }
    if (inactive_factor == 0) {
        self->inactive_factor = DEFAULT_INACTIVE_FACTOR;
    } else {
        self->inactive_factor = inactive_factor;
    }

    /* Initialize hashtable functions */
    if (hash == NULL) {
        self->hash = default_hash_function;
    } else {
        self->hash = hash;
    }
    if (match == NULL) {
        self->match = default_key_compare;
    } else {
        self->match = match;
    }

    /* Initialize probing method */
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
        case QUADRATIC:
            return quadratic_probe_search(self, key);
            break;
        default:
            fprintf(stderr, "Unsupported probing method");
            exit(EXIT_FAILURE);
    }
    /* Should never reach this point */
    return HT_INVALID_STATE;
    
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
     * - Check out of bounds delta access
     */   
    
    search_status = search_ht(self, key);
    if (search_status != HT_KEY_NOT_FOUND) {
        return HT_KEY_EXISTS;
    }

    /* Rehash if necessary befor inserting */
    lamda = (float)(self->used + 1) / self->size;
    if (lamda >= self->load_factor) {
        self->prev_size = self->size;
        rehash(self, (1 << (self->delta_idx + 1)) + delta[self->delta_idx + 1]);
        self->delta_idx = self->delta_idx++;
    }
    /* TODO: handle these errs properly */
    switch (self->probing_method) {
        case LINEAR:
            if (linear_probe_insert(self, key, value) != HT_SUCCESS) {
                return HT_FAILURE;
            }
            return HT_SUCCESS;
        case QUADRATIC:
            if (quadratic_probe_insert(self, key, value) != HT_SUCCESS) {
                return HT_FAILURE;
            }
            return HT_SUCCESS;
        default:
            fprintf(stderr, "Unsupported probing method");
            exit(EXIT_FAILURE);
    }
    /* If this point reached function is not behaving as it should */ 
    return HT_INVALID_STATE;
}

int remove_ht(
        HashTab *self,
        KEY_TYPE key
) {
    int key_index;

    key_index = search_ht(self, key);
    if (key_index < 0) {
        return HT_KEY_NOT_FOUND;
    }

    /* if flag 2 we no longer consider it no need to update actual values */
    self->table[key_index].flag = 2;
    /*self->num_entries = self->num_entries - 1;*/
    self->active = self->active--;
    try_downsize(self);
    return HT_SUCCESS;
}

int free_ht(
		HashTab *self,
		void (*freekey)(KEY_TYPE k),
		void (*freeval)(VALUE_TYPE v)
) {
    unsigned int i;

    /* TODO:
     * -check free succesfull and return HT_FAILURE
     */

	if (self == NULL) {
		return HT_INVALID_ARG;
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

	return HT_SUCCESS;
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
                "--- HashTab - size[%d] - entries[%u] - loadfct[%.2f] --- \n",
                self->size,
                self->active,
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

size_t size_ht(
        HashTab *self
) {
    return self->size;
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
                return HT_KEY_NOT_FOUND;
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

    return HT_KEY_NOT_FOUND;
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
                ht->used = ht->used++;
                goto insert_entry;
            case 2:
                goto insert_entry;
            case 1:
                break;
            default:
                fprintf(
                        stderr,
                        "Invalid flag found in entry at index:%d",
                        current_index
                );
                exit(EXIT_FAILURE);

        }
        /* consider initializing table size at start*/
        current_index = (current_index + 1) % ht->size;
    } while (current_index != hash_index);

    return HT_NO_SPACE;

insert_entry:
    ht->table[current_index].flag = 1;
    ht->table[current_index].key = key;
    ht->table[current_index].value = value;
    ht->active = ht->active++;
    return HT_SUCCESS;
}

static int quadratic_probe_search(
        HashTab *ht,
        KEY_TYPE key
) {

    int match_found;
    unsigned int hash_index, probe_index, i, table_size;

    table_size = ht->size;
    i = 0;
    match_found = 0;
    hash_index = ht->hash(key, ht->size);
    probe_index = hash_index;
    do {

        probe_index = (hash_index + ((i + i * i) >> 1)) % table_size;
        switch(ht->table[probe_index].flag) {
            case 0:
                return HT_KEY_NOT_FOUND;
            case 2:
                break;
            case 1:
                match_found = ht->match(ht->table[probe_index].key, key);
                if (match_found == 0) {
                    return probe_index;
                }
                break;
            default:
                fprintf(
                        stderr,
                        "Invalid flag found in entry at index:%d",
                        probe_index
                );
                exit(EXIT_FAILURE);

        }
        i++;
    } while (i < ht->size);

    return HT_KEY_NOT_FOUND;
}

static int quadratic_probe_insert(
        HashTab *ht,
        KEY_TYPE key,
        VALUE_TYPE value
) {
    unsigned int i, hash_index, probe_index, table_size;
    /** TODO:
     * - Update default case err code
     * - Update key not found return code
     */
    table_size = ht->size;
    hash_index = ht->hash(key, table_size);
    i = 0;
    do {
        /* NOTE this wil only guarantee table traversal when table pow of 2 */
        /* probe(i) = (hash(k) + (1/2 * i) + (1/2 * i^2)) mod m */
        probe_index = (hash_index + ((i + i * i) >> 1)) % table_size;
        
        switch(ht->table[probe_index].flag) {
            case 0:
                ht->used = ht->used++;
                goto insert_entry;
            case 2:
                goto insert_entry;
            case 1:
                continue;
            default:
                fprintf(
                        stderr,
                        "Invalid flag found in entry at index:%d",
                        probe_index
                );
                exit(EXIT_FAILURE);
        }
        i++;
    } while (i < table_size);

    return HT_NO_SPACE;

insert_entry:
    ht->table[probe_index].flag = 1;
    ht->table[probe_index].key = key;
    ht->table[probe_index].value = value;
    ht->active = ht->active++;
    return HT_SUCCESS;
}



/* --- utility functions ---------------------------------------------------- */
static int32_t rehash(
        HashTab *ht,
        uint32_t target
) {
    HTentry *old_table, *new_table, empty_entry, current_entry;
    int insert_status;
    uint32_t i, old_size;

    old_size = ht->size;
    old_table = ht->table;

    new_table = (HTentry *)calloc(target, sizeof(HTentry));
    empty_entry.flag = 0;

    /* Update table propetries */
    ht->table = new_table;
    ht->size = target;
    ht->active = 0;

    for (i = 0; i < old_size; i++) {
        current_entry = old_table[i];
        if (current_entry.flag == 1) {
            insert_status = insert_ht(
                    ht,
                    current_entry.key,
                    current_entry.value
            );
            /* NOTE: Shit fix this */
            if (insert_status != HT_SUCCESS) {
                fprintf(stderr, "Insert in rehash failed");
            }
        }
    }
    free(old_table);
    return HT_SUCCESS;
}

static int try_downsize(HashTab *ht) {
    float lamda, inactive_ratio;
    uint32_t prev_size;
    int32_t rehash_status;
    
    lamda = (float)ht->active / ht->size;
    inactive_ratio = (float)(ht->used - ht->active) / ht->size;
    /* TODO: decide what errcodes to return as default*/
    rehash_status = HT_SUCCESS;
    if (inactive_ratio > ht->inactive_factor || lamda < ht->min_load_factor) {
        if ((float)ht->active / ht->prev_size < ht->load_factor) {
            rehash_status = rehash(ht, prev_size);
            /* NOTE: No bounds check */
            ht->delta_idx = ht->delta_idx--;
            prev_size = (1 << (ht->delta_idx - 1)) + delta[ht->delta_idx - 1]; 
        } else if (prev_size <= ht->min_size) {
            rehash_status = rehash(ht, ht->size);
        }
    }
    return rehash_status;

}
/* --- prime calculation ---------------------------------------------------- */

static uint64_t power_mod(uint64_t base, uint64_t exponent, uint64_t mod) {
    uint64_t result = 1;
    base = base % mod;

    while (exponent > 0) {
        /* If exponent is odd, multiply base with result */
        if (exponent & 1) {
            result = ((__int128)result * base) % mod; 
        }
        exponent = exponent >> 1;
        base = ((__int128)base * base) % mod;
    
    }
    return result;
}

static uint8_t miller_rabin(uint32_t n) {
    uint64_t d, a, x;
    uint32_t s, i, r, num_bases, cont_outer;
    
    /* Handle base cases */
    if (n == 2 || n == 3) {
        return 1;
    }
    /* n <= 1 not needed but doesnt hurt */
    if (n <= 1 || n % 2 == 0) {
        return 0;
    }

    /* n - 1 = 2^s * d */
    d = n - 1;
    s = 0;
    /* / 2 until odd */
    while (d % 2 == 0) {
        d /= 2;
        s += 1;
    }

    num_bases = sizeof(miller_rabin_base) / sizeof(miller_rabin_base[0]);
    for (i = 0; i < num_bases; i++) {
        a = miller_rabin_base[i];
        if (a >= n) {
            continue;
        }
        x = power_mod(a, d, n);
        /* check for none trivial roots */
        if (x == 1 || x == n - 1) {
            /* possible prime for base a */
            continue;
        }
        /* flag to continue further checks */ 
        cont_outer = 0;
        /* detects composites(Carmicheal numbers?) */
        for (r = 1; r < s; r++) {
            x = power_mod(x, 2, n);
            if (x == n - 1) {
                cont_outer = 1;
                break;
            }
        }

        if (cont_outer) {
            continue;
        }

        /* flagged composite */
        return 0;
    }

    /* passed all primality checks */
    return 1;        
}

static uint32_t next_prime(uint32_t n) {
    uint8_t is_prime;
    uint32_t num;

    is_prime = 0;
    num = n;

    do {
        is_prime = miller_rabin(num);
        num++;
    } while (!is_prime);

    return num--;
}
    
static uint32_t delta_prime(uint32_t n) {
    return ((1 << n) + delta[n]);
}

/* --- default functions ---------------------------------------------------- */

/* Default hash function preforms a modified FNV-1a hash on the key bytes */
static unsigned int default_hash_function(KEY_TYPE key, unsigned int size) {

    const unsigned char *bytes_ptr = (const unsigned char *)&key;
    size_t num_bytes = sizeof(KEY_TYPE);

    /* Initialize FNV-1a hash constants */
    unsigned int hash = 2166136261u; // FNV offset basis
    unsigned int fnv_prime = 16777619u;

    /* Process each byte of the key */
    for (size_t i = 0; i < num_bytes; i++) {
        hash ^= bytes_ptr[i];       // XOR with the byte
        hash *= fnv_prime;         // Multiply by FNV prime
    }

    return hash % size;
}

/* Default key comparison function */
static int default_key_compare(KEY_TYPE a, KEY_TYPE b) {
    return a - b;
}

