#include <nccmp_dim.h>
#include <nccmp_error.h>
#include <nccmp_common.h>

void nccmp_destroy_dims(nccmp_dim_t** dims, int* ndims)
{
    *ndims = 0;

    if (!dims) {
        return;
    }

    XFREE(*dims);
}

nccmp_dim_t* nccmp_get_dim_by_id(nccmp_dim_t* dims, const int ndims, const int id)
{
    nccmp_dim_t *result = NULL;
    int i;

    if(!dims || (id < 0) || (id >= ndims)) {
        goto recover;
    }

    for(i = 0; i < ndims; ++i) {
        if (id == dims[i].dimid) {
            result = & dims[i];
            break;
        }
    }

recover:
    return result;
}

void nccmp_load_dims(int ncid, nccmp_dim_t* dims, int ndims)
{
    int status, i;

    int *dimids = XMALLOC(int, ndims);
    int include_parents = 1;

    status = nc_inq_dimids(ncid, & ndims, dimids, include_parents);
    HANDLE_NC_ERROR(status);

    for(i=0; i < ndims; ++i) {
        dims[i].dimid = dimids[i];
        status = nc_inq_dim(ncid, dimids[i], dims[i].name, & dims[i].len);
        HANDLE_NC_ERROR(status);
    }

    XFREE(dimids);
}

void nccmp_init_dims(int ncid, nccmp_dim_t** dims, int* ndims)
{
    int i, status, include_parents = 1;

    nccmp_destroy_dims(dims, ndims);

    status = nc_inq_dimids(ncid, ndims, 0, include_parents);
    HANDLE_NC_ERROR(status);

    *dims = XMALLOC(nccmp_dim_t, *ndims);
    for(i = 0; i < *ndims; ++i) {
        (*dims)[i].dimid = -1;
        (*dims)[i].len = 0;
        strcpy((*dims)[i].name, "");
    }
}

