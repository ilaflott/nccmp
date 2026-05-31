#ifndef NCCMP_VAR_H
#define NCCMP_VAR_H

#include <netcdf.h>
#include <nccmp_constants.h>
#include <nccmp_nc_type.h>
#include <nccmp_opt.h>
#include <nccmp_strlist.h>

typedef struct nccmp_var_t {
    char    name[NC_MAX_NAME];
    int     varid;
    size_t   len; /* # elements per record (or overall if static). */
    size_t  dimlens[NCCMP_MAX_DIMS_IN_VAR];
    nc_type type;
    int     ndims;
    int     dimids[NCCMP_MAX_DIMS_IN_VAR];
    char    has_rec_dim;
    int     natts;
    char    has_missing;
    nccmp_nc_type_t missing;
} nccmp_var_t;

/* Gets list of all variable names in both input files. */
int all_varnames(nccmp_opt_t* opts, nccmp_strlist_t* list, int ncid1, int ncid2);

/* Copy attribute type to same type as var, just in case of mismatch. */
void broadcast_missing(nc_type var_type, nc_type att_type, nccmp_nc_type_t *values);

int exclude_vars(nccmp_opt_t* opts, int ncid1, int ncid2, nccmp_strlist_t* result, nccmp_strlist_t* exclude);

/* Returns index into var_struct array if found using name, otherwise -1. */
int find_var(const char * name, nccmp_var_t *vars, int nvars);

char get_missing(int ncid, nccmp_var_t * var, const char* attname);

/* Get vars to use to do cmp based on input varname and exclusion lists. */
nccmp_strlist_t* make_cmp_var_name_list(nccmp_opt_t* opts, int ncid1, int ncid2);

/* valid[out]: List of valid var names.
   found1,found2: If non-NULL, each name found in the respective file's
                  group vars is recorded here (for cross-file tracking across groups). */
int validate_var_names(nccmp_opt_t* opts, nccmp_strlist_t* names, nccmp_strlist_t** valid,
        int ncid1, int ncid2, nccmp_var_t *vars1, nccmp_var_t *vars2,
        int nvars1, int nvars2,
        nccmp_strlist_t* found1, nccmp_strlist_t* found2);

void var_info_destroy(nccmp_var_t** vars, int* nvars);

/* Read all the file's metadata for variables. */
void var_info_get(int ncid, nccmp_var_t* vars, int nvars, int debug, int color);

void var_info_init(int ncid, nccmp_var_t** vars, int* nvars);

const nccmp_var_t* nccmp_find_var_by_id(const nccmp_var_t *vars, const int nvars, const int id);

const nccmp_var_t* nccmp_find_var_by_name(const nccmp_var_t *vars, const int nvars, const char * name);

int nccmp_get_var_num_atts_by_id(const nccmp_var_t *vars, const int nvars, const int varid);

int nccmp_get_var_num_dims(const nccmp_var_t *vars, const int nvars, const int varid);

int nccmp_get_var_dimid(const nccmp_var_t *var, const int dim_index);

int nccmp_get_var_dimid_by_id(const nccmp_var_t *vars, const int nvars, const int varid, const int dim_index);

size_t nccmp_get_var_last_dim_num_items(const nccmp_var_t *var);

int nccmp_get_var_type(const nccmp_var_t *var);

void nccmp_print_vars(nccmp_var_t *vars, int nvars, int color);

#endif
