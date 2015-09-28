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

int nccmp_max(int a, int b);

int nccmp_min(int a, int b);

#define PRODUCT(TYPE) TYPE nccmp_product_##TYPE(TYPE *items, TYPE first, TYPE last);
PRODUCT(int)
PRODUCT(size_t)
#undef PRODUCT

#endif
