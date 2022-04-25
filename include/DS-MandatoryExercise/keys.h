#ifndef KEYS_H
#define KEYS_H

/* client API:
 * functions called by the client to perform services;
 * they are all wrappers for 'service' function */
int init();
int set_value(int key, char *value1, int value2, float value3);
int get_value(int key, char *value1, int *value2, float *value3);
int modify_value(int key, char *value1, int value2, float value3);
int delete_key(int key);
int exist(int key);
int num_items();

#endif //KEYS_H
