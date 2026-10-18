#ifndef CONTRACT_H
#define CONTRACT_H

#include <stdio.h>
#include <stdlib.h>

/* Nếu biên dịch ở chế độ Release (có NDEBUG), các macro này sẽ bị loại bỏ để tối ưu hiệu suất */
#ifdef NDEBUG
    #define REQUIRE(expr)   ((void)0)
    #define ENSURE(expr)    ((void)0)
    #define INVARIANT(expr) ((void)0)
#else
    #define CONTRACT_FAIL_(tag, expr_str) \
        do { \
            (void)fprintf(stderr, "[%s VIOLATION] %s — in %s(), %s:%d\n", \
                          (tag), (expr_str), __func__, __FILE__, __LINE__); /* NOTINT */ \
            abort(); \
        } while (0)

    #define REQUIRE(expr)   do { if (!(expr)) { CONTRACT_FAIL_("REQUIRE",   #expr); } } while (0)
    #define ENSURE(expr)    do { if (!(expr)) { CONTRACT_FAIL_("ENSURE",    #expr); } } while (0)
    #define INVARIANT(expr) do { if (!(expr)) { CONTRACT_FAIL_("INVARIANT", #expr); } } while (0)
#endif

#endif /* CONTRACT_H */