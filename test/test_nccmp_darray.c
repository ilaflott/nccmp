#include <test_common.h>
#include <nccmp_darray.h>
#include <xmalloc.h>

int failures = 0;

int compare_int(void *a, void *b)
{
	return *(int*)a - *(int*)b;
}

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

int test_nccmp_darray_copy()
{
    int result = PASS;
    nccmp_darray_t *array1, *array2;
    int item1 = 1, item2 = 2;

    array1 = nccmp_darray_create(1);
    array2 = nccmp_darray_create(1);
    nccmp_darray_append(array1, & item1);
    nccmp_darray_append(array1, & item2);
    nccmp_darray_copy(array2, array1);

    if (2 != array2->num_items) {
    	PRINT("Expected 2 items in array, but got %d.\n", array2->num_items);
    	result = FAIL;
    	goto recover;
    }

    if (2 != *(int*) array2->items[1]) {
    	PRINT("Expected items[1]=2, but got %d.\n", array2->items[1]);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array1);
	nccmp_darray_destroy(array2);

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
    int ints[] = {1, 2};

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

void free_custom(void *item) {
	XFREE(item);
}

int test_nccmp_darray_destroy_custom()
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

	nccmp_darray_destroy_custom(array, free_custom);

recover:
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

int test_nccmp_darray_empty()
{
    int result = PASS;
    nccmp_darray_t *array = NULL;
    int item1 = 1;

    if (!nccmp_darray_empty(array)) {
    	PRINT("Expected empty.\n");
    	result = FAIL;
    	goto recover;
    }

    array = nccmp_darray_create(1);
    if (!nccmp_darray_empty(array)) {
    	PRINT("Expected empty.\n");
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_append(array, & item1);
    if (nccmp_darray_empty(array)) {
    	PRINT("Expected not empty.\n");
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_extend()
{
    int result = PASS;
    nccmp_darray_t *array1, *array2;
    int num_items = 2;
    int ints[] = {1, 2, 3, 4};
    void *items1[num_items];
    void *items2[num_items];
    int *item = NULL;
    int expect, i;

    items1[0] = & ints[0];
    items1[1] = & ints[1];
    items2[0] = & ints[2];
    items2[1] = & ints[3];

    array1 = nccmp_darray_create_filled(items1, num_items);
    array2 = nccmp_darray_create_filled(items2, num_items);
    nccmp_darray_extend(array1, array2);

    expect = 4;
    if (expect != array1->num_items) {
    	PRINT("Expected %d, but got %d.\n", expect, array1->num_items);
    	result = FAIL;
    	goto recover;
    }

    for(i = 0; i < num_items * 2; ++i) {
		expect = i + 1;
		item = (int*)nccmp_darray_get(array1, i);
		if (!item) {
			PRINT("Expected non null item.\n");
			result = FAIL;
			goto recover;
		}
		if (expect != *item) {
			PRINT("Expected %d, but got %d.\n", expect, item);
			result = FAIL;
			goto recover;
		}
    }

recover:
	nccmp_darray_destroy(array1);
	nccmp_darray_destroy(array2);

	return result;
}

int test_nccmp_darray_get()
{
    int result = PASS;
    nccmp_darray_t *array;
    int num_items = 2;
    void *items[num_items];
    int ints[] = {1, 2};
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

int test_nccmp_darray_index()
{
    int result = PASS;
    nccmp_darray_t *array;
    int ints[] = {1, 2, 3};
    void *items[3];
    int idx;
    int expect;

    items[0] = & ints[0];
    items[1] = & ints[1];
    items[2] = & ints[2];
    array = nccmp_darray_create_filled(items, 3);

    idx = nccmp_darray_index_int(array, 0);
    expect = -1;
    if (idx != expect) {
    	PRINT("Expected %d, but got %d.\n", expect, idx);
    	result = FAIL;
    	goto recover;
    }

    idx = nccmp_darray_index_int(array, 3);
    expect = 2;
    if (idx != expect) {
    	PRINT("Expected %d, but got %d.\n", expect, idx);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_index_custom()
{
    int result = PASS;
    nccmp_darray_t *array;
    int ints[] = {1, 2, 3};
    void *items[3];
    int idx;
    int expect;
    int item;

    items[0] = & ints[0];
    items[1] = & ints[1];
    items[2] = & ints[2];
    array = nccmp_darray_create_filled(items, 3);

    idx = nccmp_darray_index_custom(array, 0, compare_int);
    expect = -1;
    if (idx != expect) {
    	PRINT("Expected %d, but got %d.\n", expect, idx);
    	result = FAIL;
    	goto recover;
    }

    item = 3;
    idx = nccmp_darray_index_custom(array, & item, compare_int);
    expect = 2;
    if (idx != expect) {
    	PRINT("Expected %d, but got %d.\n", expect, idx);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_insert()
{
    int result = PASS;
    nccmp_darray_t *array;
    int item1 = 1, item2 = 2, item3 = 3;
    int *item;

    array = nccmp_darray_create(1);

    if (!array) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_insert(array, 10, & item1);
    item = (int*)nccmp_darray_get(array, 0);
    if (1 != *item) {
    	PRINT("Expected 1 at index=0, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_insert(array, 0, & item2);
    item = (int*)nccmp_darray_get(array, 0);
    if (2 != *item) {
    	PRINT("Expected 2 at index=0, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_insert(array, 1, & item3);
    item = (int*)nccmp_darray_get(array, 1);
    if (3 != *item) {
    	PRINT("Expected 3 at index=0, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_remove()
{
    int result = PASS;
    nccmp_darray_t *array;
    int *item;

    array = nccmp_darray_create(1);

    if (!array) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_remove(array, 0);
    if (item) {
    	PRINT("Expected null item.\n");
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_append_int(array, 1);
    nccmp_darray_append_int(array, 2);

    item = nccmp_darray_remove(array, 2);
    if (item) {
    	PRINT("Expected null item.\n");
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_remove(array, 1);
    if (!item) {
    	PRINT("Expected non-null item.\n");
    	result = FAIL;
    	goto recover;
    }
    if (2 != *item) {
    	PRINT("Expected item=2, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }
    XFREE(item);

    item = nccmp_darray_remove(array, 0);
    if (!item) {
    	PRINT("Expected non-null item.\n");
    	result = FAIL;
    	goto recover;
    }
    if (1 != *item) {
    	PRINT("Expected item=1, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }
    XFREE(item);

    if (array->num_items) {
    	PRINT("Expected empty, but is size of %d.\n", array->num_items);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy_deep(array);

	return result;
}

int test_nccmp_darray_remove_back()
{
    int result = PASS;
    nccmp_darray_t *array;
    int *item;

    array = nccmp_darray_create(1);

    if (!array) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_remove_back(array);
    if (item) {
    	PRINT("Expected null item.\n");
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_append_int(array, 1);
    nccmp_darray_append_int(array, 2);

    item = nccmp_darray_remove_back(array);
    if (!item) {
    	PRINT("Expected non-null item.\n");
    	result = FAIL;
    	goto recover;
    }
    if (2 != *item) {
    	PRINT("Expected item=2, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }
    XFREE(item);

    item = nccmp_darray_remove_back(array);
    if (!item) {
    	PRINT("Expected non-null item.\n");
    	result = FAIL;
    	goto recover;
    }
    if (1 != *item) {
    	PRINT("Expected item=1, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }
    XFREE(item);

    if (array->num_items) {
    	PRINT("Expected empty, but is size of %d.\n", array->num_items);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy_deep(array);

	return result;
}

int test_nccmp_darray_remove_front()
{
    int result = PASS;
    nccmp_darray_t *array;
    int *item;

    array = nccmp_darray_create(1);

    if (!array) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_remove_front(array);
    if (item) {
    	PRINT("Expected null item.\n");
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_append_int(array, 1);
    nccmp_darray_append_int(array, 2);

    item = nccmp_darray_remove_front(array);
    if (!item) {
    	PRINT("Expected non-null item.\n");
    	result = FAIL;
    	goto recover;
    }
    if (1 != *item) {
    	PRINT("Expected item=1, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }
    XFREE(item);

    item = nccmp_darray_remove_front(array);
    if (!item) {
    	PRINT("Expected non-null item.\n");
    	result = FAIL;
    	goto recover;
    }
    if (2 != *item) {
    	PRINT("Expected item=2, but got %d.\n", *item);
    	result = FAIL;
    	goto recover;
    }
    XFREE(item);

    if (array->num_items) {
    	PRINT("Expected empty, but is size of %d.\n", array->num_items);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy_deep(array);

	return result;
}

int test_nccmp_darray_reserve()
{
    int result = PASS;
    nccmp_darray_t *array;
    int item1 = 1, item2 = 2;
    int *item;
    int expect;

    array = nccmp_darray_create(1);
    nccmp_darray_append(array, & item1);
    nccmp_darray_append(array, & item2);

    expect = 5;
    nccmp_darray_reserve(array, expect);
    if (expect != array->capacity) {
    	PRINT("Expected %d != %d.\n", expect, array->capacity);
    	result = FAIL;
    	goto recover;
    }

    expect = 1;
    item = (int*) nccmp_darray_get(array, 0);
    if (expect != *item) {
    	PRINT("Expected %d != %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

    expect = 2;
    item = (int*) nccmp_darray_get(array, 1);
    if (expect != *item) {
    	PRINT("Expected %d != %d.\n", expect, *item);
    	result = FAIL;
    	goto recover;
    }

    item = (int*) nccmp_darray_get(array, 2);
    if (item) {
    	PRINT("Expected null item.\n");
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_reverse()
{
    int result = PASS;
    nccmp_darray_t *array;
    int *item;
    int expect;

    array = nccmp_darray_create(1);

    if (!array) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_reverse(array);

    nccmp_darray_append_int(array, 1);
    nccmp_darray_append_int(array, 2);
    nccmp_darray_append_int(array, 3);
    nccmp_darray_reverse(array);

    item = nccmp_darray_get(array, 0);
    if (!item) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }
    expect = 3;
    if (expect != *item) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_get(array, 1);
    if (!item) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }
    expect = 2;
    if (expect != *item) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_get(array, 2);
    if (!item) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }
    expect = 1;
    if (expect != *item) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

    nccmp_darray_destroy_deep(array);
    array = nccmp_darray_create(1);
    nccmp_darray_append_int(array, 1);
    nccmp_darray_append_int(array, 2);
    nccmp_darray_reverse(array);

    item = nccmp_darray_get(array, 0);
    if (!item) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }
    expect = 2;
    if (expect != *item) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_get(array, 1);
    if (!item) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }
    expect = 1;
    if (expect != *item) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy_deep(array);

	return result;
}

int test_nccmp_darray_reset()
{
    int result = PASS;
    nccmp_darray_t *array;
    int item1 = 1, item2 = 2;

    array = nccmp_darray_create(1);
    nccmp_darray_append(array, & item1);
    nccmp_darray_append(array, & item2);
    
    nccmp_darray_reset(array);
    
    if (array->num_items) {
    	PRINT("Expected empty.\n");
    	result = FAIL;
    	goto recover;
    }
    
    if (array->items[0]) {
    	PRINT("Expected null items.\n");
    	result = FAIL;
    	goto recover;
    }
    
recover:
    nccmp_darray_destroy(array);

    return result;
}

int test_nccmp_darray_set()
{
    int result = PASS;
    nccmp_darray_t *array;
    int ints[] = {1, 2, 3};
    void *items[3];
    int item1;
    int *item;
    int expect;

    items[0] = & ints[0];
    items[1] = & ints[1];
    items[2] = & ints[2];
    array = nccmp_darray_create_filled(items, 3);
    item1 = 4;
    item = nccmp_darray_set(array, 3, & item1);
    if (item) {
    	PRINT("Expected null item.\n");
    	result = FAIL;
    	goto recover;
    }

    item = (int*) nccmp_darray_set(array, 0, & item1);
    expect = 1; /* Old value. */
    if (!item || *item != expect) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

    item = (int*) nccmp_darray_get(array, 0);
    expect = item1;
    if (!item || *item != expect) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy(array);

	return result;
}

int test_nccmp_darray_swap_items()
{
    int result = PASS;
    nccmp_darray_t *array;
    int *item;
    int expect;

    array = nccmp_darray_create(1);
    nccmp_darray_swap_items(array, 0, 1);

    nccmp_darray_append_int(array, 1);
    nccmp_darray_append_int(array, 2);
    nccmp_darray_swap_items(array, 0, 1);

    item = nccmp_darray_get(array, 0);
    if (!item) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }
    expect = 2;
    if (expect != *item) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

    item = nccmp_darray_get(array, 1);
    if (!item) {
    	PRINT("Expected non-null array.\n");
    	result = FAIL;
    	goto recover;
    }
    expect = 1;
    if (expect != *item) {
    	PRINT("Expected %d.\n", expect);
    	result = FAIL;
    	goto recover;
    }

recover:
	nccmp_darray_destroy_deep(array);

	return result;
}

int main(int argc, char* argv[])
{
	TEST(test_nccmp_darray_create);
	TEST(test_nccmp_darray_create_filled);
	TEST(test_nccmp_darray_append);
	TEST(test_nccmp_darray_destroy_deep);
	TEST(test_nccmp_darray_destroy_custom);
	TEST(test_nccmp_darray_compact);
    TEST(test_nccmp_darray_copy);
	TEST(test_nccmp_darray_empty);
	TEST(test_nccmp_darray_get);
	TEST(test_nccmp_darray_set);
	TEST(test_nccmp_darray_index);
	TEST(test_nccmp_darray_insert);
	TEST(test_nccmp_darray_remove);
	TEST(test_nccmp_darray_remove_back);
	TEST(test_nccmp_darray_remove_front);
	TEST(test_nccmp_darray_reset);
	TEST(test_nccmp_darray_reserve);
	TEST(test_nccmp_darray_reverse);
	TEST(test_nccmp_darray_swap_items);
	TEST(test_nccmp_darray_extend);

    return failures;
}
