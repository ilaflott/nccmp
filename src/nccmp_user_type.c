#include <nccmp_constants.h>
#include <nccmp_error.h>
#include <nccmp_user_type.h>

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

nccmp_user_type_hashmap_t* nccmp_create_field_hashmap(nccmp_user_types_t *types)
{
    nccmp_user_type_hashmap_t *result = XCALLOC(nccmp_user_type_hashmap_t, 1);
    result->items = XCALLOC(nccmp_user_type_t*, nccmp_find_max_user_type_unique_id(types) + 1);
    nccmp_set_field_hashmap(result, types);
    return result;
}

nccmp_user_type_t* nccmp_create_user_type(size_t n)
{
    int i;
    nccmp_user_type_t *result = XMALLOC(nccmp_user_type_t, n);
    memset(result, 0, sizeof(nccmp_user_type_t) * n);

    for(i = 0; i < n; ++i) {
        nccmp_init_user_type(& result[i]);
    }

    return result;
}

nccmp_user_types_t* nccmp_create_user_types(size_t n)
{
    nccmp_user_types_t *result = XMALLOC(nccmp_user_types_t, 1);

    if (!result) {
        LOG_ERROR("Failed to allocate user types.\n");
        exit(EXIT_FATAL);
    }

    if (n > 0) {
        result->items = XMALLOC(nccmp_user_type_t, n);
        if (!result->items) {
                LOG_ERROR("Failed to allocate user type items.\n");
                exit(EXIT_FATAL);
            }
    } else {
        result->items = NULL;
    }

    result->num_items = n > 0 ? n : 0;

    nccmp_init_user_types(result);

    return result;
}

void nccmp_destroy_user_type(nccmp_user_type_t *node)
{
    nccmp_destroy_user_type_members(node);
    XFREE(node);
}

void nccmp_destroy_user_type_members(nccmp_user_type_t *item)
{
    if (!item) {
        return;
    }

    nccmp_destroy_user_types(item->fields);
    XFREE(item->name);
    XFREE(item->tree_name);
}

void nccmp_destroy_user_types(nccmp_user_types_t *types)
{
    int i = 0;

    if (! types) {
        return;
    }

    for(; i < types->num_items; ++i) {
        nccmp_destroy_user_type_members(& types->items[i]);
    }

    XFREE(types->items);
    XFREE(types);
}

void nccmp_destroy_field_hashmap(nccmp_user_type_hashmap_t *map)
{
    if (map) {
        XFREE(map->items);
    }
    XFREE(map);
}

int nccmp_find_max_user_type_unique_id(nccmp_user_types_t* types)
{
    int i, result = -1;
    nccmp_user_type_t *item;

    if (types) {
        for(i = 0; i < types->num_items; ++i) {
            item = & types->items[i];
            result = nccmp_max_int(item->unique_id, result);

            if (nccmp_is_user_type_compound_with_fields(item)) {
                result = nccmp_max_int(result,
                                       nccmp_find_max_user_type_unique_id(item->fields));
            }
        }
    }

    return result;
}

nccmp_user_type_t* nccmp_find_field_by_unique_id(nccmp_user_types_t* types, int id)
{
    int i;
    nccmp_user_type_t *result = NULL;
    nccmp_user_type_t *item;

    if (types) {
        for(i = 0; i < types->num_items; ++i) {
            item = & types->items[i];
            if (id == item->unique_id) {
                result = item;
                break;
            }
            if (nccmp_is_user_type_compound_with_fields(item)) {
                result = nccmp_find_field_by_unique_id(item->fields, id);
                if (result) {
                    break;
                }
            }
        }
    }

    return result;
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
    nccmp_user_type_t *type;
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
                if (! nccmp_exists_in_strlist(result, type->fields->items[i].name)) {
                    nccmp_add_to_strlist(result, type->fields->items[i].name);
                    if (debug) {
                        LOG_DEBUG("Adding compound type %s field name %s to set.\n",
                                    type->name, type->fields->items[i].name);
                    }
                }
            }
        }
    }

recover:
    return result;
}

