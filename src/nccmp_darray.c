#include <log.h>
#include <nccmp_darray.h>
#include <xmalloc.h>

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

#define NCCMP_DARRAY_APPEND_TYPE(TYPE, NAME) 							\
void nccmp_darray_append_##NAME(nccmp_darray_t *array, TYPE value) {	\
	TYPE *item = (TYPE*)malloc(sizeof(TYPE));							\
	*item = value;														\
	nccmp_darray_append(array, item);									\
}
NCCMP_DARRAY_APPEND_TYPE(char, char)
NCCMP_DARRAY_APPEND_TYPE(double, double)
NCCMP_DARRAY_APPEND_TYPE(float, float)
NCCMP_DARRAY_APPEND_TYPE(int, int)
NCCMP_DARRAY_APPEND_TYPE(long long, longlong)
NCCMP_DARRAY_APPEND_TYPE(short, short)
NCCMP_DARRAY_APPEND_TYPE(size_t, size_t)
NCCMP_DARRAY_APPEND_TYPE(unsigned char, uchar)
NCCMP_DARRAY_APPEND_TYPE(unsigned long long, ulonglong)
NCCMP_DARRAY_APPEND_TYPE(unsigned short, ushort)
NCCMP_DARRAY_APPEND_TYPE(unsigned int, uint)
#undef NCCMP_DARRAY_APPEND_TYPE

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

void nccmp_darray_copy(nccmp_darray_t *to, nccmp_darray_t *from)
{
    if (to && from) {
        XFREE(to->items);
        to->capacity = from->capacity;
        to->num_items = from->num_items;
        to->items = XMALLOC(void*, to->capacity);
        memcpy(to->items, from->items, sizeof(void*) * from->num_items);
    }
}

nccmp_darray_t* nccmp_darray_create(size_t capacity)
{
	nccmp_darray_t *result = XMALLOC(nccmp_darray_t, 1);

	result->capacity = capacity;
	result->items = XCALLOC(void*, capacity);
	result->num_items = 0;

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

void nccmp_darray_destroy(nccmp_darray_t *array)
{
	if (array) {
		XFREE(array->items);
		XFREE(array);
	}
}

void nccmp_darray_destroy_custom(nccmp_darray_t *array, void (*free_func)(void*))
{
	int i;

	if (!array) {
		return;
	}

	if (array->items) {
		for(i = 0; i < array->num_items; ++i) {
			free_func(array->items[i]);
		}
	}
	nccmp_darray_destroy(array);
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

char nccmp_darray_empty(nccmp_darray_t *array)
{
	return !(array && array->items && array->num_items > 0);
}

void nccmp_darray_extend(nccmp_darray_t *first, nccmp_darray_t *second)
{
	void **old_items;

	if (!first || !first->items || !second || !second->items || second->num_items < 1) {
		return;
	}

	old_items = first->items;
	first->capacity = first->num_items + second->num_items;
	first->items = XMALLOC(void*, first->capacity);
	memcpy(first->items, old_items, sizeof(void*) * first->num_items);
	memcpy(first->items + first->num_items, second->items, sizeof(void*) * second->num_items);
	first->num_items = first->capacity;
	XFREE(old_items);
}

void* nccmp_darray_get(nccmp_darray_t *array, size_t index)
{
	if (!array || (index >= array->num_items)) {
		return NULL;
	}

	return array->items[index];
}

#define NCCMP_DARRAY_INDEX_TYPE(TYPE, NAME) 						\
int nccmp_darray_index_##NAME(nccmp_darray_t *array, TYPE value) 	\
{ 																	\
	int i; 															\
																	\
	if (array && array->num_items) { 								\
		for(i = 0; i < array->num_items; ++i) { 					\
			if (value == *(TYPE*)array->items[i]) { 				\
				return i; 											\
			} 														\
		} 															\
	} 																\
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	\
	return -1; 														\
}
NCCMP_DARRAY_INDEX_TYPE(char, char)
NCCMP_DARRAY_INDEX_TYPE(double, double)
NCCMP_DARRAY_INDEX_TYPE(float, float)
NCCMP_DARRAY_INDEX_TYPE(int, int)
NCCMP_DARRAY_INDEX_TYPE(long long, longlong)
NCCMP_DARRAY_INDEX_TYPE(short, short)
NCCMP_DARRAY_INDEX_TYPE(size_t, size_t)
NCCMP_DARRAY_INDEX_TYPE(unsigned char, uchar)
NCCMP_DARRAY_INDEX_TYPE(unsigned int, uint)
NCCMP_DARRAY_INDEX_TYPE(unsigned long long, ulonglong)
NCCMP_DARRAY_INDEX_TYPE(unsigned short, ushort)
#undef NCCMP_DARRAY_INDEX_TYPE

int nccmp_darray_index_custom(nccmp_darray_t *array, void *item, int (*compare)(void*, void*))
{
	int i;

	if (array && item && array->num_items) {
		for(i = 0; i < array->num_items; ++i) {
			if (0 == compare(item, array->items[i])) {
				return i;
			}
		}
	}

	return -1;
}

void nccmp_darray_insert(nccmp_darray_t *array, size_t index, void *item)
{
	void **old_items;

	if (!array || !array->items) {
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
		memcpy(array->items, old_items, sizeof(void*) * index);
		array->items[index] = item;
		memcpy(array->items + index + 1, old_items + index, sizeof(void*) * (array->num_items - index));
		array->num_items += 1;
		XFREE(old_items);
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

void nccmp_darray_reserve(nccmp_darray_t *array, size_t capacity)
{
	void **old_items;

	if (array) {
		if (capacity > array->capacity) {
			old_items = array->items;
			array->items = XCALLOC(void*, capacity);
			memcpy(array->items, old_items, sizeof(void*) * array->num_items);
			array->capacity = capacity;
			XFREE(old_items);
		}
	}
}

void nccmp_darray_reset(nccmp_darray_t *array)
{
	if (array) {
		if (array->items) {
			XFREE(array->items);
		}
		array->capacity = 1;
		array->num_items = 0;
		array->items = XCALLOC(void*, array->capacity);
	}
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

void* nccmp_darray_set(nccmp_darray_t *array, size_t index, void* item)
{
	void *old_item = NULL;

	if (array && array->items && index < array->capacity) {
		if (index + 1 > array->num_items) {
			array->num_items = index + 1;
		}
		old_item = array->items[index];
		array->items[index] = item;
	}

	return old_item;
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
