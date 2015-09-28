#ifndef NCCMP_DIM_H
#define NCCMP_DIM_H 1

#include <netcdf.h>
#include <nccmp_constants.h>

typedef struct nccmp_dim_t {
    int     dimid;
    size_t  len;
    char    name[NC_MAX_NAME];
} nccmp_dim_t;

void nccmp_destroy_dims(nccmp_dim_t** dims, int* ndims);

nccmp_dim_t* nccmp_get_dim_by_id(nccmp_dim_t* dims, const int ndims, const int id);

/* Get dim info for file. */
void nccmp_load_dims(int ncid, nccmp_dim_t* dims, int ndims);

void nccmp_init_dims(int ncid, nccmp_dim_t** dims, int* ndims);

#endif
