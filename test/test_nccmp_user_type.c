#include <nccmp_error.h>
#include <nccmp_user_type.h>
#include <test_common.h>

int failures = 0;

int test_nccmp_build_user_type_field_path_str()
{
    int result = PASS;
    int status, ncid;
    nccmp_user_types_t *types;
    nccmp_user_type_t *type;
    char str[NC_MAX_NAME];
    char expected1[] = "t3";
    char expected2[] = "com2.com1.t1";

    status = nc_open("make_compound_nest_atomic1.nc", NC_NOWRITE, & ncid);
    HANDLE_NC_ERROR(status);

    types = nccmp_load_user_types(ncid, 0 /*debug*/);

    type = & types->items[2];
    strcpy(str, "");
    nccmp_build_user_type_field_path_str(type, str);

    if (strcmp("", str)) {
        PRINT("Unexpected string %s\n", str);
        result = FAIL;
        goto recover;
    }

    type = & types->items[2].fields->items[12];
    strcpy(str, "");
    nccmp_build_user_type_field_path_str(type, str);

    if (strcmp(expected1, str)) {
        PRINT("Unexpected string %s <> %s\n", expected1, str);
        result = FAIL;
        goto recover;
    }

    type = & types->items[2] // comp3
                    .fields->items[0] // com2
                    .fields->items[0] // com1
                    .fields->items[11]; // t1
    strcpy(str, "");
    nccmp_build_user_type_field_path_str(type, str);

    if (strcmp(expected2, str)) {
        PRINT("Unexpected string %s <> %s\n", expected2, str);
        result = FAIL;
        goto recover;
    }

recover:
    nc_close(ncid);
    nccmp_destroy_user_types(types);

    return result;
}

int test_nccmp_get_user_type_by_id()
{
    int result = PASS;
    const size_t n = 16;
    nccmp_user_types_t *types = nccmp_create_user_types(n);
    nccmp_user_type_t *type;
    int id;

    for(id = 0; id < n; ++id) {
        types->items[id].type_id = NC_FIRSTUSERTYPEID + id;
    }

    for(id = -1; id < NC_FIRSTUSERTYPEID; ++id) {
        type = nccmp_get_user_type_by_id(types->items, n, id);

        if (type) {
            PRINT("Unexpected type=%p id=%d\n", type, type->type_id);
            result = FAIL;
            goto recover;
        }
    }

    for(id = NC_FIRSTUSERTYPEID; id < (NC_FIRSTUSERTYPEID + n); ++id) {
        type = nccmp_get_user_type_by_id(types->items, n, id);

        if (!type) {
            PRINT("Expected user type for id=%d\n", id);
            result = FAIL;
            goto recover;
        }
    }

    id = NC_FIRSTUSERTYPEID + n + 1;
    type = nccmp_get_user_type_by_id(types->items, n, id);
    if (type) {
        PRINT("Unexpected type=%p id=%d\n", type, type->type_id);
        result = FAIL;
        goto recover;
    }


recover:
    nccmp_destroy_user_types(types);

    return result;
}

int test_nccmp_get_user_type_names()
{
    int result = PASS;
    int status, ncid1, ncid2, n1, n2, ids1[4], ids2[4];
    nccmp_strlist_t *names;
    int expected = 3;

    status = nc_open("make_compound_nest_atomic1.nc", NC_NOWRITE, & ncid1);
    HANDLE_NC_ERROR(status);
    status = nc_open("make_compound_nest_atomic2.nc", NC_NOWRITE, & ncid2);
    HANDLE_NC_ERROR(status);

    status = nc_inq_typeids(ncid1, & n1, ids1);
    HANDLE_NC_ERROR(status);
    status = nc_inq_typeids(ncid2, & n2, ids2);
    HANDLE_NC_ERROR(status);

    names = nccmp_get_user_type_names(ncid1, ncid2, n1, n2, ids1,
            ids2, NC_COMPOUND);

    if (!names) {
        PRINT("Failed to create names list.\n");
        result = FAIL;
        goto recover;
    }

    if (names->size != expected) {
        PRINT("Expected number of compound types %d <> %d\n", expected, names->size);
        result = FAIL;
        goto recover;
    }

    if (strcmp("comp3", names->items[0]) &&
        strcmp("comp3", names->items[1]) &&
        strcmp("comp3", names->items[2])
        ) {
        PRINT("Failed to find comp3.\n");
        result = FAIL;
        goto recover;
    }

recover:
    nc_close(ncid1);
    nc_close(ncid2);
    nccmp_free_strlist(& names);

    return result;
}

