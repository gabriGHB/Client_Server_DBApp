/* gtest.h declares the testing framework */
#include "gtest/gtest.h"
#include <cstdlib>

extern "C" {
#include "DS-MandatoryExercise/utils.h"
#include "DS-MandatoryExercise/keys.h"
}

/* test error codes */
const int SUCCESS = 0;
const int ERROR = -1;
const int EXISTS = 1;
const int NOT_EXISTS = 0;


TEST(keys_tests, test_init) {
    /* testing init service */
    ASSERT_EQ(init(), SUCCESS);     /* success: DB is initialized */
}


TEST(keys_tests, test_set_value) {
    /* testing set_value service: adding a new key file;
     * trying to add one that already exists */

    /* initial setup */
    init();

    /* success: tuple is created correctly */
    int key_1 = 11;
    char value1_1[] = "hello\0";
    int value2_1 = 11;
    float value3_1 = 11.1f;

    ASSERT_EQ(exist(key_1), NOT_EXISTS);    /* sanity check: tuple doesn't exist */
    ASSERT_EQ(set_value(key_1, value1_1, value2_1, value3_1), SUCCESS);
    ASSERT_EQ(exist(key_1), EXISTS);        /* sanity check: tuple exists */

    /* failure: creating a tuple with an existing key */
    char value1_2[] = "bye\0";
    int value2_2 = 22;
    float value3_2 = 22.2f;

    ASSERT_EQ(exist(key_1), EXISTS);    /* sanity check: tuple exists */
    ASSERT_EQ(set_value(key_1, value1_2, value2_2, value3_2), ERROR);
}


TEST(keys_tests, test_set_value_value1_too_long) {
    /* testing set_value service: checking that value1 never exceeds its max length */

    /* initial setup: trying to insert a tuple with a value1 longer that it's supposed to be */
    init();
    int key_1 = 11;
    /* 260 chars, longer than the 255 value1 allows */
    char value1_1[] = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
                      "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
                      "12345678901234567890123456789012345678901234567890123456789012345678901234567890\0";
    int value2_1 = 11;
    float value3_1 = 11.1f;
    set_value(key_1, value1_1, value2_1, value3_1);

    /* success: retrieve tuple & check value1: it should be shorter that the one we tried to write */
    char value1_2[VALUE1_MAX_STR_SIZE]; int value2_2; float value3_2;

    ASSERT_EQ(exist(key_1), EXISTS);        /* sanity check: tuple exists */
    get_value(key_1, value1_2, &value2_2, &value3_2);
    ASSERT_EQ(strlen(value1_1) > strlen(value1_2), true);
}


TEST(keys_tests, test_get_value) {
    /* testing get_value service: getting a tuple that exists;
     * trying to get a non-existent tuple */

    /* initial setup: inserting a tuple to retrieve it later */
    init();
    int key_1 = 11;
    char value1_1[] = "hello\0";
    int value2_1 = 11;
    float value3_1 = 11.1f;
    set_value(key_1, value1_1, value2_1, value3_1);

    /* success: tuple exists and is retrieved */
    char value1_2[VALUE1_MAX_STR_SIZE]; int value2_2; float value3_2;

    ASSERT_EQ(exist(key_1), EXISTS);        /* sanity check: tuple exists */
    ASSERT_EQ(get_value(key_1, value1_2, &value2_2, &value3_2), SUCCESS);

    /* failure: getting values of a tuple that doesn't exist */
    int key_2 = 22;

    ASSERT_EQ(exist(key_2), NOT_EXISTS);    /* sanity check: tuple doesn't exist */
    ASSERT_EQ(get_value(key_2, value1_2, &value2_2, &value3_2), ERROR);
}


TEST(keys_tests, test_modify_value) {
    /* testing modify_value service: modifying a tuple that exists;
    * trying to modify a non-existent tuple */

    /* initial setup: inserting a tuple to modify it later */
    init();
    int key_1 = 11;
    char value1_1[] = "hello\0";
    int value2_1 = 11;
    float value3_1 = 11.1f;
    set_value(key_1, value1_1, value2_1, value3_1);

    /* success: modify the tuple, retrieve it and check that the retrieved
     * values are the same as the ones given for the modify_value call */
    char value1_2[] = "aloha\0";
    int value2_2 = 22;
    float value3_2 = 22.2f;

    ASSERT_EQ(exist(key_1), EXISTS);    /* sanity check: tuple exists */
    ASSERT_EQ(modify_value(key_1, value1_2, value2_2, value3_2), SUCCESS);
    ASSERT_EQ(exist(key_1), EXISTS);    /* sanity check: tuple still exists */

    /* retrieve & check modified values */
    get_value(key_1, value1_1, &value2_1, &value3_1);
    ASSERT_EQ(!strcmp(value1_1, value1_2) && value2_1 == value2_2 && value3_1 == value3_2, true);

    /* failure: modify values of a tuple that doesn't exist */
    int key_2 = 22;

    ASSERT_EQ(exist(key_2), NOT_EXISTS);    /* sanity check: tuple doesn't exist */
    ASSERT_EQ(modify_value(key_2, value1_2, value2_2, value3_2), ERROR);
}


