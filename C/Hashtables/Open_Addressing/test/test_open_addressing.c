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

void setUp(void) {
    ht = init_ht(0, 0, 0, 0, 0, NULL, NULL, probing_method);
    TEST_ASSERT_NOT_NULL(ht);
}

void tearDown(void) {
    free_ht(ht, NULL, NULL);
}

void test_insert_should_succeed(void) {
    int result = insert_ht(ht, 1, 100);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);
}

void test_insert_duplicate_should_fail(void) {
    int result1 = insert_ht(ht, 2, 200);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result1);

    int result2 = insert_ht(ht, 2, 300);
    TEST_ASSERT_EQUAL_INT(HT_KEY_EXISTS, result2);
}

void test_search_existing_key(void) {
    insert_ht(ht, 3, 300);

    int index = search_ht(ht, 3);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, index);

    int value = fetch_ht(ht, index);
    TEST_ASSERT_EQUAL_INT(300, value);
}

void test_search_nonexistent_key(void) {
    int index = search_ht(ht, 4);
    TEST_ASSERT_EQUAL_INT(HT_KEY_NOT_FOUND, index);
}

void test_remove_existing_key(void) {
    insert_ht(ht, 5, 500);
    int result = remove_ht(ht, 5);
    TEST_ASSERT_EQUAL_INT(HT_SUCCESS, result);

    int index = search_ht(ht, 5);
    TEST_ASSERT_EQUAL_INT(HT_KEY_NOT_FOUND, index);
}

void test_remove_nonexistent_key(void) {
    int result = remove_ht(ht, 6);
    TEST_ASSERT_EQUAL_INT(HT_KEY_NOT_FOUND, result);
}

void test_rehashing(void) {
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

void test_probing_method(ProbingMethod method) {

    probing_method = method;
    RUN_TEST(test_insert_should_succeed);
    RUN_TEST(test_insert_duplicate_should_fail);
    RUN_TEST(test_search_existing_key);
    RUN_TEST(test_search_nonexistent_key);
    RUN_TEST(test_remove_existing_key);
    RUN_TEST(test_remove_nonexistent_key);
    RUN_TEST(test_rehashing);
}

// Test Runner
int main(void) {

    UNITY_BEGIN();
    printf("\n --- Linear probing --- \n");
    test_probing_method(LINEAR);
    printf("\n --- Quadratic probing --- \n");
    test_probing_method(QUADRATIC);

    return UNITY_END();
}
