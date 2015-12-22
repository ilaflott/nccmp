#include <nccmp_buffer.h>
#include <nccmp_ncinfo.h>
#include <nccmp_utils.h>

size_t nccmp_compute_max_buffer_num_bytes(
        nccmp_var_t *vars,
        int nvars,
        nccmp_strlist_t *var_names)
{
    int i;
    const nccmp_var_t *var;
    size_t nbytes = 0, nitems;
    
    for(i = 0; i < var_names->size; ++i) {
        var = nccmp_find_var_by_name(vars, nvars, var_names->items[i]);
        if (var) {
            /* Make one item larger for comparison sentinel. */
            nitems = nccmp_get_var_last_dim_num_items(var) + 1;
            nbytes = nccmp_max_size_t(nccmp_nc_type_size(var->type) * nitems, nbytes);
        }
    }
    
    return nbytes;
}

nccmp_buffer_t* nccmp_create_buffer(size_t num_bytes)
{
    nccmp_buffer_t *result = XMALLOC(nccmp_buffer_t, 1);
    result->num_bytes = num_bytes;
    result->bytes.p_uchar = XMALLOC(unsigned char, num_bytes);
    
    return result;
}

nccmp_darray_t* nccmp_create_buffer_array(
        int nthreads, 
        nccmp_var_t *vars,
        int nvars,
        nccmp_strlist_t *var_names)
{
    int i;
    nccmp_darray_t *result = nccmp_darray_create(nthreads);
    size_t num_bytes = nccmp_compute_max_buffer_num_bytes(vars, nvars, var_names);
    
    for(i = 0; i < nthreads; ++i) {
        nccmp_darray_append(result, nccmp_create_buffer(num_bytes));
    }
    
    return result;
}

void nccmp_destroy_buffer_array(nccmp_darray_t *array)
{
    int i;
    nccmp_buffer_t *item;
    
    for(i = 0; i < array->num_items; ++i) {
        item = (nccmp_buffer_t*) array->items[i];
        XFREE(item->bytes.p_uchar);
        XFREE(item);
    }
    
    nccmp_darray_destroy(array);
}