TEST(keys_tests, test_modify_value_value1_too_long) {
    /* testing modify_value service: checking that value1 never exceeds its max length */

    /* initial setup: inserting a tuple to modify it later */
    init();
    int key_1 = 11;
    char value1_1[] = "hello\0";
    int value2_1 = 11;
    float value3_1 = 11.1f;
    set_value(key_1, value1_1, value2_1, value3_1);

    /* trying to modify a tuple with a value1 longer that it's supposed to be:
     * 260 chars, longer than the 255 value1 allows */
    char value1_2[] = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
                      "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
                      "12345678901234567890123456789012345678901234567890123456789012345678901234567890\0";

    ASSERT_EQ(exist(key_1), EXISTS);        /* sanity check: tuple exists */
    modify_value(key_1, value1_2, value2_1, value3_1);
    ASSERT_EQ(exist(key_1), EXISTS);        /* sanity check: tuple still exists */

    /* success: retrieve tuple & check value1: it should be shorter that the one we tried to write */
    char value1_3[VALUE1_MAX_STR_SIZE];

    get_value(key_1, value1_3, &value2_1, &value3_1);
    ASSERT_EQ(strlen(value1_2) > strlen(value1_3), true);
}


TEST(keys_tests, test_delete_key) {
    /* testing delete_key service: deleting a tuple that exists;
    * trying to delete a non-existent tuple */

    /* initial setup: inserting a tuple to delete it later */
    init();
    int key_1 = 11;
    char value1_1[] = "hello\0";
    int value2_1 = 11;
    float value3_1 = 11.1f;
    set_value(key_1, value1_1, value2_1, value3_1);

    /* success: delete the tuple and check that it doesn't exist */
    ASSERT_EQ(exist(key_1), EXISTS);        /* sanity check: tuple exists */
    ASSERT_EQ(delete_key(key_1), SUCCESS);
    ASSERT_EQ(exist(key_1), NOT_EXISTS);    /* sanity check: tuple doesn't exist anymore */

    /* failure: deleting a file with non-existent key */
    int key_2 = 22;
    ASSERT_EQ(exist(key_2), NOT_EXISTS);    /* sanity check: tuple doesn't exist */
    ASSERT_EQ(delete_key(key_2), ERROR);
}


TEST(keys_tests, test_exists) {
    /* testing exist service: checking whether an existing tuple actually exists;
    * checking whether a non-existent tuple actually exists */

    /* initial setup: inserting a tuple to check its existence later */
    init();
    int key_1 = 11;
    char value1_1[] = "hello\0";
    int value2_1 = 11;
    float value3_1 = 11.1f;
    set_value(key_1, value1_1, value2_1, value3_1);

    /* the key exists */
    ASSERT_EQ(exist(key_1), EXISTS);

    /* the key does not exist */
    int key_2 = 22;
    ASSERT_EQ(exist(key_2), NOT_EXISTS);
}


TEST(keys_tests, test_num_items) {
    /* testing num_items service: checking how many tuples exist */

    /* initial setup */
    init();

    /* success: no tuples exist yet */
    ASSERT_EQ(num_items(), 0);

    /* add some tuples */
    int key_1 = 11; int key_2 = 22; int key_3 = 33;
    char value1_1[] = "hello1\0"; char value1_2[] = "hello2\0"; char value1_3[] = "hello3\0";
    int value2_1 = 11; int value2_2 = 22; int value2_3 = 33;
    float value3_1 = 11.1f; float value3_2 = 22.2f; float value3_3 = 33.3f;

    set_value(key_1, value1_1, value2_1, value3_1);
    set_value(key_2, value1_2, value2_2, value3_2);
    set_value(key_3, value1_3, value2_3, value3_3);

    /* success */
    ASSERT_EQ(exist(key_1), EXISTS);        /* sanity check: tuple exists */
    ASSERT_EQ(exist(key_2), EXISTS);        /* sanity check: tuple exists */
    ASSERT_EQ(exist(key_3), EXISTS);        /* sanity check: tuple exists */
    ASSERT_EQ(num_items(), 3);
}
