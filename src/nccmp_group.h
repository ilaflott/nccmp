#ifndef NCCMP_GROUP_H
#define NCCMP_GROUP_H

#include <netcdf.h>
#include <nccmp_state.h>
#include <nccmp_strlist.h>

typedef struct nccmp_group_t {
    int     id;                    /* Same as ncid in netcdf api. */
    char    name[NC_MAX_NAME];     /* Short name without parent paths. */
    char    fullname[NC_MAX_NAME]; /* Full path name with parents, such as "/a/b/c". */
    size_t  nchildren;
    struct nccmp_group_t** children;
    struct nccmp_group_t* parent;
} nccmp_group_t;

/* Compare all groups selected. */
int nccmp_cmp_groups(nccmp_state_t* state);

nccmp_group_t* nccmp_find_group(nccmp_group_t* groups, const char* name);

nccmp_group_t* nccmp_create_group();

nccmp_group_t* nccmp_create_groups(size_t n);

void nccmp_destroy_group(nccmp_group_t* data);

void nccmp_destroy_groups(nccmp_group_t** data, int* ngroups);

/* @param group[out]: Must be pre-allocated pointer. */
int nccmp_get_group_info(int ncid, nccmp_group_t* group);

#endif
