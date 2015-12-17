#ifndef NCCMP_UTILS_H
#define NCCMP_UTILS_H

#include <nccmp_opt.h>

typedef struct nccmp_int_pair_t {
    int first;
    int second;
} nccmp_int_pair_t;

typedef struct nccmp_int_pairs_t {
    int              size;
    nccmp_int_pair_t *items;
} nccmp_int_pairs_t;

void nccmp_destroy_int_pairs(nccmp_int_pairs_t *pairs);

nccmp_int_pairs_t* nccmp_create_int_pairs(size_t n);

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

/* Allocates and populates result. */
void nccmp_replace_delimited_prefix(const char* old_str, char* prefix, char delim, char **result);

#endif
