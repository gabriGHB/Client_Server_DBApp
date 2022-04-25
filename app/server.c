#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "DS-MandatoryExercise/utils.h"
#include "DS-MandatoryExercise/netUtils.h"
#include "DS-MandatoryExercise/dbms/dbms.h"

/* prototypes */
void *service_thread(void *args);
void set_server_error_code_std(reply_t *reply, int req_error_code);

/* services */
void init_db(reply_t *reply);
void insert_item(request_t *request, reply_t *reply);
void get_item(request_t *request, reply_t *reply);
void modify_item(request_t *request, reply_t *reply);
void delete_item(request_t *request, reply_t *reply);
void item_exists(request_t *request, reply_t *reply);
void get_num_items(reply_t *reply);


/* connection queue */
int conn_q[MAX_CONN_BACKLOG];   /* array of client sockets; used as a connection queue */
int conn_q_size = 0;            /* current number of backlogged connections */
int service_th_pos = 0;         /* connection queue position used by service threads to handle connections */

#define THREAD_POOL_SIZE 5      /* max number of service threads running */

/* mutex and cond vars for conn_q access */
pthread_mutex_t mutex_conn_q;
pthread_cond_t cond_conn_q_not_empty;
pthread_cond_t cond_conn_q_not_full;

pthread_mutex_t mutex_db;                   /* mutex for atomic operations on the DB */
pthread_attr_t th_attr;                     /* service thread attributes */
pthread_t thread_pool[THREAD_POOL_SIZE];    /* array of service threads */


void set_server_error_code_std(reply_t *reply, const int req_error_code) {
    /* most services follow this error code model */
    switch (req_error_code) {
        case 0: reply->server_error_code = SRV_SUCCESS; break;
        case -1: reply->server_error_code = SRV_ERROR; break;
        default: break;
    }
}


void * service_thread(void *args) {
    while (TRUE) {
        int client_socket;
        /* copy client socket descriptor and free the original */
        pthread_mutex_lock(&mutex_conn_q);

        /* there are no connections to handle, so sleep */
        while (conn_q_size == 0)
            pthread_cond_wait(&cond_conn_q_not_empty, &mutex_conn_q);

        client_socket = conn_q[service_th_pos];
        service_th_pos = (service_th_pos + 1) % MAX_CONN_BACKLOG;
        conn_q_size -= 1;

        if (conn_q_size == MAX_CONN_BACKLOG - 1)
            pthread_cond_signal(&cond_conn_q_not_full);

        pthread_mutex_unlock(&mutex_conn_q);

        /* handle connection now */
        request_t request;
        /* receive transaction ID & op_code */
        if (recv_common_header(client_socket, &request.header) == -1) continue;

        /* set up server reply */
        reply_t reply;
        reply.header.id = request.header.id;
        reply.header.op_code = request.header.op_code;

        /* check whether client request is valid and execute it */
        switch (request.header.op_code) {
            case INIT:
                /* execute client request */
                init_db(&reply);

                /* send server reply */
                if (send_reply_header(client_socket, &reply) == -1) continue;
                break;
            case SET_VALUE:
                /* receive rest of client request */
                if (recv_key(client_socket, &request.item) == -1 ||
                recv_values(client_socket, &request.item) == -1) continue;

                /* execute client request */
                insert_item(&request, &reply);

                /* send server reply */
                if (send_reply_header(client_socket, &reply) == -1) continue;
                break;
            case GET_VALUE:
                /* receive rest of client request */
                if (recv_key(client_socket, &request.item) == -1) continue;

                /* execute client request */
                get_item(&request, &reply);

                /* send server reply */
                if (send_reply_header(client_socket, &reply) == -1 ||
                        send_values(client_socket, &reply.item) == -1) continue;
                break;
            case MODIFY_VALUE:
                /* receive rest of client request */
                if (recv_key(client_socket, &request.item) == -1 ||
                    recv_values(client_socket, &request.item) == -1) continue;

                /* execute client request */
                modify_item(&request, &reply);

                /* send server reply */
                if (send_reply_header(client_socket, &reply) == -1) continue;
                break;
            case DELETE_KEY:
                /* receive rest of client request */
                if (recv_key(client_socket, &request.item) == -1) continue;

                /* execute client request */
                delete_item(&request, &reply);

                /* send server reply */
                if (send_reply_header(client_socket, &reply) == -1) continue;
                break;
            case EXIST:
                /* receive rest of client request */
                if (recv_key(client_socket, &request.item) == -1) continue;

                /* execute client request */
                item_exists(&request, &reply);

                /* send server reply */
                if (send_reply_header(client_socket, &reply) == -1) continue;
                break;
            case NUM_ITEMS:
                /* execute client request */
                get_num_items(&reply);

                /* send server reply */
                if (send_reply_header(client_socket, &reply) == -1 ||
                send_num_items(client_socket, &reply) == -1) continue;
                break;
            default:    /* invalid operation */
                fprintf(stderr, "Requested invalid operation\n");
                close(client_socket); continue;
        } // end switch
    } // end outer while
}


void init_db(reply_t *reply) {
    /* execute client request */
    pthread_mutex_lock(&mutex_db);

    int req_error_code = db_empty_db();

    pthread_mutex_unlock(&mutex_db);

    /* fill server reply */
    set_server_error_code_std(reply, req_error_code);
}


