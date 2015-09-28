#include "../src/nccmp_strlist.h"
#include "test_common.h"

int failures = 0;

int test_add_to_strlist()
{
    int result = PASS;
    nccmp_strlist_t* list = nccmp_new_strlist(10);

    if (!nccmp_is_strlist_valid(list)) {
        return FAIL;
    }

    const char * str = "foo";
    nccmp_add_to_strlist(list, str);

    if (strcmp(list->items[0], str) != 0) {
        PRINT("Expected strings[0] = %s", str);
        result = FAIL;
    }

    if (1 != list->size) {
        PRINT("Expected size = 1");
        result = FAIL;
    }

    nccmp_free_strlist(& list);

    return result;
}

int test_append_to_strlist()
{
    int result = PASS;
    const int n = 10;
    int status;
    nccmp_strlist_t* list = nccmp_new_strlist(n);

    if (! nccmp_is_strlist_valid(list)) {
        PRINT("Expected new list;");
        return FAIL;
    }

    const char * str = "foo";
    status = nccmp_append_to_strlist(& list, str);

    if (status) {
        PRINT("Expected 0, but got status = %d", status);
        result = FAIL;
        goto recover;
    }

    if (list->capacity != (n+1)) {
        PRINT("Expected capacity = %d", n+1);
        result = FAIL;
        goto recover;
    }

    if (list->size != 1) {
        PRINT("Expected size = 1.");
        result = FAIL;
        goto recover;
    }

    if (! list->items[list->capacity-1]) {
        PRINT("Expected non-empty string at end of list.");
        result = FAIL;
        goto recover;
    }

    if (strcmp(list->items[list->capacity-1], str) != 0) {
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_free_strlist(& list);

    return result;
}

int test_clear_strlist()
{
    int result = PASS, status;
    const int n = 10;
    nccmp_strlist_t* list = nccmp_new_strlist(n);

    if (!nccmp_is_strlist_valid(list)) {
        PRINT("Expected new list");
        return FAIL;
    }

    const char * str = "foo";
    status = nccmp_add_to_strlist(list, str);
    if (status) {
        PRINT("Expected 0 status but got %d", status);
        result = FAIL;
        goto recover;
    }

    if (1 != list->size) {
        PRINT("Expected size = 1, but got %d", list->size);
        result = FAIL;
        goto recover;
    }

    if (! list->items[0]) {
        PRINT("Expected non-empty string.");
        result = FAIL;
        goto recover;
    }

    if (strcmp(list->items[0], str)) {
        PRINT("Expected strings[0] == %s", str);
        result = FAIL;
        goto recover;
    }

    status = nccmp_clear_strlist(list);
    if (status) {
        PRINT("Expected 0 status but got %d", status);
        result = FAIL;
        goto recover;
    }

    if (0 != list->size) {
        PRINT("Expected size = 0");
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_free_strlist(& list);

    return result;
}

int test_deep_copy_strlist()
{
    int result = PASS, status;
    const int n = 10;
    nccmp_strlist_t* list1 = nccmp_new_strlist(n);
    nccmp_strlist_t* list2 = nccmp_new_strlist(n);
    const char* str = "foo";

    status = nccmp_add_to_strlist(list1, str);
    if (status) {
        PRINT("Expected nonzero status, but got %d", status);
        result = FAIL;
        goto recover;
    }

    status = nccmp_deep_copy_strlist(list1, list2);
    if (status) {
        PRINT("Expected nonzero status, but got %d", status);
        result = FAIL;
        goto recover;
    }

    if (list2->size != 1) {
        PRINT("Expected size = 1, but got %d", list2->size);
        result = FAIL;
        goto recover;
    }

    if (! list2->items[0]) {
        PRINT("Expected non-empty string");
        result = FAIL;
        goto recover;
    }

    if (strcmp(list2->items[0], str)) {
        PRINT("Expected strings[0] = %s", str);
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_free_strlist(& list1);
    nccmp_free_strlist(& list2);

    return result;
}

int test_diff_strlist()
{
    int result = PASS, status;
    const int n = 10;
    nccmp_strlist_t* list1 = nccmp_new_strlist(n); /* foo sip */
    nccmp_strlist_t* list2 = nccmp_new_strlist(n); /* bar sip */
    nccmp_strlist_t* diff  = nccmp_new_strlist(n); /* foo */
    const char* str1 = "foo";
    const char* str2 = "bar";
    const char* str3 = "sip";

    status = nccmp_add_to_strlist(list1, str1);
    if (status) {
        PRINT("Unexpected status = %d", status);
        result = FAIL;
        goto recover;
    }

    status = nccmp_add_to_strlist(list1, str3);
    if (status) {
        PRINT("Unexpected status = %d", status);
        result = FAIL;
        goto recover;
    }

    status = nccmp_add_to_strlist(list2, str2);
    if (status) {
        PRINT("Unexpected status = %d", status);
        result = FAIL;
        goto recover;
    }

    status = nccmp_add_to_strlist(list2, str3);
    if (status) {
        PRINT("Unexpected status = %d", status);
        result = FAIL;
        goto recover;
    }

    status = nccmp_diff_strlist(list1, list2, diff);
    if (status) {
        PRINT("Unexpected status = %d", status);
        result = FAIL;
        goto recover;
    }

    if (1 != diff->size) {
        PRINT("Expected size = 1");
        result = FAIL;
        goto recover;
    }

    if (0 != strcmp(diff->items[0], str1)) {
        PRINT("Expected strings[0] = %s", str1);
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_free_strlist(& list1);
    nccmp_free_strlist(& list2);
    nccmp_free_strlist(& diff);

    return result;
}

int test_exists_in_strlist()
{
    int result = PASS, status;
    const int n = 10;
    nccmp_strlist_t* list1 = nccmp_new_strlist(n);
    const char* str1 = "foo";
    const char* str2 = "bar";
    const char* str3 = "sip";
    const char* str4 = "missing";

    status = nccmp_add_to_strlist(list1, str1);
    if (status) {
        PRINT("Unexpected status = %d", status);
        result = FAIL;
        goto recover;
    }

    status = nccmp_add_to_strlist(list1, str2);
    if (status) {
        PRINT("Unexpected status = %d", status);
        result = FAIL;
        goto recover;
    }

    status = nccmp_add_to_strlist(list1, str3);
    if (status) {
        PRINT("Unexpected status = %d", status);
        result = FAIL;
        goto recover;
    }

    if (! nccmp_exists_in_strlist(list1, str3)) {
        PRINT("Expected to find str = %s", str3);
        result = FAIL;
        goto recover;
    }

    if (nccmp_exists_in_strlist(list1, str4)) {
        PRINT("Expected NOT to find str = %s", str4);
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_free_strlist(& list1);

    return result;
}

int test_free_strlist()
{
    nccmp_strlist_t* list = nccmp_new_strlist(10);

    if (!nccmp_is_strlist_valid(list)) {
        return FAIL;
    }

    nccmp_free_strlist(& list);
    if (NULL != list) {
        return FAIL;
    }

    return PASS;
}

int test_index_in_strlist()
{
    int result = PASS;
    const int n = 10;
    nccmp_strlist_t* list1 = nccmp_new_strlist(n);
    const char* str1 = "foo";
    const char* str2 = "bar";
    const char* str3 = "raz";
    const char* str4 = "missing";

    nccmp_add_to_strlist(list1, str1);
    nccmp_add_to_strlist(list1, str2);
    nccmp_add_to_strlist(list1, str3);

    if (nccmp_index_in_strlist(list1, str1) != 0) {
        PRINT("Expected to find index = 0");
        result = FAIL;
        goto recover;
    }

    if (nccmp_index_in_strlist(list1, str2) != 1) {
        PRINT("Expected to find index = 1");
        result = FAIL;
        goto recover;
    }

    if (nccmp_index_in_strlist(list1, str3) != 2) {
        PRINT("Expected to find index = 2");
        result = FAIL;
        goto recover;
    }

    if (nccmp_index_in_strlist(list1, str4) != -1) {
        PRINT("Expected to find index = -1");
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_free_strlist(& list1);

    return result;
}

int test_new_strlist()
{
    int result = PASS;
    const int n = 8192;
    nccmp_strlist_t* list = nccmp_new_strlist(n);

    if (!nccmp_is_strlist_valid(list)) {
        PRINT("Expected new list allocated.");
        return FAIL;
    }

    if (list->size != 0) {
        PRINT("Expected size = %d", 0);
        result = FAIL;
    }

    if (list->capacity != n) {
        PRINT("Expected capacity = %d", n);
        result = FAIL;
    }

    nccmp_free_strlist(& list);

    return result;
}

int test_split_strlist_by_delim()
{
    int result = PASS, status;
    const int n = 10;
    nccmp_strlist_t* list1 = nccmp_new_strlist(n);
    const char* null = NULL;
    const char* empty = "";
    const char* commas = ",,";
    const char* str = "foo,bar,baz sip";

    status = nccmp_split_strlist_by_delim(null, list1, ",");
    if (status) {
        PRINT("Failed to split null string.");
        result = FAIL;
        goto recover;
    }

    if (list1->size) {
        PRINT("Expected empty list after null string split.");
        result = FAIL;
        goto recover;
    }

    status = nccmp_split_strlist_by_delim(empty, list1, ",");
    if (status) {
        PRINT("Failed to split empty string.");
        result = FAIL;
        goto recover;
    }

    if (list1->size) {
        PRINT("Expected empty list after empty string split.");
        result = FAIL;
        goto recover;
    }

    status = nccmp_split_strlist_by_delim(commas, list1, ",");
    if (status) {
        PRINT("Failed to split comma only string.");
        result = FAIL;
        goto recover;
    }

    if (list1->size) {
        PRINT("Expected empty list after comma only string split.");
        result = FAIL;
        goto recover;
    }

    nccmp_split_strlist_by_delim(str, list1, ",");

    if (!nccmp_exists_in_strlist(list1, "foo")) {
        PRINT("Expected to find 'foo'");
        result = FAIL;
        goto recover;
    }

    if (!nccmp_exists_in_strlist(list1, "bar")) {
        PRINT("Expected to find 'bar'");
        result = FAIL;
        goto recover;
    }

    if (!nccmp_exists_in_strlist(list1, "baz sip")) {
        PRINT("Expected to find 'baz sip'");
        result = FAIL;
        goto recover;
    }

recover:
    nccmp_free_strlist(& list1);

    return result;
}

int test_union_strlist()
{
    int result = PASS;
    const int n = 10;
    nccmp_strlist_t* list1 = nccmp_new_strlist(n);
    nccmp_strlist_t* list2 = nccmp_new_strlist(n);
    nccmp_strlist_t* list3 = nccmp_new_strlist(n);
    const char* str1 = "foo";
    const char* str2 = "bar";
    const char* str3 = "raz";

    nccmp_add_to_strlist(list1, str1);
    nccmp_add_to_strlist(list1, str2);
    nccmp_add_to_strlist(list2, str3);

    nccmp_union_strlist(list1, list2, list3);

    if (!nccmp_exists_in_strlist(list3, str1)) {
        PRINT("Expected to find str = %s", str1);
        result = FAIL;
    }

    if (!nccmp_exists_in_strlist(list3, str2)) {
        PRINT("Expected to find str = %s", str2);
        result = FAIL;
    }

    if (!nccmp_exists_in_strlist(list3, str3)) {
        PRINT("Expected to find str = %s", str3);
        result = FAIL;
    }

    nccmp_free_strlist(& list1);
    nccmp_free_strlist(& list2);
    nccmp_free_strlist(& list3);

    return result;
}

int main(int argc, char* argv[])
{
    TEST(test_new_strlist);
    TEST(test_free_strlist);
    TEST(test_add_to_strlist);
    TEST(test_append_to_strlist);
    TEST(test_clear_strlist);
    TEST(test_deep_copy_strlist);
    TEST(test_diff_strlist);
    TEST(test_exists_in_strlist);
    TEST(test_index_in_strlist);
    TEST(test_split_strlist_by_delim);
    TEST(test_union_strlist);

    return failures;
}
