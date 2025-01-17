/**
 * @file    test_open_addressing.c
 * @brief   Test program for generic open addressing hash table implementation.
 * @author  J.W Moolman
 * @date    2024-11-25
 */

#include "unity.h"
#include "open_addressing.h"

HashTab *ht;
ProbingMethod probing_method;

/**
 * @brief Setup and teardown functions for Unity test framework.
 */
void setUp(
        void
) {
    ht = init_ht(0, 0, 0, 0, 0, NULL, NULL, probing_method);
    TEST_ASSERT_NOT_NULL(ht);
}

void tearDown(
        void
) {
    free_ht(ht, NULL, NULL);
}

/**
 * @defgroup BasicTests
 * @brief Basic functionality tests for open addressing hash table.
 * @{ 
 */
void test_insert_should_succeed(
        void
) {
    int result = insert_ht(ht, 1, 100);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);
}

void test_insert_duplicate_should_fail(
        void
) {
    int result1 = insert_ht(ht, 2, 200);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result1);

    int result2 = insert_ht(ht, 2, 300);
    TEST_ASSERT_EQUAL_INT(HT_KEY_EXISTS, result2);
}

void test_search_existing_key(
        void
) {
    insert_ht(ht, 3, 300);

    int index = search_ht(ht, 3);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index);

    int value = fetch_ht(ht, index);
    TEST_ASSERT_EQUAL_INT(300, value);
}

void test_search_nonexistent_key(
        void
) {
    int index = search_ht(ht, 4);
    TEST_ASSERT_EQUAL_INT(HT_KEY_NOT_FOUND, index);
}

void test_remove_existing_key(
        void
) {
    insert_ht(ht, 5, 500);
    int result = remove_ht(ht, 5);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);

    int index = search_ht(ht, 5);
    TEST_ASSERT_EQUAL_INT(HT_KEY_NOT_FOUND, index);
}

void test_remove_nonexistent_key(
        void
) {
    int result = remove_ht(ht, 6);
    TEST_ASSERT_EQUAL_INT(HT_KEY_NOT_FOUND, result);
}
/** @} */

/**
 * @defgroup EdgeCaseTests
 * @brief Tests for edge cases and boundary conditions.
 * @{ 
 */
void test_null_input(
        void
) {
    int result = insert_ht(NULL, 1, 100);
    TEST_ASSERT_EQUAL_INT(HT_INVALID_ARG, result);

    int index = search_ht(NULL, 1);
    TEST_ASSERT_EQUAL_INT(HT_INVALID_ARG, index);

    result = remove_ht(NULL, 1);
    TEST_ASSERT_EQUAL_INT(HT_INVALID_ARG, result);
}

void test_boundary_keys(void) {
    int result_min = insert_ht(ht, INT_MIN, -1);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result_min);

    int result_max = insert_ht(ht, INT_MAX, 1);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result_max);

    int index_min = search_ht(ht, INT_MIN);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index_min);
    int value_min = fetch_ht(ht, index_min);
    TEST_ASSERT_EQUAL_INT(-1, value_min);

    int index_max = search_ht(ht, INT_MAX);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index_max);
    int value_max = fetch_ht(ht, index_max);
    TEST_ASSERT_EQUAL_INT(1, value_max);
}

void test_high_collision_scenario(void) {
    size_t table_size = size_ht(ht);

    for (size_t i = 0; i < table_size; i++) {
        int key = i * table_size; // Keys designed to collide
        int result = insert_ht(ht, key, i);
        TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);
    }

    for (size_t i = 0; i < table_size; i++) {
        int key = i * table_size;
        int index = search_ht(ht, key);
        TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index);
        int value = fetch_ht(ht, index);
        TEST_ASSERT_EQUAL_INT(i, value);
    }
}

void test_zero_key_insertion(void) {
    int result = insert_ht(ht, 0, 999);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);

    int index = search_ht(ht, 0);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index);
    int value = fetch_ht(ht, index);
    TEST_ASSERT_EQUAL_INT(999, value);
}
/** @} */

