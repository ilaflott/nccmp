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

#define NCCMP_MAX(TYPE) \
TYPE nccmp_max_##TYPE(TYPE a, TYPE b) { return a > b ? a : b; }
NCCMP_MAX(double)
NCCMP_MAX(int)
NCCMP_MAX(size_t)
#undef NCCMP_MAX

#define NCCMP_MIN(TYPE) \
TYPE nccmp_min_##TYPE(TYPE a, TYPE b) { return a > b ? b : a; }
NCCMP_MIN(double)
NCCMP_MIN(int)
NCCMP_MIN(size_t)
#undef NCCMP_MIN

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

void nccmp_replace_delimited_prefix(const char *old_str, char *new_prefix, char delim, char **result)
{
    char tmp[NC_MAX_NAME];
    strcpy(tmp, new_prefix);

    // Find location of first delim to trim old prefix.
    char *substring = strchr(old_str, delim);
    if (substring) {
        strcat(tmp, substring);
    }
    *result = XMALLOC(char, strlen(tmp) + 1);
    strcpy(*result, tmp);
}
