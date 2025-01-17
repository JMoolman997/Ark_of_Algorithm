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

#define DEFAULT_LOAD_FACTOR 0.5
#define DEFAULT_MIN_LOAD_FACTOR 0.25
#define DEFAULT_INACTIVE_FACTOR 0.1
#define DEFAULT_SIZE_MAX 1048576/* TODO*/
#define DEFAULT_SIZE_MIN 13

/* --- error return codes --------------------------------------------------- */

#define HT_FAILURE 1
#define HT_SUCCESS 0
#define HT_KEY_EXISTS -1
#define HT_NO_SPACE   -2
#define HT_KEY_NOT_FOUND -3
#define HT_MEM_ERROR -4
#define HT_INVALID_ARG -5
#define HT_INVALID_STATE -6

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
		size_t max_size,
		size_t min_size,
		float load_factor,
		float min_load_factor,
		float inactive_factor,
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

size_t size_ht(
		HashTab *self
);
#endif /* OPEN_ADDRESSING_H */
