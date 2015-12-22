#include <nccmp_constants.h>
#include <nccmp_error.h>
#include <nccmp_user_type.h>

NCCMP_DARRAY_FIND_FIELD(nccmp_find_user_type_by_type_id, nccmp_user_type_t, type_id, nc_type)
NCCMP_DARRAY_FIND_FIELD(nccmp_find_user_type_by_id, nccmp_user_type_t, id, nc_type)

void nccmp_build_user_type_field_path_str(nccmp_user_type_t *type, char *result)
{
    if (! type) {
        return;
    }
    if (nccmp_is_user_type_field(type)) {
        nccmp_build_user_type_field_path_str(type->parent, result);

        if (type->parent && nccmp_is_user_type_field(type->parent)) {
            strcat(result, ".");
        }

        strcat(result, type->name);
    }
}

void nccmp_build_user_type_tree_name(nccmp_user_type_t *type, char **result)
{
    nccmp_user_type_t *parent = type;
    if (! type) {
        return;
    }

    /* Get root var name. */
    while (NULL != parent->parent) {
        parent = parent->parent;
    }
    strcpy(*result, parent->name);
    /* Only insert dot if root has children. */
    if (parent != type) {
        strcat(*result, ".");
    }

    /* Concat fields tree. */
    nccmp_build_user_type_field_path_str(type, *result);

    /* Optimize string length. */
    *result = realloc(*result, strlen(*result)+1);
    if (NULL == *result) {
        LOG_ERROR("Failed to realloc string.\n");
    }
}


nccmp_user_type_t* nccmp_create_user_type(size_t n)
{
    int i;
    nccmp_user_type_t *result = XMALLOC(nccmp_user_type_t, n);

    for(i = 0; i < n; ++i) {
        nccmp_init_user_type(& result[i]);
    }

    return result;
}

void nccmp_destroy_user_type(nccmp_user_type_t *item)
{
    if (item) {
        nccmp_darray_destroy(item->fields);
        XFREE(item->name);
        XFREE(item->tree_name);
        XFREE(item);
    }
}

void nccmp_destroy_user_type_array(nccmp_darray_t *items)
{
    int i;
    nccmp_user_type_t *item;
    
    if (!items) {
        return;
    }

    for(i = 0; i < items->num_items; ++i) {
        item = NCCMP_USER_TYPE_ITEM(items, i);
        nccmp_destroy_user_type(item);            
    }
    
    nccmp_darray_destroy(items);    
}

nccmp_user_type_t* nccmp_get_user_type_by_id(nccmp_user_type_t* items, int num_items, int id)
{
    int idx = id - NC_FIRSTUSERTYPEID;

    if (items && (idx >= 0) && (idx < num_items)) {
        return & (items[idx]);
    }

    return 0;
}

nccmp_user_type_t* nccmp_get_user_type_by_name(nccmp_user_type_t *types, int ntypes, const char *name)
{
    int i = 0;
    for(; i < ntypes; ++i) {
        if (strcmp(types[i].name, name) == 0) {
            return & types[i];
        }
    }
    return 0;
}

nccmp_strlist_t* nccmp_get_user_type_names(int ncid1, int ncid2,
        int ntypes1, int ntypes2, nc_type* typeids1, nc_type* typeids2, int clazz)
{
    nccmp_strlist_t* result;
    int group, typei, clazz_i, status;
    int ncids[2] = {ncid1, ncid2};
    int ntypes[2] = {ntypes1, ntypes2};
    nc_type typeid, *typeids[2] = {typeids1, typeids2};
    char name[NC_MAX_NAME];

    result = nccmp_new_strlist(NC_MAX_DIMS);

    for(group = 0; group < 2; ++group) {
        for(typei = 0; typei < ntypes[group]; ++typei) {
            typeid = typeids[group][typei];

            if (NC_FIRSTUSERTYPEID > typeid) {
                continue;
            }

            status = nc_inq_user_type(ncids[group],
                        typeid,
                        name,
                        0,
                        0,
                        0,
                        & clazz_i);
            HANDLE_NC_ERROR(status);

            if (clazz_i == clazz) {
                if (!nccmp_exists_in_strlist(result, name)) {
                    nccmp_add_to_strlist(result, name);
                }
            }
        }
    }

    return result;
}

