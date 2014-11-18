#ifndef SOCK_SERV_H
#define SOCK_SERV_H

#define SIMP_SOCK_SEND(fd, res, len) \
    write(fd, &len, sizeof(int)); \
    write(fd, res, sizeof(char)*len);


int simple_sock_serv(int num_workers, void (*callback)(char *, int, int), char *sock_path, int backlog, int bufflen);

#endif
