/* 
 * File:   nccmp_buffer.h
 * Author: rsz
 *
 * Created on December 22, 2015, 11:10 AM
 */

#ifndef NCCMP_BUFFER_H
#define NCCMP_BUFFER_H

#include <nccmp_darray.h>
#include <nccmp_strlist.h>
#include <nccmp_utils.h>
#include <nccmp_var.h>

typedef struct nccmp_buffer_t {
    size_t num_bytes;
    union {
        char               *p_text;
        double             *p_double;
        float              *p_float;
        int                *p_int;
        long long          *p_longlong;
        short              *p_short;
        signed char        *p_schar;
        unsigned int       *p_uint;
        unsigned char      *p_uchar;
        unsigned long long *p_ulonglong;
        unsigned short     *p_ushort;
    } bytes;
} nccmp_buffer_t;

#define NCCMP_BUFFER_ITEM(ARRAY, INDEX) ( (nccmp_buffer_t*) ARRAY->items[INDEX] )

size_t nccmp_compute_max_buffer_byte_size(
        nccmp_var_t *vars,
        int nvars,
        nccmp_strlist_t *var_names);

/* Returns newly allocated buffer instance. */
nccmp_buffer_t* nccmp_create_buffer(size_t num_bytes);

/* Creates a buffer per thread. */
nccmp_darray_t* nccmp_create_buffer_array(
        int nthreads, 
        nccmp_var_t *vars,
        int nvars,
        nccmp_strlist_t *var_names);

/* Deep destruction. */
void nccmp_destroy_buffer_array(nccmp_darray_t *array);

#endif /* NCCMP_BUFFER_H */

