#ifndef INCLUDE_CONTAINER_OF_H
#define INCLUDE_CONTAINER_OF_H

#include <stddef.h>

/* Inspired via the Linux-kernel macro 'container_of' */
#define container_of(ptr, type, member) \
    ((type *) ((char*)(ptr) - offsetof(type, member)))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

#endif