nccmp_int_pairs_t* ncccmp_get_user_type_compound_field_index_pairs(nccmp_user_type_t *type1,
        nccmp_user_type_t *type2, int debug)
{
    int i, j, k;
    nccmp_strlist_t *field_names;
    nccmp_int_pairs_t *result = NULL;
    int items[NC_MAX_VARS][2];
    int count = 0;

    field_names = nccmp_get_user_type_compound_field_names_cached(type1, type2, debug);

    for(i = 0; i < field_names->size; ++i) {
        for(j = 0; type1->fields && j < type1->fields->num_items; ++j) {
            if (strcmp(type1->fields->items[j].name, field_names->items[i]) == 0) {
                for(k = 0; type2->fields && k < type2->fields->num_items; ++k) {
                    if (strcmp(type2->fields->items[k].name, field_names->items[i]) == 0) {
                        items[count][0] = j;
                        items[count][1] = k;
                        ++count;
                        break;
                    }
                }
                break;
            }
        }
    }

    nccmp_free_strlist(& field_names);

    result = nccmp_create_int_pairs(count);
    for(i = 0; i < count; ++i) {
        result->items[i].first = items[i][0];
        result->items[i].second = items[i][1];
    }

    if (debug) {
        for(i = 0; i < result->size; ++i) {
            LOG_DEBUG("field_id pair %d first=%d second=%d\n",
                    i, result->items[i].first, result->items[i].second);
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

nccmp_user_types_t* nccmp_load_group_user_types(int ncid, int debug)
{
    int status;
    int i, typeid, *typeids = NULL, ntypeids = 0, fieldid;
    size_t num_fields = 0;
    nccmp_user_type_t *node = NULL;
    nccmp_user_types_t* results = NULL;
    char *results_str = NULL;

    status = nc_inq_typeids(ncid, & ntypeids, NULL);
    HANDLE_NC_ERROR(status);
    
    if (ntypeids < 1) {
        goto recover;
    }
    
    typeids = XCALLOC(int, ntypeids);

    status = nc_inq_typeids(ncid, NULL, typeids);
    HANDLE_NC_ERROR(status);

    results = nccmp_create_user_types(ntypeids);
        
    for(i = 0; i < ntypeids; ++i) {
        typeid = typeids[i];
        node = nccmp_get_user_type_by_id(results->items, results->num_items, typeid);
        node->type_id = typeid;
        status = nc_inq_user_type(ncid,
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

            node->fields = nccmp_create_user_types(num_fields);
            for(fieldid = 0; fieldid < num_fields; ++fieldid) {
                status = nc_inq_compound_field(ncid,
                              typeid,
                              fieldid,
                              node->fields->items[fieldid].name,
                            & node->fields->items[fieldid].offset,
                            & node->fields->items[fieldid].type_id,
                            & node->fields->items[fieldid].num_dims,
                              node->fields->items[fieldid].dim_sizes);
                HANDLE_NC_ERROR(status);

                node->fields->items[fieldid].field_index = fieldid;
                node->fields->items[fieldid].parent = node;
            }
        }
    }

    nccmp_load_user_type_compound_tree(ncid, results, typeids);

    if (debug) {
        results_str = XMALLOC(char, NC_MAX_NAME * NC_MAX_NAME);
        strcpy(results_str, "");
        nccmp_user_types_to_str(results, results_str);
        LOG_DEBUG("Loaded %d user-defined types for ncid = %d\n%s",
                ntypeids, ncid, results_str);
        XFREE(results_str);
    }
    
recover:    
    XFREE(typeids);

    return results;
}

void nccmp_load_user_type_compound_tree(int ncid, nccmp_user_types_t* types,
        int* typeids)
{
    int i, j, id;
    nccmp_user_type_t *type;
    int unique_id = 0;

    if (!types) {
        return;
    }

    for(i = 0; i < types->num_items; ++i) {
        id = typeids[i];
        type = nccmp_get_user_type_by_id(types->items, types->num_items, id);
        type->unique_id = unique_id++;

        if (type && type->fields && (NC_COMPOUND == type->user_class)) {
            for(j = 0; j < type->fields->num_items; ++j) {
                nccmp_load_user_type_compound_tree_field(ncid, types,
                        & type->fields->items[j], & unique_id);
            }
        }

        nccmp_build_user_type_tree_name(type, & type->tree_name);
    }
}

void nccmp_load_user_type_compound_tree_field(int ncid,
        nccmp_user_types_t* types, nccmp_user_type_t* field, int *unique_id)
{
    int i, status;
    nccmp_user_type_t *type_info = 0, *child = 0;

    if (!field) {
        return;
    }

    field->offset += field->parent->offset;
    field->root_size = field->parent->root_size;
    field->unique_id = (*unique_id)++;

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
        status = nc_inq_user_type(ncid, field->type_id, 0 /*name*/, & field->size,
                                  & field->base_type, 0 /*nfields*/, & field->user_class);
        HANDLE_NC_ERROR(status);
        break;
    }

    if (NC_COMPOUND == field->user_class) {
        type_info = nccmp_get_user_type_by_id(types->items, types->num_items, field->type_id);
        if (type_info->fields && !field->fields) {
            // Populate dependencies according to the field's type definition.
            field->fields = nccmp_create_user_types(type_info->fields->num_items);

            for(i = 0; i < field->fields->num_items; ++i) {
                child = & field->fields->items[i];
                nccmp_shallow_copy_user_type(child, & type_info->fields->items[i]);
                child->parent = field;

                nccmp_load_user_type_compound_tree_field(
                        ncid,
                        types,
                        child,
                        unique_id);
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
        LOG_ERROR("Failed to allocate user type dotname.\n");
        exit(EXIT_FATAL);
    }

    item->base_type = 0;

    memset(item->dim_sizes, 0, sizeof(item->dim_sizes));

    item->fields = 0;
    item->field_index = 0;
    item->num_dims = 0;
    item->num_enums = 0;
    item->offset = 0;
    item->parent = 0;
    item->root_size = 0;
    item->size = 0;
    item->type_id = 0;
    item->user_class = 0;
}

void nccmp_init_user_types(nccmp_user_types_t* types)
{
    int i;

    if (!types) {
        return;
    }

    for(i = 0; i < types->num_items; ++i) {
        nccmp_init_user_type(& types->items[i]);
    }
}

void nccmp_print_field_hashmap(nccmp_user_type_hashmap_t *map)
{
    int i;
    if (!map) {
        LOG_DEBUG("map=NULL\n");
    } else {
        LOG_DEBUG("map->num_items=%d\n", map->num_items);
        for(i = 0; i < map->num_items; ++i) {
            LOG_DEBUG("i=%d unique_id=%d tree_name=%s\n",
                      i, map->items[i]->unique_id, map->items[i]->tree_name);
        }
    }
}

void nccmp_set_field_hashmap(nccmp_user_type_hashmap_t *result, nccmp_user_types_t *types)
{
    result->num_items = 0;
    nccmp_set_field_hashmap_traverse(result, types);
    /* Shrink memory usage after populating. */
    result->items = realloc(result->items, result->num_items * sizeof(nccmp_user_type_t*));
}

void nccmp_set_field_hashmap_traverse(nccmp_user_type_hashmap_t *result, nccmp_user_types_t *types)
{
    int i;
    nccmp_user_type_t *item;

    if (types) {
        for(i = 0; i < types->num_items; ++i) {
            item = & types->items[i];
            result->items[item->unique_id] = item;
            ++result->num_items;

            if (nccmp_is_user_type_compound_with_fields(item)) {
                nccmp_set_field_hashmap_traverse(result, item->fields);
            }
        }
    }
}

void nccmp_shallow_copy_user_type(nccmp_user_type_t *to, nccmp_user_type_t *from)
{
    to->base_type   = from->base_type;
    memcpy(to->dim_sizes, from->dim_sizes, sizeof(from->dim_sizes));
    to->field_index = from->field_index;
    if (from->name) {
        strcpy(to->name,  from->name);
    }
    if (from->tree_name) {
        strcpy(to->tree_name, from->tree_name);
    }
    to->num_dims    = from->num_dims;
    to->num_enums   = from->num_enums;
    to->offset      = from->offset;
    to->root_size   = from->root_size;
    to->size        = from->size;
    to->type_id     = from->type_id;
    to->unique_id   = from->unique_id;
    to->user_class  = from->user_class;
}

void nccmp_user_type_to_str(nccmp_user_type_t* type, char *result, int num_indent)
{
    int i;
    char tmpstr[256];

    if (! type) {
        return;
    }

    for(i = 0; i < num_indent; ++i) {
        strcat(result, "    ");
    }

    sprintf(tmpstr, "base_type=%d class=%d field_index=%d is_field=%d name=%s"
               " tree_name=%s num_dims=%d num_enums=%zu num_fields=%zu offset=%zu"
               " size=%zu type_id=%d unique_id=%d\n",
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
               type->unique_id);
     strcat(result, tmpstr);

     if (type->fields) {
         for(i = 0; i < type->fields->num_items; ++i) {
             nccmp_user_type_to_str(& type->fields->items[i], result, num_indent + 1);
         }
     }
}

void nccmp_user_types_to_str(nccmp_user_types_t* types, char *result)
{
    int i;

    if (!types) {
        return;
    }

    for(i = 0; i < types->num_items; ++i) {
        nccmp_user_type_to_str(& types->items[i], result, 0);
    }
}
