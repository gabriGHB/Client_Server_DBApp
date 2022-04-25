#ifndef DBMS_UTILS_H
#define DBMS_UTILS_H

#include <stdio.h>
#include <dirent.h>


/* functions called internally in dbms module */
DIR *open_db(void);
int open_keyfile(int key, char mode);
int read_value_from_keyfile(int key_fd, char *value, int size);
int write_values_to_keyfile(int key_fd, const char *value1, const int *value2, const float *value3);

#endif //DBMS_UTILS_H
