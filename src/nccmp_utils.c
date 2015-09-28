#include <stddef.h>
#include <nccmp_common.h>
#include <nccmp_utils.h>

nccmp_int_pairs_t* nccmp_create_int_pairs(size_t n)
{
    nccmp_int_pairs_t *result = XMALLOC(nccmp_int_pairs_t, 1);

    result->items = XMALLOC(nccmp_int_pair_t, n);
    memset(result->items, 0, sizeof(nccmp_int_pair_t) * n);
    result->size = n;

    return result;
}

void nccmp_destroy_int_pairs(nccmp_int_pairs_t *pairs)
{
    XFREE(pairs->items);
    XFREE(pairs);
}

int nccmp_max(int a, int b)
{
    return a > b ? a : b;
}

int nccmp_min(int a, int b)
{
    return a > b ? b : a;
}

#define PRODUCT(TYPE) \
TYPE nccmp_product_##TYPE(TYPE *items, TYPE first, TYPE last) \
{ \
    TYPE result = 1; \
    \
    for(; first <= last; ++first) { \
        result *= items[first]; \
    } \
    return result; \
}

PRODUCT(int)
PRODUCT(size_t)
#undef PRODUCT