/**
 * @defgroup AdvancedTests
 * @brief Advanced scenarios including rehashing and stress testing.
 * @{ 
 */
void test_rehashing(
        void
) {
    size_t initial_size = size_ht(ht);
    unsigned int max_entries = (unsigned int)(initial_size * DEFAULT_LOAD_FACTOR);

    for (unsigned int i = 0; i < max_entries + 1; i++) { 
        int result = insert_ht(ht, i, i * 10);
        TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);
    }

    // Verify that all inserted keys are searchable
    for (unsigned int i = 0; i < max_entries + 1; i++) {
        int index = search_ht(ht, i);
        TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index);
        int value = fetch_ht(ht, index);
        TEST_ASSERT_EQUAL_INT(i * 10, value);
    }
}

void test_table_resize_downward(void) {
    // Fill the table
    for (int i = 0; i < 10; i++) {
        int result = insert_ht(ht, i, i * 10);
        TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);
    }

    // Remove elements to trigger potential shrinking
    for (int i = 0; i < 8; i++) {
        int result = remove_ht(ht, i);
        TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);
    }

    // Validate the remaining entries
    for (int i = 8; i < 10; i++) {
        int index = search_ht(ht, i);
        TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index);
        int value = fetch_ht(ht, index);
        TEST_ASSERT_EQUAL_INT(i * 10, value);
    }
}

void test_large_insertions(void) {
    size_t large_size = 1000000; // 1 million entries
    for (size_t i = 0; i < large_size; i++) {
        int result = insert_ht(ht, i, i * 10);
        TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);
    }

    for (size_t i = 0; i < large_size; i++) {
        int index = search_ht(ht, i);
        TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index);
        int value = fetch_ht(ht, index);
        TEST_ASSERT_EQUAL_INT(i * 10, value);
    }
}

void test_insert_into_full_table_should_fail(
        void
) {
    /* Initialize hash table with a small fixed size 
     * and load factor of 1.0 to prevent resizing
     */ 
    unsigned int i, size;
    int result;

    size = size_ht(ht);
    ht = init_ht(2, 0, 1.0, 0, 0, NULL, NULL, probing_method);
    TEST_ASSERT_NOT_NULL(ht);

    // Fill the table to capacity
    for (i = 0; i < size; i++) {
        result = insert_ht(ht, i, i * 10);
        TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);
    }

    // Attempt to insert into a full table
    result = insert_ht(ht, size, size * 10);
    TEST_ASSERT_EQUAL_INT(HT_NO_SPACE, result);

}
/** @} */

/**
 * @brief Tests the hash table implementation using a specified probing method.
 * @param method The probing method to use (e.g., LINEAR, QUADRATIC).
 */
void test_probing_method(
        ProbingMethod method
) {

    probing_method = method;
    /* BasicTests */
    RUN_TEST(test_insert_should_succeed);
    RUN_TEST(test_insert_duplicate_should_fail);
    RUN_TEST(test_search_existing_key);
    RUN_TEST(test_search_nonexistent_key);
    RUN_TEST(test_remove_existing_key);
    RUN_TEST(test_remove_nonexistent_key);
    /* EdgeCaseTests */
    RUN_TEST(test_null_input);
    RUN_TEST(test_boundary_keys);
    RUN_TEST(test_high_collision_scenario);
    RUN_TEST(test_zero_key_insertion);
    /* AdvancedTests */
    RUN_TEST(test_rehashing);
    RUN_TEST(test_table_resize_downward);
    RUN_TEST(test_large_insertions);
    /**RUN_TEST(test_insert_into_full_table_should_fail);**/
}

/**
 * @brief Test runner for open addressing hash table tests.
 */
int main(
        void
) {

    UNITY_BEGIN();
    printf("\n --- Linear probing --- \n");
    test_probing_method(LINEAR);
    printf("\n --- Quadratic probing --- \n");
    test_probing_method(QUADRATIC);

    return UNITY_END();
}