void insert_item(request_t *request, reply_t *reply) {
    /* execute client request */
    pthread_mutex_lock(&mutex_db);

    int req_error_code = db_write_item(request->item.key, request->item.value1,
                                       &(request->item.value2),&(request->item.value3), CREATE);

    pthread_mutex_unlock(&mutex_db);

    /* fill server reply */
    set_server_error_code_std(reply, req_error_code);
}


void get_item(request_t *request, reply_t *reply) {
    /* execute client request */
    pthread_mutex_lock(&mutex_db);

    int req_error_code = db_read_item(request->item.key, reply->item.value1,
                                      &(reply->item.value2), &(reply->item.value3));

    pthread_mutex_unlock(&mutex_db);

    /* fill server reply */
    reply->item.key = request->item.key;
    set_server_error_code_std(reply, req_error_code);
}


void modify_item(request_t *request, reply_t *reply){
    /* execute client request */
    pthread_mutex_lock(&mutex_db);

    int req_error_code = db_write_item(request->item.key, request->item.value1,
                                       &(request->item.value2), &(request->item.value3), MODIFY);

    pthread_mutex_unlock(&mutex_db);

    /* fill server reply */
    set_server_error_code_std(reply, req_error_code);
}


void delete_item(request_t *request, reply_t *reply) {
    /* execute client request */
    pthread_mutex_lock(&mutex_db);

    int req_error_code = db_delete_item(request->item.key);

    pthread_mutex_unlock(&mutex_db);

    /* fill server reply */
    set_server_error_code_std(reply, req_error_code);
}


void item_exists(request_t *request, reply_t *reply) {
    /* execute client request */
    pthread_mutex_lock(&mutex_db);

    int req_error_code = db_item_exists(request->item.key);

    pthread_mutex_unlock(&mutex_db);

    /* fill server reply */
    switch (req_error_code) {
        case 1: reply->server_error_code = SRV_EXISTS; break;
        case 0: reply->server_error_code = SRV_NOT_EXISTS; break;
        default: break;
    }
}


void get_num_items(reply_t *reply) {
    /* execute client request */
    pthread_mutex_lock(&mutex_db);

    int num_items = db_get_num_items();

    pthread_mutex_unlock(&mutex_db);

    /* fill server reply */
    if (num_items == -1) reply->server_error_code = SRV_ERROR;
    else {
        reply->server_error_code = SRV_SUCCESS;
        reply->num_items = num_items;
    }
}


void shutdown_server() {
    /* destroy server resources before shutting it down */
    pthread_mutex_destroy(&mutex_conn_q);
    pthread_mutex_destroy(&mutex_db);
    pthread_attr_destroy(&th_attr);
    fprintf(stderr, "Shutting down server\n");
    exit(0);
}


int main(int argc, char **argv) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int server_sd, client_sd;
    int val = 1;

    if (argc != 2) {
        fprintf(stderr, "Usage server <PORT>\n"); return -1;
    }

    int server_port;
    if (str_to_num(argv[1], (void *) &server_port, INT) == -1) {
        perror("Invalid server port"); return -1;
    }

    /* set up connection queue */
    pthread_mutex_init(&mutex_conn_q, NULL);
    pthread_cond_init(&cond_conn_q_not_empty, NULL);
    pthread_cond_init(&cond_conn_q_not_full, NULL);
    /* conn_q position used by main thread to enqueue connections */
    int producer_pos = 0;

    /* make service threads detached */
    pthread_attr_init(&th_attr);
    pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_DETACHED);

    pthread_mutex_init(&mutex_db, NULL);    /* for atomic DB operations */

    /* set up SIGINT (CTRL+C) signal handler to shut down server */
    struct sigaction keyboard_interrupt;
    keyboard_interrupt.sa_handler = shutdown_server;
    keyboard_interrupt.sa_flags = 0;
    sigemptyset(&keyboard_interrupt.sa_mask);
    sigaction(SIGINT, &keyboard_interrupt, NULL);

    /* get server up & running */
    if ((server_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Can't create server socket"); return -1;
    }

    setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    if (bind(server_sd, (struct sockaddr *) &server_addr, sizeof server_addr) == -1) {
        perror("Server socket binding error"); return -1;
    }

    if (listen(server_sd, MAX_CONN_BACKLOG) == -1) {
        perror("Server listen error"); return -1;
    }

    /* now create thread pool */
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&thread_pool[i], &th_attr, service_thread, NULL);
    }

    while (TRUE) {
        printf("Press Ctrl + C to shut down server\n");
        printf("Waiting for connections...\n");

        client_sd = accept(server_sd, (struct sockaddr *) &client_addr, &addr_size);
        if (client_sd == -1) {
            perror("Server accept error"); return -1;
        }

        printf("Accepted connection IP: %s    Port: %d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        /* add connection to conn_q backlog */
        pthread_mutex_lock(&mutex_conn_q);

        /* if the connection queue is full, the main server thread sleeps:
         * no new connections can be opened until one is processed */
        while (conn_q_size == MAX_CONN_BACKLOG)
            pthread_cond_wait(&cond_conn_q_not_full, &mutex_conn_q);

        /* enqueue new connection */
        conn_q[producer_pos] = client_sd;
        producer_pos = (producer_pos + 1) % MAX_CONN_BACKLOG;
        conn_q_size += 1;

        /* signal that there are connections to handle */
        if (conn_q_size == 1)
            pthread_cond_signal(&cond_conn_q_not_empty);

        pthread_mutex_unlock(&mutex_conn_q);
    } // END while
}