nccmp_strlist_t* nccmp_get_user_type_compound_field_names(int ncid1, int ncid2,
        const char* name, int debug)
{
    nccmp_strlist_t *result;
    int status, typeid, fieldid, group;
    size_t num_fields;
    size_t      ncids[2] = {ncid1, ncid2};
    char fieldname[NC_MAX_NAME];

    result = nccmp_new_strlist(NC_MAX_NAME);
    for(group = 0; group < 2; ++group) {
        status = nc_inq_typeid(ncids[group], name, & typeid);
        if (NC_EBADTYPE == status) {
            continue;
        }
        HANDLE_NC_ERROR(status);

        status = nc_inq_compound(ncids[group],
                    typeid,
                    0 /* name */,
                    0 /* base_size */,
                    & num_fields);
        HANDLE_NC_ERROR(status);

        for(fieldid = 0; fieldid < num_fields; ++fieldid) {
            status = nc_inq_compound_fieldname(ncids[group], typeid, fieldid, fieldname);
            HANDLE_NC_ERROR(status);

            if (!nccmp_exists_in_strlist(result, fieldname)) {
                nccmp_add_to_strlist(result, fieldname);
                if (debug) {
                    LOG_DEBUG("Adding compound type %s field name %s to set.\n",
                                name, fieldname);
                }
            }
        }
    }

    return result;
}

nccmp_strlist_t* nccmp_get_user_type_compound_field_names_cached(
        nccmp_user_type_t *type1, nccmp_user_type_t *type2, int debug)
{
    nccmp_strlist_t *result;
    nccmp_user_type_t *type, *field;
    int group, i;

    result = nccmp_new_strlist(NC_MAX_NAME);
    if (!type1 || !type2) {
        goto recover;
    }

    for(group = 0; group < 2; ++group) {
        switch(group) {
        case 0: type = type1; break;
        case 1: type = type2; break;
        }
        if (type->fields) {
            for(i = 0; i < type->fields->num_items; ++i) {
            	field = (nccmp_user_type_t*) type->fields->items[i];
                if (! nccmp_exists_in_strlist(result, field->name)) {
                    nccmp_add_to_strlist(result, field->name);
                    if (debug) {
                        LOG_DEBUG("Adding compound type %s field name %s to set.\n",
                                    type->name, field->name);
                    }
                }
            }
        }
    }

recover:
    return result;
}

nccmp_darray_t* ncccmp_create_user_type_field_id_pairs(
        nccmp_user_type_t *parent1,
        nccmp_user_type_t *parent2,
        int debug)
{
    int i, j;
    nccmp_pair_int_t *pair = NULL;
    nccmp_darray_t *result = nccmp_darray_create(1);
    nccmp_user_type_t *field1, *field2;

    for(i = 0; i < parent1->fields->num_items; ++i) {
        field1 = NCCMP_USER_TYPE_ITEM(parent1->fields, i);
        for(j = 0; j < parent2->fields->num_items; ++j) {
            field2 = NCCMP_USER_TYPE_ITEM(parent2->fields, j);
            if (strcmp(field1->tree_name, field2->tree_name) == 0) {
                pair = nccmp_create_pair_int(1);
                pair->first = i;
                pair->second = j;
                nccmp_darray_append(result, pair);
                if (debug) {
                    LOG_DEBUG("field id pair tree_name=%s first=%d second=%d\n",
                              field1->tree_name, pair->first, pair->second);
                }
                break;
            }
        }
    }
    
    return result;
}

