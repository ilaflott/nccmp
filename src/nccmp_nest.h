#ifndef NCCMP_NEST_H
#define NCCMP_NEST_H

#include <nccmp_user_type.h>

typedef struct nccmp_nest_t {
    nccmp_user_type_t *type1, *type2;
    int index[NCCMP_MAX_COMPOUND_FIELD_DIMS];
    int num_dims;
    struct nccmp_nest_t *prev;
} nccmp_nest_t;

void nccmp_build_nest_index_str(nccmp_nest_t *nest, int fortran, char *result);

/* Examples:
     myField[1,2].child[0,1]
     myVlen[1,1][0]
*/
void nccmp_build_nest_str(nccmp_nest_t *nest, int fortran, char *result);

/* Builds dotted string without indices. Example: myField.child.myShort */
void nccmp_build_nest_str_no_index(nccmp_nest_t *nest, int fortran, char *result);

nccmp_nest_t* nccmp_create_nest();

void nccmp_destroy_nest(nccmp_nest_t* item);

int nccmp_nest_cardinal_index(nccmp_nest_t *nest);

#endif
