#include <test_common.h>
#include <nccmp_darray.h>
#include <xmalloc.h>

int failures = 0;

int test_nccmp_darray_append()
{
    int result = PASS;
    nccmp_darray_t *array;
    int item1 = 1, item2 = 2;

    array = nccmp_darray_create(1);
    nccmp_darray_append(array, & item1);
    nccmp_darray_append(array, & item2);

    if (2 != array->num_items) {
    	PRINT("Expected 2 items in array, but got %d.\n", array->num_items);
    	result = FAIL;
    	goto recover;
    }

    if (2 != *(int*)array->items[1]) {
    	PRINT("Expected items[1]=2, but got %d.\n", array->items[1]);
    	result = FAIL;
    	goto recover;
    }


recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_compact()
{
    int result = PASS;
    nccmp_darray_t *array;
    int item1 = 1, item2 = 2, item3 = 3;
    int expect;

    array = nccmp_darray_create(1);
    nccmp_darray_append(array, & item1);
    nccmp_darray_append(array, & item2);
    nccmp_darray_append(array, & item3);

    expect = 3;
    if (expect != array->num_items) {
    	PRINT("Expected %d items in array, but got %d.\n", expect, array->num_items);
    	result = FAIL;
    	goto recover;
    }

    if (expect >= array->capacity) {
    	PRINT("Expected extra capacity, but got %d == num_items.\n", array->capacity);
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_compact(array);
    if (expect != array->capacity) {
    	PRINT("Expected capacity=%d, but got %d.\n", expect, array->capacity);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_create()
{
    int result = PASS;
    nccmp_darray_t *array;

    array = nccmp_darray_create(10);

    if (!array) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_create_filled()
{
    int result = PASS;
    nccmp_darray_t *array;
    int num_items = 2;
    void *items[num_items];
    int ints[2] = {1, 2};

    items[0] = & ints[0];
    items[1] = & ints[1];
    array = nccmp_darray_create_filled(items, num_items);

    if (!array) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }

    if (num_items != array->num_items) {
		PRINT("Expected num_items=%d, but got %d.\n", num_items, array->num_items);
		result = FAIL;
		goto recover;
	}

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_destroy_deep()
{
    int result = PASS;
    nccmp_darray_t *array;
    int *item1, *item2;

    item1 = XMALLOC(int, 1);
    *item1 = 1;
    item2 = XMALLOC(int, 1);
    *item2 = 2;

    array = nccmp_darray_create(1);
    nccmp_darray_append(array, item1);
    nccmp_darray_append(array, item2);

    if (2 != array->num_items) {
    	PRINT("Expected 2 items in array, but got %d.\n", array->num_items);
    	result = FAIL;
    	goto recover;
    }

	nccmp_darray_destroy_deep(array);

recover:
	return result;
}

int test_nccmp_darray_get()
{
    int result = PASS;
    nccmp_darray_t *array;
    int num_items = 2;
    void *items[num_items];
    int ints[2] = {1, 2};
    void *item;

    items[0] = & ints[0];
    items[1] = & ints[1];
    array = nccmp_darray_create_filled(items, num_items);

    if (!array) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }

    if (num_items != array->num_items) {
		PRINT("Expected num_items=%d, but got %d.\n", num_items, array->num_items);
		result = FAIL;
		goto recover;
	}

    item = nccmp_darray_get(array, 100);
    if (item) {
    	PRINT("Expected null item.\n");
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_get(array, 1);
    if (!item) {
    	PRINT("Expected non-null item.\n");
    	result = FAIL;
    	goto recover;
    }

    if (2 != *(int*)item) {
    	PRINT("Expected item=%d, but got %d.\n", 2, *(int*)item);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}
int main(int argc, char* argv[])
{
	TEST(test_nccmp_darray_create);
	TEST(test_nccmp_darray_create_filled);
	TEST(test_nccmp_darray_append);
	TEST(test_nccmp_darray_destroy_deep);
	TEST(test_nccmp_darray_compact);
	TEST(test_nccmp_darray_get);

	TEST(test_nccmp_darray_insert);
	TEST(test_nccmp_darray_remove);
	TEST(test_nccmp_darray_remove_front);
	TEST(test_nccmp_darray_remove_back);
	TEST(test_nccmp_darray_reverse);
	TEST(test_nccmp_darray_swap_items);

    return failures;
}