size_t nccmp_get_num_user_type_fields(nccmp_user_type_t *type)
{
    if (type && type->fields) {
        return type->fields->num_items;
    }
    return 0;
}

int nccmp_is_user_type_compound_with_fields(nccmp_user_type_t *type)
{
    return type && type->fields && (NC_COMPOUND == type->user_class);
}

int nccmp_is_user_type_field(nccmp_user_type_t *type)
{
    return type && type->parent;
}

int nccmp_is_user_type_id(int id)
{
    return id >= NC_FIRSTUSERTYPEID;
}

void nccmp_find_user_type_leafs_recurse(nccmp_darray_t *result, nccmp_darray_t *types)
{
	int i;
	nccmp_user_type_t *type;

	if (result && types) {
		for(i = 0; i < types->num_items; ++i) {
			type = (nccmp_user_type_t*) nccmp_darray_get(types, i);
			if (type) {
				if (type->fields) {
					if (0 == type->fields->num_items) {
						nccmp_darray_append(result, type);
					}
					nccmp_find_user_type_leafs_recurse(result, type->fields);
				}
			}
		}
	}
}

nccmp_darray_t* nccmp_find_user_type_leafs(nccmp_darray_t *types, nccmp_user_type_t *base_type)
{
	nccmp_darray_t *result = nccmp_darray_create(1);

	if (base_type && types) {
		nccmp_find_user_type_leafs_recurse(result, base_type->fields);
	}

	return result;
}

void nccmp_load_group_user_types_recurse(
		int group_id,
		nccmp_darray_t *output,
		int *id,
		int debug)
{
	int status, child_i, num_children, children[NC_MAX_NAME];
	nccmp_darray_t *types;

	/* Load own. */
	types = nccmp_load_group_user_type_array(group_id, id, debug);
	nccmp_darray_extend(output, types);
	nccmp_darray_destroy(types);

	/* Load children. */
	status = nc_inq_grps(group_id, & num_children, children);
    HANDLE_NC_ERROR(status);

    for(child_i = 0; child_i < num_children; ++child_i) {
    	nccmp_load_group_user_types_recurse(children[child_i], output, id, debug);
    }
}

/* Creates new array with each user-type id as index.
   Item references are into the input array, so result should be shallow destroyed.
*/
void nccmp_set_user_type_map(nccmp_darray_t *map, nccmp_darray_t *types)
{
	int type_i;
	nccmp_user_type_t *type;

	for(type_i = 0; type_i < types->num_items; ++type_i) {
		type = (nccmp_user_type_t*) nccmp_darray_get(types, type_i);
		if (type) {
			nccmp_darray_set(map, type->id, type);
			nccmp_set_user_type_map(map, type->fields);
		}
	}
}

nccmp_darray_t* nccmp_load_group_user_type_map(int group_id, int debug)
{
	nccmp_darray_t *map, *types;
	int id = 0;

	types = nccmp_darray_create(1);
	nccmp_load_group_user_types_recurse(group_id, types, & id, debug);

	map = nccmp_darray_create(id);
	nccmp_set_user_type_map(map, types);
	nccmp_darray_destroy(types);

	return map;
}

