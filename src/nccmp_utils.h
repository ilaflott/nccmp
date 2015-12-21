#ifndef NCCMP_UTILS_H
#define NCCMP_UTILS_H

#include <nccmp_opt.h>

typedef struct nccmp_pair_int_t {
    int first;
    int second;
} nccmp_pair_int_t;

nccmp_pair_int_t* nccmp_create_pair_int(size_t n);

#define NCCMP_MAX(TYPE) TYPE nccmp_max_##TYPE(TYPE a, TYPE b);
NCCMP_MAX(double)
NCCMP_MAX(int)
NCCMP_MAX(size_t)
#undef NCCMP_MAX

#define NCCMP_MIN(TYPE) TYPE nccmp_min_##TYPE(TYPE a, TYPE b);
NCCMP_MIN(double)
NCCMP_MIN(int)
NCCMP_MIN(size_t)
#undef NCCMP_MIN

#define PRODUCT(TYPE) TYPE nccmp_product_##TYPE(TYPE *items, TYPE first, TYPE last);
PRODUCT(int)
PRODUCT(size_t)
#undef PRODUCT

/* Allocates and populates new result. Caller owns result memory. */
void nccmp_replace_delimited_prefix(const char* old_str, const char* prefix,
		char delim, char **result);

#endif
