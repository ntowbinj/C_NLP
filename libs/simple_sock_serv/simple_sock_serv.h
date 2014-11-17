#ifndef SOCK_SERV_H
#define SOCK_SERV_H

struct worker_args {
    int listenfd;
    void (*do_it)(char *, int, int);
    pthread_mutex_t accept_mutex;
    int *continue_flagptr;
};

void simple_sock_serv(int num_workers, void (*do_it)(char *, int, int), char *sock_path, int backlog, int bufflen);

#endif
