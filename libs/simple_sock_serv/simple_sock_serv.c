#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include "simple_sock_serv.h"
#include "block_q.h"

static volatile sig_atomic_t halt_flag = 0;
static int listenfd;
static int BUFFLEN;
static sem_t thrd_count;
static block_q conn_q;
static void (*callback)(char *, int, int);

static void stop_it(int sig)
{
    shutdown(listenfd, SHUT_RDWR);
    halt_flag = 1;
}

static void *worker(void *ign)
{
    char buf[BUFFLEN];
    int socketfd = block_q_take(&conn_q);
    if(socketfd >= 0)
    {
        int len = -1;
        while(read(socketfd, &len, sizeof(len)) > 0 && !halt_flag)
        {
            if(len > BUFFLEN-1)
                len = BUFFLEN;
            buf[len] = '\0';
            read(socketfd, buf, len*sizeof(*buf));
            (*callback)(buf, strlen(buf), socketfd);
        }
        close(socketfd);
    }
    sem_post(&thrd_count);
    return NULL;
}
    
static int set_handlers()
{
    struct sigaction finish = {
        .sa_handler = &stop_it,
        .sa_flags = 0
    };
    if(sigaction(SIGTERM, &finish, 0) < 0)
        return -1;
    if(sigaction(SIGINT, &finish, 0) < 0)
        return -1;

    struct sigaction pip = {
        .sa_handler = SIG_IGN,
        .sa_flags = 0
    };

    if(sigaction(SIGPIPE, &pip, 0) < 0)
        return -1;

    return 0;
}

int simple_sock_serv(int max_conns, void (*callback_func)(char *, int, int), char *sock_path, int backlog, int bufflen)
{
    BUFFLEN = bufflen;
    struct sockaddr_un serv_addr; 
    memset(&serv_addr, 0, sizeof(struct sockaddr_un));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, sock_path, sizeof(serv_addr.sun_path) - 1);

    if((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        return -1;
    if(bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        return -1;
    if(listen(listenfd, backlog) < 0)
        return -1;

    if(set_handlers() < 0)
        return -1;


    callback = callback_func;

    /* a blocking queue seems like overkill
     * for passing fd's to threads, but I had an implementation 
     * already.  Not synchronizing this passing 
     * leaves the possilbity for unlikely race conditions.
     * The alternatives would be casting
     * int to void*, or mallocing an int and expecting
     * the thread to free it.  
     */

    int q_buffer;
    block_q_init(&conn_q, 1, &q_buffer);
    sem_init(&thrd_count, 0, max_conns);

    int socketfd;
    while(!halt_flag)
    {
        socketfd = accept(listenfd, NULL, NULL);
        int wait_success = sem_trywait(&thrd_count);
        if(wait_success < 0 || halt_flag)
        {
            int allowed = 0;
            write(socketfd, &allowed, sizeof(allowed));
            close(socketfd);
        }
        else
        {
            int allowed = 1;
            write(socketfd, &allowed, sizeof(allowed));
            block_q_put(&conn_q, socketfd);
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_t thrd;
            pthread_create(&thrd, &attr, &worker, NULL);
            pthread_attr_destroy(&attr);
        }
    }
    block_q_destroy(&conn_q);
    unlink(sock_path);
    return 0;
}
