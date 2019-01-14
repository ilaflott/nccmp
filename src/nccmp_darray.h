#ifndef NCCMP_DARRAY_H
#define NCCMP_DARRAY_H

#include <nccmp_common.h>

/* Non-threadsafe dynamic array that grows when adding generic pointers. */
typedef struct nccmp_darray_t {
	size_t capacity;     /* Number of reserved pointers. */
	size_t num_items;    /* Number of actual items. */
	void **items;        /* Array of void pointers to arbitrary data. */
} nccmp_darray_t;

/* Add item and grow capacity if necessary. */
void nccmp_darray_append(nccmp_darray_t *array, void *item);

/* Allocate and append a single typed value. */
#define NCCMP_DARRAY_APPEND_TYPE(TYPE, NAME) void nccmp_darray_append_##NAME(nccmp_darray_t *array, TYPE value);
NCCMP_DARRAY_APPEND_TYPE(char, char)
NCCMP_DARRAY_APPEND_TYPE(double, double)
NCCMP_DARRAY_APPEND_TYPE(float, float)
NCCMP_DARRAY_APPEND_TYPE(int, int)
NCCMP_DARRAY_APPEND_TYPE(long long, longlong)
NCCMP_DARRAY_APPEND_TYPE(short, short)
NCCMP_DARRAY_APPEND_TYPE(size_t, size_t)
NCCMP_DARRAY_APPEND_TYPE(unsigned char, uchar)
NCCMP_DARRAY_APPEND_TYPE(unsigned int, uint)
NCCMP_DARRAY_APPEND_TYPE(unsigned long long, ulonglong)
NCCMP_DARRAY_APPEND_TYPE(unsigned short, ushort)
#undef NCCMP_DARRAY_APPEND_TYPE

/* Define compare function for numeric fields in a struct.
   Returns 0 if equal, negative if first less than second, and positive if greater than second.
   NULL items are considered greater, so they will go to end of sorted array.
*/
#define NCCMP_DARRAY_CMP_FIELD(NAME, TYPE, FIELD) 					  \
int NAME(void *first, void *second) 								          \
{ 																	                          \
	if (first) { 												                       	\
		if (second) { 												                    \
			return ((TYPE*)first)->FIELD - ((TYPE*)second)->FIELD;  \
		} else { 													                        \
			return -1; 												                      \
		} 															                          \
	} else if (second) {											                  \
		return 1;													                        \
	}																                            \
	return 0;														                        \
}

#define NCCMP_DARRAY_CMP_FIELD_STR(NAME, TYPE, FIELD) 				\
int NAME(void *first, void *second) 								          \
{ 																	                          \
	if (first) { 													                      \
		if (second) { 												                    \
			if (((TYPE*)first)->FIELD) { 							              \
				if (((TYPE*)second)->FIELD) { 						            \
					return strcmp( ((TYPE*)first)->FIELD, ((TYPE*)second)->FIELD );	\
				} else { 											                        \
					return -1; 										                      \
				} 													                          \
			} else { 												                        \
				return 1; 											                      \
			} 														                          \
       } else { 													                    \
    	   return -1;												                    \
       }															                        \
	} else if (second) {											                  \
		return 1;													                        \
	}																                            \
	return 0; 														                      \
}

/* Shrink storage capacity to number of items stored. */
void nccmp_darray_compact(nccmp_darray_t *array);

/* Resizes destination and copies items from source. */
void nccmp_darray_copy(nccmp_darray_t *to, nccmp_darray_t *from);

/* Creates empty array with capacity. */
nccmp_darray_t* nccmp_darray_create(size_t capacity);

/* Create and add all items. Caller still owns the items memory. */
nccmp_darray_t* nccmp_darray_create_filled(void **items, size_t num_items);

/* Free only the array, but not the items. */
void nccmp_darray_destroy(nccmp_darray_t *array);

/* Free the array and individual items. */
void nccmp_darray_destroy_deep(nccmp_darray_t *array);

/* Free the array and use custom destructor function on each item. */
void nccmp_darray_destroy_custom(nccmp_darray_t *array, void (*free_func)(void*));

/* Return 1 if empty or invalid, 0 if has at least one item. */
char nccmp_darray_empty(nccmp_darray_t *array);

/* Append copied pointers from second to first array. */
void nccmp_darray_extend(nccmp_darray_t *first, nccmp_darray_t *second);

// TODO RFIND
/* Linear scan returns first item with field equal to value. Returns NULL if not found. */
#define NCCMP_DARRAY_FIND_FIELD_DECLARE(NAME, ITEM_TYPE, FIELD, VALUE_TYPE) \
ITEM_TYPE* NAME(nccmp_darray_t *array, const VALUE_TYPE value);

#define NCCMP_DARRAY_FIND_FIELD(NAME, ITEM_TYPE, FIELD, VALUE_TYPE) \
ITEM_TYPE* NAME(nccmp_darray_t *array, const VALUE_TYPE value) 			\
{																	\
	int i;															\
	ITEM_TYPE *item = NULL;											\
	if (array) { 													\
		for(i = 0; i < array->num_items; ++i) {						\
			item = (ITEM_TYPE*) array->items[i];					\
			if (value == item->FIELD) {								\
				break;												\
			}														\
		}															\
	}																\
	return item;													\
}

/* Safe get 0-base index. Returns null if invalid index. */
void* nccmp_darray_get(nccmp_darray_t *array, size_t index);

#define NCCMP_DARRAY_INDEX_TYPE(TYPE, NAME) int nccmp_darray_index_##NAME(nccmp_darray_t *array, TYPE value);
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

#define NCCMP_DARRAY_ITEM(TYPE, ARRAY, INDEX) ((TYPE*) ARRAY->items[INDEX])

/* Linear scan for index of item equal to input. Returns -1 if not found. */
int nccmp_darray_index_custom(nccmp_darray_t *array, void *item, int (*compare)(void*, void*));

/* TODO: Bisect search for index of item equal to input. Returns -1 if not found. */
//int nccmp_darray_index_sorted(nccmp_darray_t *array, void *item, int (*compare)(void*, void*));

/* Insert before 0-base index. If num_items is exceeded, then appends. */
void nccmp_darray_insert(nccmp_darray_t *array, size_t index, void *item);

/* Print pointer addresses of items. */
void nccmp_darray_print(nccmp_darray_t *array, FILE *stream);

/* Safely remove 0-base index item and shrink array by one and shift items left.
   Returns null if invalid index.
   Caller owns returned item memory.
*/
void* nccmp_darray_remove(nccmp_darray_t *array, size_t index);

/* Safely get last item and shrink array by one.
   Returns null if empty array.
   Caller owns returned item memory.
*/
void* nccmp_darray_remove_back(nccmp_darray_t *array);

/* Safely get first item and shrink array by one.
   Returns null if empty array.
   Caller owns returned item memory.
*/
void* nccmp_darray_remove_front(nccmp_darray_t *array);

/* Reserves extra space up to new capacity. To shrink, see nccmp_darray_compact(). */
void nccmp_darray_reserve(nccmp_darray_t *array, size_t capacity);

/* Clears the item array and resets the size. */
void nccmp_darray_reset(nccmp_darray_t *array);

/* Reverse order of items. */
void nccmp_darray_reverse(nccmp_darray_t *array);

/* Safe set 0-base index. Returns previous item at index, or NULL if none existed. */
void* nccmp_darray_set(nccmp_darray_t *array, size_t index, void* item);

/* Swap two items using 0-based index. */
void nccmp_darray_swap_items(nccmp_darray_t *array, size_t index1, size_t index2);

#endif // DARRAY_H
