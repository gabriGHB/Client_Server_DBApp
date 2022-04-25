#ifndef DBMS_H
#define DBMS_H

/* functions called by the server to manage the DB */
int db_list_items(void);
int db_get_num_items(void);
int db_empty_db(void);
int db_item_exists(int key);
int db_read_item(int key, char *value1, int *value2, float *value3);
int db_write_item(int key, const char *value1, const int *value2, const float *value3, char mode);
int db_delete_item(int key);

#endif //DBMS_H
