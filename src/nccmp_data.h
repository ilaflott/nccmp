#ifndef NCCMP_DATA_H
#define NCCMP_DATA_H

#include <nccmp_group.h>
#include <nccmp_nest.h>
#include <nccmp_opt.h>

int nccmp_data(nccmp_state_t* state, nccmp_strlist_t* varnames, nccmp_group_t* group1, nccmp_group_t* group2);

/* Return 0 if no difference. The value is returned in the variable absdiff_output. */
int nccmp_cmp_tolerance(nccmp_state_t* state, double value1, double value2, double* absdiff_output);

int nccmp_data_start_threads(nccmp_state_t* state, nccmp_strlist_t* var_names,
                             const char* group_name1, const char* group_name2);

int nccmp_cmp_var_user_type_compound_field_ids(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits, void* data1, void* data2,
        nccmp_user_type_t* comp_type1, nccmp_user_type_t* comp_type2, 
        nccmp_darray_t *fieldids);

int nccmp_cmp_var_user_type_vlen_atomic(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits);

int nccmp_cmp_var_user_type_vlen_compound(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits);

int nccmp_cmp_var_user_type_vlen_enum(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits);

int nccmp_cmp_var_user_type_nest_compound(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, unsigned char *data1, unsigned char *data2,
        nccmp_nest_t *nest);

/* Dispatches the correct handler for the nest types input. */
int nccmp_cmp_var_user_type_nest_select(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest);

int nccmp_cmp_var_user_type_nest_vlen(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest);

int nccmp_cmp_var_user_type_vlen_opaque(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits);

#endif
