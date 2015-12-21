#ifndef NCCMP_USER_TYPE_H
#define NCCMP_USER_TYPE_H

#include <netcdf.h>
#include <nccmp_constants.h>
#include <nccmp_darray.h>
#include <nccmp_opt.h>
#include <nccmp_utils.h>
#include <nccmp_strlist.h>

typedef struct nccmp_user_type_t {
    nc_type base_type;                  /* Used by vlen and enum types for their primitive type. */
    int     dim_sizes[NCCMP_MAX_COMPOUND_FIELD_DIMS];
    nccmp_darray_t *fields;
    char    field_index;             	/* Relative to parent compound. */
    int     group_id;              	/* Group that defined the type. */
    int     id;                  	/* Unique instance id of a type in tree defined by nccmp. */
    char   *name;                    	/* Base name like "myField". */
    int     num_dims;
    size_t  num_enums;             	/* Number of enum symbols defined by enum type. */
    size_t  offset;                	/* Byte offset within a compound. */
    struct nccmp_user_type_t *parent;
    size_t  root_size;             	/* Total outermost root compound size. */
    size_t  size;                  	/* Number of bytes. */
    char   *tree_name;             	/* Dotted name from root to child, like "myVar.myField1.myChild2". */
    nc_type type_id;               	/* Defined by file metadata. */
    nc_type user_class;            	/* Class like NC_BYTE or NC_COMPOUND. */
} nccmp_user_type_t;

NCCMP_DARRAY_FIND_FIELD_DECLARE(nccmp_find_user_type_by_type_id, nccmp_user_type_t, type_id, nc_type)
NCCMP_DARRAY_FIND_FIELD_DECLARE(nccmp_find_user_type_by_unique_id, nccmp_user_type_t, unique_id, nc_type)
#define NCCMP_USER_TYPE_ITEM(ARRAY, INDEX) NCCMP_DARRAY_ITEM(nccmp_user_type_t, ARRAY, INDEX)

/* Builds dotted tree name of only child fields and excludes root user-type name.
 * Returns allocated result string. 
 */        
void nccmp_build_user_type_field_path_str(nccmp_user_type_t *type, char *result);

/* Build string with root var name and all field names along tree with dot delimiter.
 * @param result Should be pre-allocated.
 *               Example output: "myVar.field1.field2.leafField".
 */
void nccmp_build_user_type_tree_name(nccmp_user_type_t *type, char **result);

/* Return pairs of unique_ids for field types that match by tree_name in both
   type field arrays.
 */
nccmp_darray_t* ncccmp_create_user_type_field_id_pairs(
        nccmp_user_type_t *parent1,
        nccmp_user_type_t *parent2,
        int debug);

nccmp_user_type_t* nccmp_create_user_type(size_t n);

/* Non recursive free of single item. */
void nccmp_destroy_user_type(nccmp_user_type_t *type);

/* Recursively free all items and container. */
void nccmp_destroy_user_type_array(nccmp_darray_t *types);

/* Return new array with user_types that are leaf nodes to a base_type. Leafs have no children. */
nccmp_darray_t* nccmp_find_user_type_leafs(nccmp_darray_t *types, nccmp_user_type_t *base_type);

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

/* @param type: Must be newly pre-allocated pointer. */
void nccmp_init_user_type(nccmp_user_type_t *type);

/* Returns 1 if type id is in user_type id range above netcdf constant. */
int nccmp_is_user_type_id(int id);

int nccmp_is_user_type_compound_with_fields(nccmp_user_type_t *type);

int nccmp_is_user_type_field(nccmp_user_type_t *type);

nccmp_darray_t* nccmp_load_group_user_type_array(int group_id, int *unique_id, int debug);

/* Loads all user-types in group and subgroups indexed by a unique id. */
nccmp_darray_t* nccmp_load_group_user_type_map(int group_id, int debug);

void nccmp_load_user_type_compound_tree(
		int ncid,
		nccmp_darray_t *types,
        int *typeids,
		int *unique_id);

/* Recursively compute offsets of a nested field relative to root compound. */
void nccmp_load_user_type_compound_tree_field(int ncid,
		nccmp_darray_t *types, nccmp_user_type_t *field, int *unique_id);

void nccmp_set_user_type_map(nccmp_darray_t *map, nccmp_darray_t *types);

void nccmp_shallow_copy_user_type(nccmp_user_type_t *to, nccmp_user_type_t *from);

void nccmp_user_type_to_str(nccmp_user_type_t *type, char *result, int indent);

void nccmp_user_type_array_to_str(nccmp_darray_t *types, char *result);

#endif
