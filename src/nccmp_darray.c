#include <log.h>
#include <nccmp_darray.h>
#include <xmalloc.h>

nccmp_darray_t* nccmp_darray_create(size_t capacity)
{
	nccmp_darray_t *result = XMALLOC(nccmp_darray_t, 1);

	result->capacity = capacity;
	result->num_items = 0;
	result->items = XCALLOC(void*, result->capacity);

	return result;
}

nccmp_darray_t* nccmp_darray_create_filled(void **items, size_t num_items)
{
	nccmp_darray_t *result = XMALLOC(nccmp_darray_t, 1);
	int i = 0;

	result->capacity = num_items * 2;
	result->items = XCALLOC(void*, result->capacity);

	if (items) {
		for(; i < num_items; ++i) {
			result->items[i] = items[i];
		}
	}

	result->num_items = i;

	return result;
}

void nccmp_darray_destroy_deep(nccmp_darray_t *array)
{
	int i;

	if (!array) {
		return;
	}

	if (array->items) {
		for(i = 0; i < array->num_items; ++i) {
			XFREE(array->items[i]);
		}
	}
	nccmp_darray_destroy(array);
}

void nccmp_darray_destroy(nccmp_darray_t *array)
{
	if (!array) {
		return;
	}

	XFREE(array->items);
	XFREE(array);
}

void nccmp_darray_append(nccmp_darray_t *array, void *item)
{
	void **old_items;
	int i;

	if (!array) {
		return;
	}

	if (array->capacity == array->num_items) {
		old_items = array->items;
		array->capacity *= 2;
		array->items = XCALLOC(void*, array->capacity);
		for(i = 0; i < array->num_items; ++i) {
			array->items[i] = old_items[i];
		}
		XFREE(old_items);
	}

	array->items[array->num_items] = item;
	++array->num_items;
}

void nccmp_darray_compact(nccmp_darray_t *array)
{
	void **old_items;
	int i;

	if (!array) {
		return;
	}

	old_items = array->items;
	array->capacity = array->num_items;
	array->items = XCALLOC(void*, array->capacity);
	for(i = 0; i < array->num_items; ++i) {
		array->items[i] = old_items[i];
	}
	XFREE(old_items);
}

void* nccmp_darray_get(nccmp_darray_t *array, size_t index)
{
	if (!array || (index >= array->num_items)) {
		return NULL;
	}

	return array->items[index];
}

void nccmp_darray_insert(nccmp_darray_t *array, size_t index, void *item)
{
	void **old_items;
	int i;

	if (!array) {
		return;
	}

	if (index >= array->num_items) {
		nccmp_darray_append(array, item);
	} else {
		if (array->capacity <= (array->num_items + 1)) {
			array->capacity = array->num_items * 2;
		}

		old_items = array->items;
		array->items = XCALLOC(void*, array->capacity);
		for(i = 0; i < index; ++i) {
			array->items[i] = old_items[i];
		}
		array->items[i] = item;
		for(; i < array->num_items; ++i) {
			array->items[i+1] = old_items[i];
		}

		array->num_items += 1;
	}
}

void nccmp_darray_print_debug(nccmp_darray_t *array)
{
	int i;
	LOG_DEBUG("darray=%p capacity=%d num_items=%d\n",
			   array,
			   array ? array->capacity : 0,
			   array ? array->num_items : 0);

	if (array) {
		for(i = 0; i < array->num_items; ++i) {
			LOG_DEBUG("  item[%d]=%p\n", i, array->items[i]);
		}
	}
}

void* nccmp_darray_remove(nccmp_darray_t *array, size_t index)
{
	void *result = NULL;
	int i;

	if (array && array->items && array->num_items && (index < array->num_items)) {
		result = array->items[index];
		array->num_items -= 1;
		for(i = index; i < array->num_items; ++i) {
			array->items[i] = array->items[i + 1];
		}
		array->items[array->num_items] = NULL; /* Clear previous last item. */
	}

	return result;
}

void* nccmp_darray_remove_back(nccmp_darray_t *array)
{
	void *result = NULL;

	if (array && array->items && array->num_items) {
		result = array->items[array->num_items - 1];
		array->num_items -= 1;
		array->items[array->num_items] = NULL;
	}

	return result;
}

void* nccmp_darray_remove_front(nccmp_darray_t *array)
{
	void *result = NULL;
	int i;

	if (array && array->items && array->num_items) {
		result = array->items[0];
		array->num_items -= 1;
		for(i = 0; i < array->num_items; ++i) {
			array->items[i] = array->items[i + 1];
		}
		array->items[array->num_items] = NULL; /* Clear previous last item. */
	}

	return result;
}

void nccmp_darray_reverse(nccmp_darray_t *array)
{
	int left, right;
	void *tmp;

	if (array && array->items && array->num_items) {
		left = 0;
		right = array->num_items - 1;
		while (left < right) {
			tmp = array->items[left];
			array->items[left] = array->items[right];
			array->items[right] = tmp;
			++left;
			--right;
		}
	}
}

void nccmp_darray_swap_items(nccmp_darray_t *array, size_t index1, size_t index2)
{
	void *tmp;

	if (array && array->items && array->num_items &&
	    index1 < array->num_items &&
		index2 < array->num_items) {
		tmp = array->items[index1];
		array->items[index1] = array->items[index2];
		array->items[index2] = tmp;
	}
}