int test_nccmp_get_user_type_compound_field_names()
{
    int result = PASS;
    int status, ncid1, ncid2;
    nccmp_strlist_t *names;
    int i, expected = 13;

    status = nc_open("make_compound_nest_atomic1.nc", NC_NOWRITE, & ncid1);
    HANDLE_NC_ERROR(status);
    status = nc_open("make_compound_nest_atomic2.nc", NC_NOWRITE, & ncid2);
    HANDLE_NC_ERROR(status);

    names = nccmp_get_user_type_compound_field_names(ncid1, ncid2, "comp3", 0);

    if (!names) {
        PRINT("Failed to create names list.\n");
        result = FAIL;
        goto recover;
    }

    if (names->size != expected) {
        PRINT("Expected number of names %d <> %d\n", expected, names->size);
        result = FAIL;
        goto recover;
    }

    for(i = 0; i < expected; ++i) {
        if (strcmp("t3", names->items[i])) {
            break;
        }
    }

    if (i >= expected) {
        PRINT("Failed to find t3.");
        result = FAIL;
        goto recover;
    }

recover:
    nc_close(ncid1);
    nc_close(ncid2);
    nccmp_free_strlist(& names);

    return result;
}

int test_nccmp_get_user_type_compound_field_names_cached()
{
    int result = PASS;
    int status, ncid1, ncid2;
    nccmp_user_types_t *types1, *types2;
    int expected = 13;
    nccmp_strlist_t *names;

    status = nc_open("make_compound_nest_atomic1.nc", NC_NOWRITE, & ncid1);
    HANDLE_NC_ERROR(status);
    status = nc_open("make_compound_nest_atomic2.nc", NC_NOWRITE, & ncid2);
    HANDLE_NC_ERROR(status);

    types1 = nccmp_load_user_types(ncid1, 0);
    types2 = nccmp_load_user_types(ncid2, 0);

    names = nccmp_get_user_type_compound_field_names_cached(
            & types1->items[2], & types2->items[2], 0);

    if (expected != names->size) {
        PRINT("Unexpected num names %d <> %d\n", expected, names->size);
        result = FAIL;
        goto recover;
    }

    if (strcmp("t3", names->items[expected-1])) {
        PRINT("Unexpected name t3 <> %s\n", names->items[expected-1]);
        result = FAIL;
        goto recover;
    }

recover:
    nc_close(ncid1);
    nc_close(ncid2);
    nccmp_destroy_user_types(types1);
    nccmp_destroy_user_types(types2);
    nccmp_free_strlist(& names);

    return result;
}

int test_ncccmp_get_user_type_compound_field_index_pairs()
{
    int result = PASS;
    int status, ncid1, ncid2;
    nccmp_int_pairs_t *pairs;
    nccmp_user_types_t *types1, *types2;
    int i, expected = 13;

    status = nc_open("make_compound_nest_atomic1.nc", NC_NOWRITE, & ncid1);
    HANDLE_NC_ERROR(status);
    status = nc_open("make_compound_nest_atomic2.nc", NC_NOWRITE, & ncid2);
    HANDLE_NC_ERROR(status);

    types1 = nccmp_load_user_types(ncid1, 0);
    types2 = nccmp_load_user_types(ncid2, 0);

    pairs = ncccmp_get_user_type_compound_field_index_pairs(& types1->items[2],
            & types2->items[2], 0);

    if (expected != pairs->size) {
        PRINT("Unexpected num pairs %d <> %d\n", expected, pairs->size);
        result = FAIL;
        goto recover;
    }

    for(i = 0; i < pairs->size; ++i) {
        //printf("%d %d\n", pairs->pairs[i].first, pairs->pairs[i].second);
        if (pairs->items[i].first != pairs->items[i].second) {
            PRINT("Unexpected pairs %d <> %d\n", pairs->items[i].first, pairs->items[i].second);
            result = FAIL;
            goto recover;
        }
    }

recover:
    nc_close(ncid1);
    nc_close(ncid2);
    nccmp_destroy_user_types(types1);
    nccmp_destroy_user_types(types2);
    nccmp_destroy_int_pairs(pairs);

    return result;
}

