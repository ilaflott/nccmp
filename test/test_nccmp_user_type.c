
#include <stddef.h>

#include <nccmp_error.h>
#include <nccmp_user_type.h>
#include <test_common.h>

int failures = 0;

const char *file1 = "make_compound_nest_atomic1.user_type.nc";
const char *file2 = "make_compound_nest_atomic2.user_type.nc";

int test_nccmp_build_user_type_field_path_str()
{
    int result = PASS;
    int status, ncid;
    nccmp_darray_t *types;
    nccmp_user_type_t *type;
    char str[NC_MAX_NAME];
    char expected[256];
    status = nc_open(file1, NC_NOWRITE, & ncid);
    if (status) {
        PRINT("Failed to open %s\n", file1);
    }
    HANDLE_NC_ERROR(status);

    types = nccmp_load_group_user_type_map(ncid, 0 /*debug*/, 0);

    type = NCCMP_USER_TYPE_ITEM(types, 0);
    strcpy(str, "");
    nccmp_build_user_type_field_path_str(type, str);
    strcpy(expected, "");
    if (strcmp(expected, str)) {
        PRINT("Unexpected string %s\n", str);
        result = FAIL;
        goto recover;
    }

    type = NCCMP_USER_TYPE_ITEM(NCCMP_USER_TYPE_ITEM(types, 0)->fields, 11);
    strcpy(str, "");
    nccmp_build_user_type_field_path_str(type, str);
    strcpy(expected, "t1");
    if (strcmp(expected, str)) {
        PRINT("Unexpected %s <> %s\n", expected, str);
        result = FAIL;
        goto recover;
    }

    type = (nccmp_user_type_t*)
                ((nccmp_user_type_t*)
                    ((nccmp_user_type_t*)
                        (NCCMP_USER_TYPE_ITEM(types, 39))   // comp3
                            ->fields->items[0])             // com2
                                ->fields->items[0])         // com1
                                    ->fields->items[11];    // t1
    strcpy(str, "");
    nccmp_build_user_type_field_path_str(type, str);
    strcpy(expected, "com1.t1");
    if (strcmp(expected, str)) {
        PRINT("Unexpected %s <> %s\n", expected, str);
        result = FAIL;
        goto recover;
    }

recover:
    nc_close(ncid);
    nccmp_destroy_user_type_array(types);

    return result;
}

