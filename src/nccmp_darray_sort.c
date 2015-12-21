#include <nccmp_darray_sort.h>


#define NCCMP_DARRAY_SORT_PIVOT(TYPE, NAME) 															\
int nccmp_darray_sort_pivot_##NAME(nccmp_darray_t *array, int index_lo, int index_hi) 					\
{ 																										\
	int i, j;																							\
	i = j = index_lo; 																					\
	for(; j < index_hi; ++j) { 																			\
		if ( *(TYPE*) array->items[j] < *(TYPE*) array->items[index_hi] ) { 							\
			nccmp_darray_swap_items(array, i, j); 														\
			++i; 																						\
		} 																								\
	} 																									\
																										\
	nccmp_darray_swap_items(array, i, index_hi); 														\
	return i; 																							\
}

#define NCCMP_DARRAY_SORT_RUN(TYPE, NAME) \
void nccmp_darray_sort_run_##NAME(nccmp_darray_t *array, int index_lo, int index_hi) 				\
{ 																									\
	int pivot; 																						\
	if (index_lo < index_hi) { 																		\
		pivot = nccmp_darray_sort_pivot_##NAME(array, index_lo, index_hi);							\
		nccmp_darray_sort_run_##NAME(array, index_lo, pivot - 1);									\
		nccmp_darray_sort_run_##NAME(array, pivot + 1, index_hi);									\
	} 																								\
}

#define NCCMP_DARRAY_SORT(TYPE, NAME) 																\
void nccmp_darray_sort_##NAME(nccmp_darray_t *array) 												\
{ 																									\
	if (array && (array->num_items > 1)) { 															\
		nccmp_darray_sort_run_##NAME(array, 0, array->num_items - 1); 								\
	} 																								\
}
NCCMP_DARRAY_SORT_PIVOT(char, char)
NCCMP_DARRAY_SORT_PIVOT(double, double)
NCCMP_DARRAY_SORT_PIVOT(float, float)
NCCMP_DARRAY_SORT_PIVOT(int, int)
NCCMP_DARRAY_SORT_PIVOT(short, short)
NCCMP_DARRAY_SORT_PIVOT(long long, longlong)
NCCMP_DARRAY_SORT_PIVOT(size_t, size_t)
NCCMP_DARRAY_SORT_PIVOT(unsigned long long, ulonglong)
NCCMP_DARRAY_SORT_PIVOT(unsigned char, uchar)
NCCMP_DARRAY_SORT_PIVOT(unsigned short, ushort)
NCCMP_DARRAY_SORT_PIVOT(unsigned int, uint)
NCCMP_DARRAY_SORT_RUN(char, char)
NCCMP_DARRAY_SORT_RUN(double, double)
NCCMP_DARRAY_SORT_RUN(float, float)
NCCMP_DARRAY_SORT_RUN(int, int)
NCCMP_DARRAY_SORT_RUN(short, short)
NCCMP_DARRAY_SORT_RUN(long long, longlong)
NCCMP_DARRAY_SORT_RUN(size_t, size_t)
NCCMP_DARRAY_SORT_RUN(unsigned long long, ulonglong)
NCCMP_DARRAY_SORT_RUN(unsigned char, uchar)
NCCMP_DARRAY_SORT_RUN(unsigned short, ushort)
NCCMP_DARRAY_SORT_RUN(unsigned int, uint)
NCCMP_DARRAY_SORT(char, char)
NCCMP_DARRAY_SORT(double, double)
NCCMP_DARRAY_SORT(float, float)
NCCMP_DARRAY_SORT(int, int)
NCCMP_DARRAY_SORT(short, short)
NCCMP_DARRAY_SORT(long long, longlong)
NCCMP_DARRAY_SORT(size_t, size_t)
NCCMP_DARRAY_SORT(unsigned long long, ulonglong)
NCCMP_DARRAY_SORT(unsigned char, uchar)
NCCMP_DARRAY_SORT(unsigned short, ushort)
NCCMP_DARRAY_SORT(unsigned int, uint)
#undef NCCMP_DARRAY_SORT_PIVOT
#undef NCCMP_DARRAY_SORT_RUN
#undef NCCMP_DARRAY_SORT

int nccmp_darray_sort_str_pivot(nccmp_darray_t *array, int index_lo, int index_hi)
{
	int i, j;
	i = j = index_lo;
	for(; j < index_hi; ++j) {
		if ( strcmp( (char*)array->items[j], (char*)array->items[index_hi] ) < 0 ) {
			nccmp_darray_swap_items(array, i, j);
			++i;
		}
	}

	nccmp_darray_swap_items(array, i, index_hi);
	return i;
}

void nccmp_darray_sort_str_run(nccmp_darray_t *array, int index_lo, int index_hi)
{
	int pivot;
	if (index_lo < index_hi) {
		pivot = nccmp_darray_sort_str_pivot(array, index_lo, index_hi);
		nccmp_darray_sort_str_run(array, index_lo, pivot - 1);
		nccmp_darray_sort_str_run(array, pivot + 1, index_hi);
	}
}

void nccmp_darray_sort_str(nccmp_darray_t *array)
{
	if (array && (array->num_items > 1)) {
		nccmp_darray_sort_str_run(array, 0, array->num_items - 1);
	}
}

int nccmp_darray_sort_custom_pivot(nccmp_darray_t *array, int index_lo, int index_hi,
		int (*compare)(void*, void*))
{
	int i, j;
	i = j = index_lo;
	for(; j < index_hi; ++j) {
		if ( compare(array->items[j], array->items[index_hi]) < 0 ) {
			nccmp_darray_swap_items(array, i, j);
			++i;
		}
	}

	nccmp_darray_swap_items(array, i, index_hi);
	return i;
}

void nccmp_darray_sort_custom_run(nccmp_darray_t *array, int index_lo, int index_hi,
		int (*compare)(void*, void*))
{
	int pivot;
	if (index_lo < index_hi) {
		pivot = nccmp_darray_sort_custom_pivot(array, index_lo, index_hi, compare);
		nccmp_darray_sort_custom_run(array, index_lo, pivot - 1, compare);
		nccmp_darray_sort_custom_run(array, pivot + 1, index_hi, compare);
	}
}

void nccmp_darray_sort_custom(nccmp_darray_t *array, int (*compare)(void*, void*))
{
	if (array && (array->num_items > 1)) {
		nccmp_darray_sort_custom_run(array, 0, array->num_items - 1, compare);
	}
}
