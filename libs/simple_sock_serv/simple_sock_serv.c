#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "simple_sock_serv.h"

static int continue_flag = 1;
static int listenfd;
static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
static int BUFFLEN;

static void quit(char *message)
{
    perror(message);
    exit(-1);
}

static void stop_it(int sig)
{
    pthread_mutex_lock(&accept_mutex);
    shutdown(listenfd, SHUT_RDWR);
    continue_flag = 0;
    pthread_mutex_unlock(&accept_mutex);
}

static void *worker(void *argsptr)
{
    char buf[BUFFLEN];
    struct sockaddr_in cli_addr;
    memset((char *) &cli_addr, 0, sizeof(cli_addr));
    socklen_t length;
    struct worker_args args;
    int socketfd;
    args = * (struct worker_args *) argsptr;
    while(1)
    {
        pthread_mutex_lock(&args.accept_mutex);
        if(!*args.continue_flagptr)
            break;
        length = sizeof(cli_addr);
        socketfd = accept(args.listenfd, (struct sockaddr *) &cli_addr, &length);
        pthread_mutex_unlock(&args.accept_mutex);
        if(socketfd >= 0)
        {
            int len = -1;
            read(socketfd, &len, sizeof(len));
            if(len > BUFFLEN-1)
                len = BUFFLEN;
            buf[len] = '\0';
            read(socketfd, buf, BUFFLEN*sizeof(*buf));
            args.do_it(buf, strlen(buf), socketfd);
            close(socketfd);
        }
        sleep(1);
    }
    return NULL;
}
    

void simple_sock_serv(int num_workers, void (*do_it)(char *, int, int), int port, int backlog, int bufflen)
{
    BUFFLEN = bufflen;
    static struct sockaddr_in serv_addr; 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port);

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        quit("couldn't open listening socket");
    if(bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        quit("couldn't bind to port");
    if(listen(listenfd, backlog) < 0)
        quit("listen failed");

    signal(SIGTERM, &stop_it);
    signal(SIGINT, &stop_it);

    struct worker_args args = {
        .listenfd = listenfd,
        .do_it = do_it,
        .accept_mutex = accept_mutex,
        .continue_flagptr = &continue_flag
    };

    pthread_t thrds[num_workers];
    for(int i = 1; i<num_workers; i++)
        pthread_create(&thrds[i], NULL, &worker, &args);

    worker(&args); 
    for(int i = 1; i<num_workers; i++)
        pthread_join(thrds[i], NULL);
}