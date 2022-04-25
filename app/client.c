#include <stdio.h>
#include "DS-MandatoryExercise/utils.h"
#include "DS-MandatoryExercise/keys.h"

/* some common messages to print */
const char display_actions_str[] = "The possible operations to perform are the following:\n"
                                   "1. Initialize DataBase\n"
                                   "2. Insert a new tuple\n"
                                   "3. Obtain an existing tuple\n"
                                   "4. Modify an existing tuple\n"
                                   "5. Delete a tuple\n"
                                   "6. Check if a tuple is already stored\n"
                                   "7. How many tuples are stored?\n"
                                   "8. Exit\n\0";
/* prompts */
const char ask_key_prompt[] = "Please, enter the key\0";
const char ask_key_delete_prompt[] = "Please, enter the key of the tuple to delete\0";
const char ask_key_exist_prompt[] = "Please, enter the key of the tuple to search for\0";
const char ask_value1_prompt[] = "Please, enter value1\0";
const char ask_value2_prompt[] = "Please, enter value2\0";
const char ask_value3_prompt[] = "Please, enter value3\0";
/* input errors */
const char action_error[] = "Please, select one of the previously defined actions: \0";
const char int_required_error[] = "Please, enter an integer\0";
const char float_required_error[] = "Please, enter a real number\0";

const unsigned int ACTION_STR_LEN = 2;            /* size of client interface action string */


int get_key(int *key, const char *prompt_str);
int get_key_and_values(int *key, char *value1, int *value2, float *value3);


int get_key(int *key, const char *prompt_str) {
    char key_str[MAX_STR_SIZE];

    /* ask for the key */
    printf("%s: ", prompt_str); scanf("%s", key_str);
    if (str_to_num(key_str, (void *) key, INT) == -1) {
        fprintf(stderr, "%s\n", int_required_error); return -1;
    }

    return 0;
}


int get_key_and_values(int *key, char *value1, int *value2, float *value3) {
    char value2_str[MAX_STR_SIZE]; char value3_str[MAX_STR_SIZE];

    /* ask for the key */
    if (get_key(key, ask_key_prompt) == -1) return -1;

    /* ask for the values */
    printf("%s: ", ask_value1_prompt); scanf("%s", value1);
    printf("%s: ", ask_value2_prompt); scanf("%s", value2_str);
    if (str_to_num(value2_str, (void *) value2, INT) == -1) {
        fprintf(stderr, "%s\n", int_required_error); return -1;
    }
    printf("%s: ", ask_value3_prompt); scanf("%s", value3_str);
    if (str_to_num(value3_str, (void *) value3, FLOAT) == -1) {
        fprintf(stderr, "%s\n", float_required_error); return -1;
    }

    return 0;
}


int main() {
    int control_var = TRUE;

    /* loop to control client requests */
    while (control_var) {
        /* display available actions */
        int action; char action_str[ACTION_STR_LEN];
        printf("\n%s\n", display_actions_str);

        /* ask for an action */
        printf("Please, insert the number of the operation to perform: "); scanf("%s", action_str);
        while ((str_to_num(action_str, (void *) &action, INT) == -1) || (action < 1) || (action > 8)) {
            fprintf(stderr, "%s", action_error); scanf("%s", action_str);
        } // end inner while

        /* continue with chosen action */
        switch (action) {
            case 1: {       /* initializing server db */
                if (!init()) fprintf(stderr, "\nThe Database has been initialized\n");
                else perror("\nThe Database has not been initialized");
                break;
            } // end case 1
            case 2: {       /* insert a new tuple */
                int key; char value1[VALUE1_MAX_STR_SIZE]; int value2; float value3;

                /* ask for the values and key */
                if (get_key_and_values(&key, value1, &value2, &value3) == -1) continue;

                /* call the set_value function to perform the task */
                int error = set_value(key, value1, value2, value3);
                if (!error) fprintf(stderr, "\nThe tuple was successfully inserted\n");
                else fprintf(stderr, "\nError while inserting the tuple\n");
                break;
            } // end case 2
            case 3: {       /* obtain an existing tuple */
                int key; char value1[VALUE1_MAX_STR_SIZE]; int value2; float value3;

                /* ask for the key */
                if (get_key(&key, ask_key_prompt) == -1) continue;

                /* calling get_value service and checking errors */
                int error = get_value(key, value1, &value2, &value3);
                if (!error) fprintf(stderr, "\nThe tuple with key %d stores value 1 = %s, "
                                            "value 2 = %d and value 3 = %f\n", key, value1, value2, value3);
                else fprintf(stderr, "\nAn error happened when searching the tuple.\n");
                break;
            } // end case 3
            case 4: {       /* modify an existing tuple */
                int key; char value1[VALUE1_MAX_STR_SIZE]; int value2; float value3;

                /* ask for the values and key */
                if (get_key_and_values(&key, value1, &value2, &value3) == -1) continue;

                /* calling modify_value service and checking errors */
                int error = modify_value(key, value1, value2, value3);
                if (!error) fprintf(stderr, "\nThe tuple with key %d was modified to value 1 = %s, "
                                   "value 2 = %d and value 3 = %f\n", key, value1, value2, value3);
                else fprintf(stderr, "\nError modifying the tuple\n");
                break;
            } //end case 4
            case 5: {       /* delete a tuple */
                int key;

                /* ask for the key */
                if (get_key(&key, ask_key_delete_prompt) == -1) continue;

                /* calling delete_key service and checking errors */
                int error = delete_key(key);
                if (!error) fprintf(stderr, "\nThe tuple with key %d was deleted.\n", key);
                else fprintf(stderr, "\nError deleting the tuple\n");
                break;
            } // end case 5
            case 6: {       /* check if a tuple exists */
                int key;

                /* ask for the key */
                if (get_key(&key, ask_key_exist_prompt) == -1) continue;

                /* calling exist service and checking errors */
                int error = exist(key);
                if (error == 1) fprintf(stderr, "\nA tuple with the key %d is already stored.\n", key);
                else if (!error) fprintf(stderr, "\nThere are no tuples with the key %d stored.\n", key);
                else fprintf(stderr, "\nCommunication error.\n");
                break;
            } // end case 6
            case 7: {       /* find out how many tuples are stored */
                /* calling num_items service and checking errors */
                int num_tuples = num_items();
                if (num_tuples >= 0) fprintf(stderr, "\nThere are %d tuples stored.\n", num_tuples);
                else fprintf(stderr, "\nError counting the number of elements stored.\n");
                break;
            } // end case 7
            case 8: {       /* exit by changing the control var to 0 */
                control_var = FALSE; break;
            } // end case 8
            default: break;
        } // end switch
    } // end outer while
    return 0;
}
