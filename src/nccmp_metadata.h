#ifndef NCCMP_METADATA_H
#define NCCMP_METADATA_H

#include <nccmp_nc_type.h>
#include <nccmp_opt.h>
#include <nccmp_state.h>
#include <nccmp_var.h>

int nccmp_cmp_att(int ncid1, int ncid2, int varid1, int varid2, 
        const char* name, const char* varname, nccmp_opt_t* opts);

/* Assumes that both attributes have same length. */
int nccmp_cmp_att_val(int nc1, int nc2, int varid1, int varid2, 
        const char* name, int len1, int len2, nc_type type);

int nccmp_cmp_metadata_var_types(nccmp_state_t *state, nccmp_strlist_t* varlist, int ncid1, int ncid2);

int nccmp_cmp_metadata_var_dim_names(const nccmp_state_t *state, nccmp_strlist_t* varlist,
        const char* recname1, const char* recname2, int ncid1, int ncid2);

int nccmp_cmp_metadata_var_atts(nccmp_state_t *state, nccmp_strlist_t* varlist, int ncid1, int ncid2);

int nccmp_cmp_metadata(nccmp_state_t* state, nccmp_strlist_t* var_names, int ncid1, int ncid2);

int nccmp_cmp_metadata_dim_count(nccmp_state_t *state);

int nccmp_cmp_metadata_dim_lens(nccmp_state_t *state, int ncid1, int ncid2);

/* Compares record names and lengths. */
int nccmp_cmp_rec_info(nccmp_state_t* state, nccmp_strlist_t* var_names, int ncid1, int ncid2);

int nccmp_dump_info(nccmp_state_t* opts, int ncid);

int nccmp_dump_info_all(nccmp_state_t* state, int ncid);

int nccmp_dump_info_format(int ncid);

int nccmp_dump_info_headerpad(int ncid);

int nccmp_dump_info_path(nccmp_state_t* state, int ncid);

int nccmp_dump_info_vars(int ncid);

void nccmp_get_rec_names(nccmp_opt_t *opts, int ncid1, int ncid2,
        int recid1, int recid2, char* recname1, char* recname2);

int nccmp_file_formats(nccmp_opt_t* opts, int ncid1, int ncid2);

int nccmp_global_atts(nccmp_opt_t* opts, int ncid1, int ncid2);

int nccmp_cmp_metadata_user_types(nccmp_opt_t *opts, int ncid1, int ncid2);

int nccmp_header_pad(nccmp_opt_t* opts, int ncid1, int ncid2);

int nccmp_var_encoding(nccmp_state_t *state, nccmp_strlist_t* varlist, int ncid1, int ncid2);

#endif
