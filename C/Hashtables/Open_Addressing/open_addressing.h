/**
 * @file    open_addressing.h
 * @brief   A modular open addressing hash table implementation for
 *          testing and benchmarking.
 * @author  J.W Moolman
 * @date    2024-10-23
 */

#ifndef OPEN_ADDRESSING_H
#define OPEN_ADDRESSING_H

/* --- type macros ---------------------------------------------------------- */
/* NOTE: does not currently support pointers, using pointers will cause UB */
#ifndef KEY_TYPE
#define KEY_TYPE int
#endif

#ifndef VALUE_TYPE
#define VALUE_TYPE int
#endif

/* --- error return codes --------------------------------------------------- */

#define HASH_TABLE_KEY_VALUE_PAIR_EXISTS -1
#define HASH_TABLE_NO_SPACE_FOR_ENTRY    -2
#define REHASH_TABLE_RESIZE_FAILURE      -3
#define HASH_TABLE_KEY_NOT_FOUND         -4

/** the container structure for a hash table */
typedef struct hashtab HashTab;

/** an entry in the hash table */
typedef struct htentry HTentry;

/** Enumerated probing methods */
typedef enum {
    LINEAR,
    QUADRATIC,
    DOUBLE_HASHING
} ProbingMethod;

/* Function prototypes */
HashTab *init_ht(
		unsigned int max_size,
		float load_factor,
        unsigned int (*hash)(KEY_TYPE key, unsigned int size),
		int (*match)(KEY_TYPE key1, KEY_TYPE key2),
		ProbingMethod probing_method
);

int free_ht(
		HashTab *self,
		void (*freekey)(KEY_TYPE k),
		void (*freeval)(VALUE_TYPE v)
);

int search_ht(
		HashTab *self,
		KEY_TYPE key
);

VALUE_TYPE fetch_ht(
		HashTab *self,
		unsigned int index
);

int insert_ht(
		HashTab *self,
		KEY_TYPE key,
		VALUE_TYPE value
);

int remove_ht(
		HashTab *self,
		KEY_TYPE key
);

void print_ht(
		HashTab *self,
		void (*keyval2str)(int flag, KEY_TYPE k, VALUE_TYPE v, char *b)
);

#endif /* OPEN_ADDRESSING_H */