int test_nccmp_load_user_types()
{
    int result = PASS;
    int status, ncid;
    char *name;
    nccmp_user_types_t *types;

    status = nc_open("make_compound_nest_atomic1.nc", NC_NOWRITE, & ncid);
    HANDLE_NC_ERROR(status);

    types = nccmp_load_user_types(ncid, 0);

    if (!types) {
        PRINT("Unexpected null types.");
        result = FAIL;
        goto recover;
    }

    if (types->num_items != 3) {
        PRINT("Unexpected num items 3 <> %d\n", types->num_items);
        result = FAIL;
        goto recover;
    }

#define CMP(NAME, INDEX) \
    if (strcmp(NAME, types->items[INDEX].name)) { \
        PRINT("Unexpected type name " #NAME " <> %s\n", types->items[INDEX].name); \
        result = FAIL; \
        goto recover; \
    }

    CMP("comp1", 0);
    CMP("comp2", 1);
    CMP("comp3", 2);
#undef CMP

    name = types->items[2].fields->items[0].fields->items[0].fields->items[11].name;
    if (strcmp("t1", name)) {
        PRINT("Unexpected field name t1 <> %s\n", name);
        result = FAIL;
        goto recover;
    }

recover:
    nc_close(ncid);
    nccmp_destroy_user_types(types);

    return result;
}

int test_nccmp_shallow_copy_user_type()
{
    int result = PASS;
    nccmp_user_type_t to, from;

    from.name = malloc(32);
    to.name = malloc(32);

    from.base_type = 1;
    from.dim_sizes[0] = 1;
    from.dim_sizes[1] = 2;
    from.dim_sizes[2] = 3;
    from.dim_sizes[3] = 4;
    from.field_index = 5;
    strcpy(from.name, "test");
    from.num_dims = 4;
    from.num_enums = 6;
    from.offset = 100;
    from.root_size = 200;
    from.size = 123;
    from.type_id = 44;
    from.user_class = NC_ENUM;

    nccmp_shallow_copy_user_type(& to, & from);

#define CMP(V) \
    if (to.V != from.V) { \
        PRINT("Unexpected " #V " %d <> %d\n", to.V, from.V); \
        result = FAIL; \
        goto recover; \
    }

    CMP(base_type);
    CMP(dim_sizes[0]);
    CMP(dim_sizes[1]);
    CMP(dim_sizes[2]);
    CMP(dim_sizes[3]);
    CMP(field_index);
    CMP(num_dims);
    CMP(num_enums);
    CMP(offset);
    CMP(root_size);
    CMP(size);
    CMP(type_id);
    CMP(user_class);

#undef CMP

    if (strcmp(to.name, from.name)) {
        PRINT("Unexpected name %s <> %s\n", to.name, from.name);
        result = FAIL;
        goto recover;
    }

recover:
    free(from.name);
    free(to.name);

    return result;
}

int test_nccmp_print_user_types()
{
    int result = PASS;
    int status, ncid;
    char str[NC_MAX_NAME * NC_MAX_NAME];
    nccmp_user_types_t *types;

    status = nc_open("make_compound_nest_atomic1.nc", NC_NOWRITE, & ncid);
    HANDLE_NC_ERROR(status);

    types = nccmp_load_user_types(ncid, 0);

    strcpy(str, "");
    nccmp_user_types_to_str(types, str);

    nc_close(ncid);
    nccmp_destroy_user_types(types);

    return result;
}

int test_new_user_types()
{
    int result = PASS;
    const int n = 8192;
    nccmp_user_types_t *types = nccmp_create_user_types(n);

    if (types->num_items != n) {
        PRINT("Expected num_items %d <> %d\n", types->num_items, n);
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_destroy_user_types(types);

    return result;
}

int main(int argc, char* argv[])
{
    TEST(test_new_user_types);
    TEST(test_nccmp_build_user_type_field_path_str);
    TEST(test_nccmp_get_user_type_by_id);
    TEST(test_nccmp_get_user_type_names);
    TEST(test_nccmp_get_user_type_compound_field_names);
    TEST(test_nccmp_get_user_type_compound_field_names_cached);
    TEST(test_ncccmp_get_user_type_compound_field_index_pairs);
    TEST(test_nccmp_load_user_types);
    TEST(test_nccmp_shallow_copy_user_type);
    TEST(test_nccmp_print_user_types);

    return failures;
}
