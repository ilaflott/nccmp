#ifndef NCCMP_DARRAY_H
#define NCCMP_DARRAY_H

/* Non-threadsafe dynamic array that grows when adding more items. */
typedef struct nccmp_darray_t {
	size_t capacity;     /* Number of reserved pointers. */
	size_t num_items;    /* Number of actual items. */
	void **items;        /* Array of void pointers to arbitrary data. */
} nccmp_darray_t;

/* Creates empty array with capacity. */
nccmp_darray_t* nccmp_darray_create(size_t capacity);

/* Create and add all with items. */
nccmp_darray_t* nccmp_darray_create_filled(void **items, size_t num_items);

/* Free the items and the array. */
void nccmp_darray_destroy_deep(nccmp_darray_t *array);

/* Free only the array, but not the items. */
void nccmp_darray_destroy(nccmp_darray_t *array);

/* Add item and grow capacity if necessary. */
void nccmp_darray_append(nccmp_darray_t *array, void *item);

/* Shrink storage capacity to number of items stored. */
void nccmp_darray_compact(nccmp_darray_t *array);

/* Safe get 0-base index. Returns null if invalid index. */
void* nccmp_darray_get(nccmp_darray_t *array, size_t index);

/* Insert before 0-base index. If num_items is exceeded, then appends. */
void nccmp_darray_insert(nccmp_darray_t *array, size_t index, void *item);

/* Print pointer addresses of items. */
void nccmp_darray_print(nccmp_darray_t *array, FILE *stream);

/* Safely remove 0-base index item and shrink array by one and shift items left. Returns null if invalid index. */
void* nccmp_darray_remove(nccmp_darray_t *array, size_t index);

/* Safely get last item and shrink array by one. Returns null if empty array. */
void* nccmp_darray_remove_back(nccmp_darray_t *array);

/* Safely get first item and shrink array by one. Returns null if empty array. */
void* nccmp_darray_remove_front(nccmp_darray_t *array);

/* Reverse order of items. */
void nccmp_darray_reverse(nccmp_darray_t *array);

/* Swap two items using 0-based index. */
void nccmp_darray_swap_items(nccmp_darray_t *array, size_t index1, size_t index2);

#endif // DARRAY_H
