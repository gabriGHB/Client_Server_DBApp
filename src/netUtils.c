#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "DS-MandatoryExercise/utils.h"
#include "DS-MandatoryExercise/netUtils.h"


int send_common_header(const int socket, header_t *header) {
    /* function that sends transaction ID, op_code members to socket */
    header->id = htonl(header->id);
    if (send_msg(socket, (char *) &header->id, sizeof(uint32_t)) == -1) {
        perror("Send transaction ID error");
        close(socket); return -1;
    }

    if (send_msg(socket, &header->op_code, 1) == -1) {
        perror("Send op_code error");
        close(socket); return -1;
    }

    return 0;
}


int send_reply_header(const int socket, reply_t *reply) {
    /* function that sends transaction ID, op_code & server_error_code members to socket */
    if (send_common_header(socket, &reply->header) == -1) return -1;

    reply->server_error_code = (int32_t) htonl(reply->server_error_code);
    if (send_msg(socket, (char *) &reply->server_error_code, sizeof(int32_t)) == -1) {
        perror("Send server_error_code error");
        close(socket); return -1;
    }

    return 0;
}


int send_num_items(const int socket, reply_t *reply) {
    /* function that sends num_items member to socket */
    reply->num_items = htonl(reply->num_items);
    if (send_msg(socket, (char *) &reply->num_items, sizeof(uint32_t)) == -1) {
        perror("Send num_items error");
        close(socket); return -1;
    }

    return 0;
}


int send_key(const int socket, item_t *item) {
    /* function that sends the key member to socket */
    item->key = (int32_t) htonl(item->key);
    if (send_msg(socket, (char *) &item->key, sizeof(int32_t)) == -1) {
        perror("Send key error");
        close(socket); return -1;
    }

    return 0;
}


int send_values(const int socket, item_t *item) {
    /* function that sends value members to socket */

    /* send value1 */
    if (send_msg(socket, item->value1, (int) (strlen(item->value1) + 1)) == -1) {
        perror("Send value1 error");
        close(socket); return -1;
    }

    /* send value2 */
    item->value2 = (int32_t) htonl(item->value2);
    if (send_msg(socket, (char *) &item->value2, sizeof(int32_t)) == -1) {
        perror("Send value2 error");
        close(socket); return -1;
    }

    /* send value3 */
    uint32_t tmp;
    memcpy((char *) &tmp, (char *) &item->value3, sizeof(float));
    tmp = htonl(tmp);
    if (send_msg(socket, (char *) &tmp, sizeof(float)) == -1) {
        perror("Send value3 error");
        close(socket); return -1;
    }

    return 0;
}


int recv_common_header(const int socket, header_t *header) {
    /* function that receives transaction ID & op_code members from socket */

    /* receive transaction ID */
    if (recv_msg(socket, (char *) &header->id, sizeof(uint32_t)) == -1) {
        perror("Receive transaction ID error");
        close(socket); return -1;
    }
    header->id = ntohl(header->id);

    /* receive op_code */
    if (recv_msg(socket, &header->op_code, 1) == -1) {
        perror("Receive op_code error");
        close(socket); return -1;
    }

    return 0;
}


int recv_reply_header(const int socket, reply_t *reply) {
    /* function that receives transaction ID, op_code & server_error_code members from socket */
    if (recv_common_header(socket, &reply->header) == -1) return -1;

    /* receive server_error_code */
    if (recv_msg(socket, (char *) &reply->server_error_code, sizeof(int32_t)) == -1) {
        perror("Receive server_error_code error");
        close(socket); return -1;
    }
    reply->server_error_code = (int32_t) ntohl(reply->server_error_code);

    return 0;
}


int recv_num_items(const int socket, reply_t *reply) {
    /* function that receives num_items member from socket */
    if (recv_msg(socket, (char *) &reply->num_items, sizeof(uint32_t)) == -1) {
        perror("Receive num_items error");
        close(socket); return -1;
    }
    reply->num_items = ntohl(reply->num_items);

    return 0;
}


int recv_key(const int socket, item_t *item) {
    /* function that receives the key member from socket */
    if (recv_msg(socket, (char *) &item->key, sizeof(int32_t)) == -1) {
        perror("Receive key error");
        close(socket); return -1;
    }
    item->key = (int32_t) ntohl(item->key);

    return 0;
}


int recv_values(const int socket, item_t *item) {
    /* function that receives value members from socket */

    /* receive value1 */
    if (read_line(socket, item->value1, VALUE1_MAX_STR_SIZE) == -1) {
        perror("Receive value1 error");
        close(socket); return -1;
    }

    /* receive value2 */
    if (recv_msg(socket, (char *) &item->value2, sizeof(int32_t)) == -1) {
        perror("Receive value2 error");
        close(socket); return -1;
    }
    item->value2 = (int32_t) ntohl(item->value2);

    /* receive value3 */
    uint32_t tmp;
    if (recv_msg(socket, (char *) &tmp, sizeof(float)) == -1) {
        perror("Receive value3 error");
        close(socket); return -1;
    }
    tmp = ntohl(tmp);
    memcpy((char *) &item->value3, (char *) &tmp, sizeof(float));

    return 0;
}
