#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "DS-MandatoryExercise/utils.h"


int str_to_num(const char *value_str, void *value, const char type) {
    /* function that casts a given string value_str to a number;
     * writes cast value to value pointer; this value pointer
     * should be a pointer to the desired number type.
     * Currently supported types: int, float */
    errno = 0;

    char * endptr;                  /* used for castings */
    char error[MAX_STR_SIZE];       /* used to display errors */
    void *cast_value;               /* this points to cast value */
    size_t result_size;             /* used to determine the size of the cast value in memcpy call */

    /* auxiliary variables to cast value */
    int value_str_to_int;
    float value_str_to_float;

    if (!strlen(value_str)) {
        fprintf(stderr, "String is empty\n"); return -1;
    }

    /* cast value */
    switch (type) {
        case INT:
            value_str_to_int = (int)strtol(value_str, &endptr, 10);
            cast_value = (void *) &value_str_to_int;
            strcpy(error, "strtol");
            result_size = sizeof(int);
            break;
        case FLOAT:
            value_str_to_float = strtof(value_str, &endptr);
            cast_value = (void *) &value_str_to_float;
            strcpy(error, "strtof");
            result_size = sizeof(float);
            break;
        default: fprintf(stderr, "Invalid casting type\n"); return -1;
    }

    /* check casting errors */
    if (errno) {
        perror(error); return -1;
    }
    if (endptr == value_str) {
        fprintf(stderr, "No digits were found\n"); return -1;
    }

    /* cast succeeded: set value */
    memcpy(value, cast_value, result_size);
    return 0;
}


/* file & socket stuff */

int send_msg(const int d, char *buffer, const int len) {
    /* sends a message of len bytes to d (socket, file... descriptor) */
    ssize_t bytes_sent;         /* number of bytes written by last write() */
    ssize_t bytes_left = len;   /* number of bytes left to be received */

    do {
        bytes_sent = write(d, buffer, bytes_left);
        bytes_left = bytes_left - bytes_sent;
        buffer = buffer + bytes_sent;
    } while ((bytes_left > 0) && (bytes_sent >= 0));

    if (bytes_sent < 0) return -1;  /* write() error */
    return 0;	/* full length has been sent */
}


int recv_msg(const int d, char *buffer, const int len) {
    /* receives a message of len bytes from d (socket, file... descriptor) */
    ssize_t bytes_received;     /* number of bytes fetched by last read() */
    ssize_t bytes_left = len;   /* number of bytes left to be received */

    do {
        bytes_received = read(d, buffer, bytes_left);
        bytes_left -= bytes_received;
        buffer += bytes_received;
    } while ((bytes_left > 0) && (bytes_received >= 0));

    if (bytes_received < 0) return -1;  /* read() error */
    return 0;	/* full length has been received */
}


ssize_t read_line(const int d, char *buffer, const int buf_space) {
    /* reads a 1-line string of at most (buf_space -1) chars, excluding terminating byte,
     * from d (socket, file... descriptor); stops reading when a '\0' or '\n' is found */
    ssize_t bytes_read;             /* number of bytes fetched by last read() */
    ssize_t bytes_read_total;       /* total bytes read so far */
    char ch;

    /* check arguments */
    if (buf_space <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    bytes_read_total = 0;
    /* read from fd */
    while (TRUE) {
        bytes_read = read(d, &ch, 1);	/* read a byte */

        /* check what's been read */
        if (bytes_read == -1) {
            if (errno == EINTR)	                    /* interrupted -> restart read() */
                continue;
            else
                return -1;		                    /* some other error */
        } else if (!bytes_read) {	                /* EOF */
            if (!bytes_read_total)	                /* no bytes read; return 0 */
                return 0;
            else
                break;
        } else {			                        /* bytes_read must be 1 if we get here */
            if (ch == '\n' || ch == '\0')           /* break line or string end found, so line ends */
                break;
            if (bytes_read_total < buf_space - 1) {		    /* discard > (n-1) bytes */
                bytes_read_total++;
                *buffer++ = ch;
            }
        }
    }
    *buffer = '\0';
    return bytes_read_total;
}