nccmp_darray_t* nccmp_load_group_user_type_array(int group_id, int *id, int debug)
{
    int status;
    int i, typeid, *typeids = NULL, ntypeids = 0, field_i;
    size_t num_fields = 0;
    nccmp_user_type_t *node = NULL, *field = NULL;
    nccmp_darray_t *results = NULL;
    char *results_str = NULL;

    status = nc_inq_typeids(group_id, & ntypeids, NULL);
    HANDLE_NC_ERROR(status);
    
    if (ntypeids < 1) {
        goto recover;
    }
    
    typeids = XCALLOC(int, ntypeids);

    status = nc_inq_typeids(group_id, NULL, typeids);
    HANDLE_NC_ERROR(status);

    results = nccmp_darray_create(ntypeids);
        
    for(i = 0; i < ntypeids; ++i) {
        typeid = typeids[i];
        node = nccmp_create_user_type(1);
        nccmp_darray_append(results, node);
        node->type_id = typeid;
        node->group_id = group_id;
        status = nc_inq_user_type(group_id,
                      typeid,
                      node->name,
                    & node->size,
                    & node->base_type,
                    & num_fields,
                    & node->user_class);
        HANDLE_NC_ERROR(status);

        if (node->user_class == NC_ENUM) {
            node->num_enums = num_fields;
        }

        if ( (node->user_class == NC_COMPOUND) && num_fields) {
            node->root_size = node->size; /* This node is a compound root. */

            for(field_i = 0; field_i < num_fields; ++field_i) {
            	field = nccmp_create_user_type(1);
                field->group_id = group_id;
                field->field_index = field_i;
                field->parent = node;

                status = nc_inq_compound_field(group_id,
                              typeid,
                              field_i,
                              field->name,
                            & field->offset,
                            & field->type_id,
                            & field->num_dims,
							  field->dim_sizes);
                HANDLE_NC_ERROR(status);

                nccmp_darray_append(node->fields, field);
            }
        }
    }

    nccmp_load_user_type_compound_tree(group_id, results, typeids, id);

    if (debug) {
        results_str = XMALLOC(char, NC_MAX_NAME * NC_MAX_NAME);
        strcpy(results_str, "");
        nccmp_user_type_array_to_str(results, results_str);
        LOG_DEBUG("Loaded %d user-defined types for group_id = %d\n%s",
                ntypeids, group_id, results_str);
        XFREE(results_str);
    }
    
recover:    
    XFREE(typeids);

    return results;
}

void nccmp_load_user_type_compound_tree(int group_id, nccmp_darray_t *types,
        int* typeids, int *id)
{
    int i, j;
    nccmp_user_type_t *type;

    if (!types) {
        return;
    }

    for(i = 0; i < types->num_items; ++i) {
        type = (nccmp_user_type_t*) nccmp_find_user_type_by_type_id(types, typeids[i]);

        if (type) {
			type->id = (*id)++;
        	if (!nccmp_darray_empty(type->fields) && (NC_COMPOUND == type->user_class)) {
				for(j = 0; j < type->fields->num_items; ++j) {
					nccmp_load_user_type_compound_tree_field(
							group_id,
							types,
							(nccmp_user_type_t*) nccmp_darray_get(type->fields, j),
							id);
				}
        	}
        }

        nccmp_build_user_type_tree_name(type, & type->tree_name);
    }
}

void nccmp_load_user_type_compound_tree_field(int group_id,
		nccmp_darray_t *types, nccmp_user_type_t* field, int *id)
{
    int i, status;
    nccmp_user_type_t *type, *child;

    if (!field) {
        return;
    }

    field->offset += field->parent->offset;
    field->root_size = field->parent->root_size;
    field->id = (*id)++;

    switch(field->type_id) {
    case NC_BYTE:
    case NC_UBYTE:
    case NC_CHAR:   field->size = 1; break;
    case NC_SHORT:
    case NC_USHORT: field->size = 2; break;
    case NC_FLOAT:
    case NC_INT:
    case NC_UINT:   field->size = 4; break;
    case NC_INT64:
    case NC_UINT64:
    case NC_DOUBLE: field->size = 8; break;
    case NC_STRING: field->size = 0; break;
    case NC_COMPOUND:
    case NC_ENUM:
    case NC_OPAQUE:
    case NC_VLEN:
    default:
        status = nc_inq_user_type(group_id, field->type_id, 0 /*name*/, & field->size,
                                  & field->base_type, 0 /*nfields*/, & field->user_class);
        HANDLE_NC_ERROR(status);
        break;
    }

    if (NC_COMPOUND == field->user_class) {
        type = nccmp_find_user_type_by_type_id(types, field->type_id);
        if (type && !nccmp_darray_empty(type->fields) && nccmp_darray_empty(field->fields)) {
            // Populate dependencies according to the field's type definition.
        	nccmp_darray_reserve(field->fields, type->fields->num_items);
            for(i = 0; i < type->fields->num_items; ++i) {
            	child = nccmp_create_user_type(1);
            	nccmp_darray_append(field->fields, child);
                nccmp_shallow_copy_user_type(child, (nccmp_user_type_t*) type->fields->items[i]);
                child->parent = field;

                nccmp_load_user_type_compound_tree_field(
                        group_id,
                        types,
                        child,
                        id);
            }
        }
    }

    nccmp_build_user_type_tree_name(field, & field->tree_name);
}

