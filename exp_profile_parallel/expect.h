#ifndef EXPECT_H
#define EXPECT_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define EXPECT(expr)                                    \
    do {                                                \
        if (!(expr)) {                                  \
            fprintf(stderr, "%s:%i: %s\n",              \
                    __FILE__, __LINE__, # expr);        \
            abort();                                    \
        }                                               \
    } while (0)

#define EXPECT_ERRNO(expr)                                              \
    do {                                                                \
        if (!(expr)) {                                                  \
            fprintf(stderr, "%s:%i: %s\n  %s\n",                        \
                    __FILE__, __LINE__, # expr, strerror(errno));       \
            abort();                                                    \
        }                                                               \
    } while (0)


#endif


