#include <stddef.h>
#include <nccmp_common.h>
#include <nccmp_utils.h>

nccmp_pair_int_t* nccmp_create_pair_int(size_t n)
{
    return XCALLOC(nccmp_pair_int_t, n);
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

void nccmp_replace_delimited_prefix(
		const char *old_str,
		const char *new_prefix,
		char delim,
		char **result)
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