void nccmp_init_user_type(nccmp_user_type_t *item)
{
    if (!item) {
        return;
    }

    item->name = XCALLOC(char, NC_MAX_NAME);
    if (! item->name) {
        LOG_ERROR("Failed to allocate user type name.\n");
        exit(EXIT_FATAL);
    }

    item->tree_name = XCALLOC(char, NC_MAX_NAME);
    if (! item->tree_name) {
        LOG_ERROR("Failed to allocate user type tree_name.\n");
        exit(EXIT_FATAL);
    }

    memset(item->dim_sizes, 0, sizeof(item->dim_sizes));

    item->base_type = 0;
    item->fields = nccmp_darray_create(1);
    item->field_index = 0;
    item->id = 0;
    item->num_dims = 0;
    item->num_enums = 0;
    item->offset = 0;
    item->parent = 0;
    item->root_size = 0;
    item->size = 0;
    item->type_id = 0;
    item->user_class = 0;
}

void nccmp_shallow_copy_user_type(nccmp_user_type_t *to, nccmp_user_type_t *from)
{
    memcpy(to->dim_sizes, from->dim_sizes, sizeof(from->dim_sizes));

    if (from->name) {
        strcpy(to->name,  from->name);
    }

    if (from->tree_name) {
        strcpy(to->tree_name, from->tree_name);
    }

    to->base_type   = from->base_type;
    to->field_index = from->field_index;
    to->group_id    = from->group_id;
    to->id          = from->id;
    to->num_dims    = from->num_dims;
    to->num_enums   = from->num_enums;
    to->offset      = from->offset;
    to->root_size   = from->root_size;
    to->size        = from->size;
    to->type_id     = from->type_id;
    to->user_class  = from->user_class;
}

void nccmp_user_type_to_str(nccmp_user_type_t* type, char *result, int num_indent)
{
    int i;
    char tmp[NC_MAX_NAME];

    if (! type) {
        return;
    }

    for(i = 0; i < num_indent; ++i) {
        strcat(result, "    ");
    }

    sprintf(tmp, "base_type=%d class=%d field_index=%d is_field=%d name=%s"
               " tree_name=%s num_dims=%d num_enums=%zu num_fields=%zu offset=%zu"
               " size=%zu type_id=%d id=%d\n",
               type->base_type,
               type->user_class,
               type->field_index,
               nccmp_is_user_type_field(type),
               type->name,
               type->tree_name,
               type->num_dims,
               type->num_enums,
               nccmp_get_num_user_type_fields(type),
               type->offset,
               type->size,
               type->type_id,
               type->id);
     strcat(result, tmp);

     if (type->fields) {
         for(i = 0; i < type->fields->num_items; ++i) {
             nccmp_user_type_to_str( (nccmp_user_type_t*) type->fields->items[i], result, num_indent + 1);
         }
     }
}

void nccmp_user_type_array_to_str(nccmp_darray_t *types, char *result)
{
    int i;

    if (!types) {
        return;
    }

    for(i = 0; i < types->num_items; ++i) {
        nccmp_user_type_to_str( NCCMP_USER_TYPE_ITEM(types, i), result, 0);
    }
}