int test_nccmp_get_user_type_names()
{
    int result = PASS;
    int status, ncid1, ncid2, n1, n2, ids1[4], ids2[4];
    nccmp_strlist_t *names;
    int expected = 3;

    status = nc_open(file1, NC_NOWRITE, & ncid1);
    HANDLE_NC_ERROR(status);
    status = nc_open(file2, NC_NOWRITE, & ncid2);
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

    status = nc_open(file1, NC_NOWRITE, & ncid1);
    HANDLE_NC_ERROR(status);
    status = nc_open(file2, NC_NOWRITE, & ncid2);
    HANDLE_NC_ERROR(status);

    names = nccmp_get_user_type_compound_field_names(ncid1, ncid2, "comp3", 0, 0);

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

int test_ncccmp_create_user_type_field_id_pairs()
{
    int result = PASS;
    int status, ncid1, ncid2;
    nccmp_darray_t *pairs;
    nccmp_darray_t *types1, *types2;
    int i, expected = 13;
    int first, second;

    status = nc_open(file1, NC_NOWRITE, & ncid1);
    HANDLE_NC_ERROR(status);
    status = nc_open(file2, NC_NOWRITE, & ncid2);
    HANDLE_NC_ERROR(status);

    types1 = nccmp_load_group_user_type_map(ncid1, 0, 0);
    types2 = nccmp_load_group_user_type_map(ncid2, 0, 0);

    pairs = ncccmp_create_user_type_field_id_pairs(NCCMP_USER_TYPE_ITEM(types1, 13), // comp2
            NCCMP_USER_TYPE_ITEM(types2, 13), 
            0, 0);

    if (expected != pairs->num_items) {
        PRINT("Unexpected number of pairs %d <> %d\n", expected, pairs->num_items);
        result = FAIL;
        goto recover;
    }

    for(i = 0; i < pairs->num_items; ++i) {
        first = ((nccmp_pair_int_t*) pairs->items[i])->first;
        second = ((nccmp_pair_int_t*) pairs->items[i])->second;
        if (first != second) {
            PRINT("Unexpected pairs %d <> %d\n", first, second);
            result = FAIL;
            goto recover;
        }
    }

recover:
    nc_close(ncid1);
    nc_close(ncid2);
    nccmp_destroy_user_type_array(types1);
    nccmp_destroy_user_type_array(types2);
    nccmp_darray_destroy_deep(pairs);

    return result;
}

int test_nccmp_load_user_types()
{
    int result = PASS;
    int status, ncid;
    char *name;
    nccmp_darray_t *types;
    int expect; 
    nccmp_user_type_t *t1, *t2, *t3, *t4;
    
    status = nc_open(file1, NC_NOWRITE, & ncid);
    HANDLE_NC_ERROR(status);

    types = nccmp_load_group_user_type_map(ncid, 0, 0);

    if (!types) {
        PRINT("Unexpected null types.");
        result = FAIL;
        goto recover;
    }

    expect = 66;
    if (expect != types->num_items) {
        PRINT("Unexpected %d <> %d\n", expect, types->num_items);
        result = FAIL;
        goto recover;
    }

#define CMP(NAME, INDEX)                                                        \
    if (strcmp(NAME, NCCMP_USER_TYPE_ITEM(types, INDEX)->name)) {               \
        PRINT("Unexpected type name " #NAME " <> \"%s\"\n",                     \
              NCCMP_USER_TYPE_ITEM(types, INDEX)->name);                        \
        result = FAIL;                                                          \
        goto recover;                                                           \
    }

    CMP("comp1", 0);
    CMP("comp2", 13);
    CMP("comp3", 39);
#undef CMP
    
    t1 = NCCMP_USER_TYPE_ITEM(types, 39);
    t2 = NCCMP_USER_TYPE_ITEM(t1->fields, 0);
    t3 = NCCMP_USER_TYPE_ITEM(t2->fields, 0);
    t4 = NCCMP_USER_TYPE_ITEM(t3->fields, 11);
    name = "t1";
    if (strcmp("t1", t4->name)) {
        PRINT("Unexpected field name %s <> %s\n", name, t4->name);
        result = FAIL;
        goto recover;
    }

recover:
    nc_close(ncid);
    nccmp_destroy_user_type_array(types);

    return result;
}

int test_nccmp_shallow_copy_user_type()
{
    int result = PASS;
    nccmp_user_type_t to, from;

    from.name = malloc(32);
    from.tree_name = NULL;
    from.fields = NULL;
    to.name = malloc(32);
    to.tree_name = NULL;
    to.fields = NULL;
    
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
    nccmp_darray_t *types;

    status = nc_open(file1, NC_NOWRITE, & ncid);
    HANDLE_NC_ERROR(status);

    types = nccmp_load_group_user_type_map(ncid, 0, 0);

    strcpy(str, "");
    nccmp_user_type_array_to_str(types, str);

    nc_close(ncid);
    nccmp_destroy_user_type_array(types);

    return result;
}

int test_new_user_type()
{
    int result = PASS;
    nccmp_user_type_t *type = nccmp_create_user_type(1);

    if (!type) {
        PRINT("Expected item\n");
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_destroy_user_type(type);

    return result;
}

int main(int argc, char* argv[])
{
    TEST(test_new_user_type);
    TEST(test_nccmp_build_user_type_field_path_str);
    TEST(test_nccmp_get_user_type_names);
    TEST(test_nccmp_get_user_type_compound_field_names);
    TEST(test_ncccmp_create_user_type_field_id_pairs);
    TEST(test_nccmp_load_user_types);
    TEST(test_nccmp_shallow_copy_user_type);
    TEST(test_nccmp_print_user_types);

    return failures;
}
