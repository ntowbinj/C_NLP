#ifndef UTIL_H
#define UTIL_H

int util_sortby_comparefunc(const void *a, const void *b, void *arg);
int *util_sortby_remap(int *key, int len);

#endif
