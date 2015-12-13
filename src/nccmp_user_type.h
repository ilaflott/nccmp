#ifndef NCCMP_USER_TYPE_H
#define NCCMP_USER_TYPE_H

#include <netcdf.h>
#include <nccmp_constants.h>
#include <nccmp_opt.h>
#include <nccmp_utils.h>
#include <nccmp_strlist.h>

struct nccmp_user_types_t; /* Forward declaration. */

typedef struct nccmp_user_type_t {
    nc_type base_type;
    int     dim_sizes[NCCMP_MAX_COMPOUND_FIELD_DIMS];
    struct nccmp_user_types_t *fields;
    char    field_index;
    char   *name;                 /* Base name like "myField". */
    char   *tree_name;            /* Dotted name from root to child, like "myVar.myField1.myChild2". */
    int     num_dims;
    size_t  num_enums;
    size_t  offset;
    struct nccmp_user_type_t *parent;
    size_t  root_size;             /* Total outermost root compound size. */
    size_t  size;                  /* Number of bytes. */
    nc_type type_id;               /* Defined by file metadata. */
    nc_type user_class;            /* Class like NC_BYTE or NC_COMPOUND. */
    int     unique_id;             /* Unique instance id of a type in tree defined by nccmp. */
} nccmp_user_type_t;

typedef struct nccmp_user_types_t {
    size_t num_items;
    nccmp_user_type_t *items;
} nccmp_user_types_t;

void nccmp_build_user_type_field_path_str(nccmp_user_type_t *type, char *result);

/* Build string with root var name and all field names along tree with dot delimiter.
 * @param result Should be pre-allocated.
 *               Example output: "myVar.field1.field2.leafField".
 */
void nccmp_build_user_type_tree_name(nccmp_user_type_t *type, char *result);

nccmp_user_type_t* nccmp_create_user_type(size_t n);

nccmp_user_types_t* nccmp_create_user_types(size_t n);

/* Free members, and the pointer. */
void nccmp_destroy_user_type(nccmp_user_type_t *type);

/* Free members, but not the pointer. */
void nccmp_destroy_user_type_members(nccmp_user_type_t *type);

/* Free members, and the pointer. */
void nccmp_destroy_user_types(nccmp_user_types_t *types);

int nccmp_find_max_user_type_unique_id(nccmp_user_types_t* types);

nccmp_user_type_t* nccmp_find_field_by_unique_id(nccmp_user_types_t* types, int id);

size_t nccmp_get_num_user_type_fields(nccmp_user_type_t *type);

nccmp_user_type_t* nccmp_get_user_type_by_id(nccmp_user_type_t *types, int ntypes, int id);

nccmp_user_type_t* nccmp_get_user_type_by_name(nccmp_user_type_t *types, int ntypes, const char *name);

nccmp_strlist_t* nccmp_get_user_type_names(int ncid1, int ncid2, int ntypes1, int ntypes2,
        nc_type *typeids1, nc_type *typeids2, int clazz);

nccmp_strlist_t* nccmp_get_user_type_compound_field_names(int ncid1, int ncid2,
        const char* name, int debug);

/* Return union of all field names in both user types. */
nccmp_strlist_t* nccmp_get_user_type_compound_field_names_cached(nccmp_user_type_t *type1,
        nccmp_user_type_t *type2, int debug);

nccmp_int_pairs_t* ncccmp_get_user_type_compound_field_index_pairs(nccmp_user_type_t *type1,
        nccmp_user_type_t *type2, int debug);

/* @param type: Must be newly pre-allocated pointer. */
void nccmp_init_user_type(nccmp_user_type_t *type);

void nccmp_init_user_types(nccmp_user_types_t *types);

int nccmp_is_user_type_compound_with_fields(nccmp_user_type_t *type);

int nccmp_is_user_type_field(nccmp_user_type_t *type);

nccmp_user_types_t* nccmp_load_user_types(int ncid, int debug);

void nccmp_load_user_type_compound_tree(int ncid, nccmp_user_types_t *types,
        int *typeids);

/* Recursively compute offsets of a nested field relative to root compound. */
void nccmp_load_user_type_compound_tree_field(int ncid,
        nccmp_user_types_t *types, nccmp_user_type_t *field, int *unique_id);

void nccmp_shallow_copy_user_type(nccmp_user_type_t *to, nccmp_user_type_t *from);

void nccmp_user_type_to_str(nccmp_user_type_t *type, char *result, int indent);

void nccmp_user_types_to_str(nccmp_user_types_t *types, char *result);

#endif
