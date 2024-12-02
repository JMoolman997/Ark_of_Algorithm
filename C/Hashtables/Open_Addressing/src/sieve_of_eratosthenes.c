#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_POWER 29
#define SIEVE_LIMIT (1U << (MAX_POWER + 1))


typedef struct {
    /** bit arry representing primality                                */
    uint8_t *bits;
    /** maximum number represented in the sieve                        */
    uint32_t size;
} BitSieve;

/* --- function prototypes ------------------------------------------------- */
static BitSieve *init_bit_sieve(uint32_t limit);
static void free_bit_sieve(BitSieve *sieve);

static uint8_t is_prime_bit(BitSieve *sieve, uint32_t index);
static void mark_non_prime(BitSieve *sieve, uint32_t index);
static void bitwise_sieve_of_eratosthenes(BitSieve *sieve);
static uint32_t power_of_two(uint32_t power);
static uint32_t next_prime(BitSieve *sieve, uint32_t target);
static uint32_t prev_prime(BitSieve *sieve, uint32_t target);

static BitSieve *init_bit_sieve(uint32_t limit) {

    BitSieve *sieve;
    uint32_t byte_len;

    sieve = (BitSieve*)malloc(sizeof(BitSieve));
    if (sieve == NULL) {
        return NULL;
    }
    sieve->size = limit;

    byte_len = (limit / 8) + 1;
    sieve->bits = (uint8_t*)malloc(byte_len * sizeof(uint8_t));
    if (sieve->bits == NULL) {
        return NULL;
    }

    /* initialize all bits to 1 */
    memset(sieve->bits, 0xFF, byte_len);

    /* 0 and 1 not prime */
    sieve->bits[0] &= ~(1 << 0);
    sieve->bits[0] &= ~(1 << 1);

    return sieve;

}

static void free_bit_sieve(BitSieve *sieve) {
    if (sieve) {
        free(sieve->bits);
        free(sieve);
    }
}

static uint8_t is_prime_bit(BitSieve *sieve, uint32_t index) {
    
    uint32_t byte_index, bit_pos;

    if (index > sieve->size) {
        return 0;
    }
    byte_index = index / 8;
    bit_pos = index % 8;

    return (sieve->bits[byte_index] & (1 << bit_pos)) ? 1 : 0;

}

static void mark_non_prime(BitSieve *sieve, uint32_t index) {

    uint32_t byte_index, bit_pos;
    if (index > sieve->size) {
        return;
    }
    byte_index = index / 8;
    bit_pos = index % 8;

    sieve->bits[byte_index] &= ~(1 << bit_pos);

}

static void bitwise_sieve_of_eratosthenes(BitSieve *sieve) {

    uint32_t limit, p, multiple;

    limit = sieve->size;
    for (p = 2; p * p <= limit; p++) {
        if (is_prime_bit(sieve, p)) {
            /* Mark a multiples of p as non-prime from p*p */
            for (multiple = p * p; multiple <= limit; multiple += p) {
                mark_non_prime(sieve, multiple);
            }
        }
    }
}

static uint32_t power_of_two(uint32_t power) {
    if (power >= sizeof(uint32_t) * 8) {  
        fprintf(stderr, "Power too large for uint32_t.\n");
        exit(EXIT_FAILURE);
    }
    return 1U << power;
}

static uint32_t next_prime(BitSieve *sieve, uint32_t target) {

    uint32_t index;

    if (target > sieve->size) {
        return 0;
    }

    for (index = target; index < sieve->size; index++) {
        if (is_prime_bit(sieve, index)) {
            return index;
        }
    }

    fprintf(stderr, "No prime found >= %u within the sieve limit.\n", target);
    exit(EXIT_FAILURE);
}

static uint32_t prev_prime(BitSieve *sieve, uint32_t target) {
    uint32_t index;
    if (target < 0) {
        return 0;
    }

    for (index = target; index > 0; index--) {
        if (is_prime_bit(sieve, index)) {
            return index;
        }
    }

    return 0;
}

void populate_primes_near_powers(BitSieve* sieve, uint32_t* prime_near_powers) {
    uint32_t power, target;

    for (power = 0; power < MAX_POWER; power++) {
        target = power_of_two(power);
        prime_near_powers[power] = next_prime(sieve, target);
    }
}

int main() {
    uint32_t *prime_near_powers, power, current_power, prime;
    clock_t start_time, end_time;
    double elapsed_time;

    printf("Initializing bitwise sieve up to %u...\n", SIEVE_LIMIT);
    BitSieve* sieve = init_bit_sieve(SIEVE_LIMIT);
    printf("Bitwise sieve initialized.\n");

    start_time = clock();
    printf("Performing sieve of Eratosthenes...\n");
    bitwise_sieve_of_eratosthenes(sieve);
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Sieve completed in %.4f seconds.\n", elapsed_time);

    prime_near_powers = (uint32_t*)malloc(MAX_POWER * sizeof(uint32_t));
    if (prime_near_powers == NULL) {
        fprintf(stderr, "Memory allocation failed for prime_near_powers.\n");
        free_bit_sieve(sieve);
        exit(EXIT_FAILURE);
    }

    printf("Populating primes near powers of two...\n");
    populate_primes_near_powers(sieve, prime_near_powers);
    printf("Primes populated.\n");

    printf("Primes near powers of two:\n");
    for (power = 0; power < MAX_POWER; power++) {
        current_power = power_of_two(power);
        prime = prime_near_powers[power];
        printf(
                "2^%u = %u, Next Prime: %u, dif = %u\n",
                power,
                current_power,
                prime,
                prime - current_power
        );
    }

    free_bit_sieve(sieve);
    free(prime_near_powers);

    return 0;
}
