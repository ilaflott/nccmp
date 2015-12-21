#include <test_common.h>
#include <nccmp_darray_sort.h>

int failures = 0;

typedef struct test_item_t {
	char field1[8];
	int field2;
} test_item_t;

int custom_cmp_int(void* a, void* b)
{
	return *(int*)a < *(int*)b ? -1 : 1;
}

int test_nccmp_darray_sort_custom()
{
    int result = PASS;
    nccmp_darray_t *array;
    int ints[] = { 3, 2, 4, 1 };
    void *items[4];
    int *item;
    int expect;

    items[0] = & ints[0];
    items[1] = & ints[1];
    items[2] = & ints[2];
    items[3] = & ints[3];
    array = nccmp_darray_create_filled(items, 4);

	nccmp_darray_sort_custom(array, custom_cmp_int);

	item = (int*)nccmp_darray_get(array, 0);
	expect = 1;
    if (expect != *item) {
    	PRINT("Expected %d, but got %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (int*)nccmp_darray_get(array, 1);
	expect = 2;
    if (expect != *item) {
    	PRINT("Expected %d, but got %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (int*)nccmp_darray_get(array, 2);
	expect = 3;
    if (expect != *item) {
    	PRINT("Expected %d, but got %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (int*)nccmp_darray_get(array, 3);
	expect = 4;
    if (expect != *item) {
    	PRINT("Expected %d, but got %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

NCCMP_DARRAY_CMP_FIELD_STR(test_field_cmp_field1, test_item_t, field1)
NCCMP_DARRAY_CMP_FIELD(test_field_cmp_field2, test_item_t, field2)

int test_nccmp_darray_sort_field()
{
    int result = PASS;
    nccmp_darray_t *array;
    test_item_t data[3], *item;
    void *items[3];
    int expect;

    data[0].field2 = 3;
    data[1].field2 = 1;
    data[2].field2 = 2;
    items[0] = & data[0];
    items[1] = & data[1];
    items[2] = & data[2];
    array = nccmp_darray_create_filled(items, 3);

	nccmp_darray_sort_custom(array, test_field_cmp_field2);

	item = (test_item_t*)nccmp_darray_get(array, 0);
	expect = 1;
    if (expect != item->field2) {
    	PRINT("Expected %d, but got %d.\n", expect, item->field2);
    	result = FAIL;
    	goto recover;
    }

	item = (test_item_t*)nccmp_darray_get(array, 1);
	expect = 2;
    if (expect != item->field2) {
    	PRINT("Expected %d, but got %d.\n", expect, item->field2);
    	result = FAIL;
    	goto recover;
    }

	item = (test_item_t*)nccmp_darray_get(array, 2);
	expect = 3;
    if (expect != item->field2) {
    	PRINT("Expected %d, but got %d.\n", expect, item->field2);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_sort_field_str()
{
    int result = PASS;
    nccmp_darray_t *array;
    test_item_t data[3], *item;
    void *items[3];
    char *expect;

    strcpy(data[0].field1, "ac");
    strcpy(data[1].field1, "aa");
	strcpy(data[2].field1, "ab");
    items[0] = & data[0];
    items[1] = & data[1];
    items[2] = & data[2];
    array = nccmp_darray_create_filled(items, 3);

	nccmp_darray_sort_custom(array, test_field_cmp_field1);

	item = (test_item_t*)nccmp_darray_get(array, 0);
	expect = "aa";
    if (strcmp(expect, item->field1)) {
    	PRINT("Expected %s, but got %d.\n", expect, item->field1);
    	result = FAIL;
    	goto recover;
    }

	item = (test_item_t*)nccmp_darray_get(array, 1);
	expect = "ab";
	if (strcmp(expect, item->field1)) {
    	PRINT("Expected %s, but got %d.\n", expect, item->field1);
    	result = FAIL;
    	goto recover;
    }

	item = (test_item_t*)nccmp_darray_get(array, 2);
	expect = "ac";
	if (strcmp(expect, item->field1)) {
    	PRINT("Expected %s, but got %d.\n", expect, item->field1);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_sort_int()
{
    int result = PASS;
    nccmp_darray_t *array;
    int ints[] = { 3, 2, 4, 1 };
    void *items[4];
    int *item;
    int expect;

    items[0] = & ints[0];
    items[1] = & ints[1];
    items[2] = & ints[2];
    items[3] = & ints[3];
    array = nccmp_darray_create_filled(items, 4);

	nccmp_darray_sort_int(array);

	item = (int*)nccmp_darray_get(array, 0);
	expect = 1;
    if (expect != *item) {
    	PRINT("Expected %d, but got %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (int*)nccmp_darray_get(array, 1);
	expect = 2;
    if (expect != *item) {
    	PRINT("Expected %d, but got %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (int*)nccmp_darray_get(array, 2);
	expect = 3;
    if (expect != *item) {
    	PRINT("Expected %d, but got %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (int*)nccmp_darray_get(array, 3);
	expect = 4;
    if (expect != *item) {
    	PRINT("Expected %d, but got %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_sort_str()
{
    int result = PASS;
    nccmp_darray_t *array;
    char *items[] = { "30", "32", "12", "31" };
    char *item;
    char *expect;

    array = nccmp_darray_create_filled((void**)items, 4);

	nccmp_darray_sort_str(array);

	item = (char*)nccmp_darray_get(array, 0);
	expect = "12";
    if (strcmp(item, expect)) {
    	PRINT("Expected %s, but got %s.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (char*)nccmp_darray_get(array, 1);
	expect = "30";
	if (strcmp(item, expect)) {
    	PRINT("Expected %s, but got %s.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (char*)nccmp_darray_get(array, 2);
	expect = "31";
	if (strcmp(item, expect)) {
    	PRINT("Expected %s, but got %s.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

	item = (char*)nccmp_darray_get(array, 3);
	expect = "32";
	if (strcmp(item, expect)) {
    	PRINT("Expected %s, but got %s.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int main(int argc, char* argv[])
{
	TEST(test_nccmp_darray_sort_int);
	TEST(test_nccmp_darray_sort_str);
	TEST(test_nccmp_darray_sort_custom);
	TEST(test_nccmp_darray_sort_field);
	TEST(test_nccmp_darray_sort_field_str);

    return failures;
}
