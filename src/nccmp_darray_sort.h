#ifndef NCCMP_DARRAY_SORT_H
#define NCCMP_DARRAY_SORT_H

#include <nccmp_darray.h>

/* Quicksort numeric types in dynamic array. */
#define NCCMP_DARRAY_SORT(TYPE, NAME) void nccmp_darray_sort_##NAME(nccmp_darray_t *array);
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
#undef NCCMP_DARRAY_SORT

/* Sort with custom compare function pointer.
   It must accept the two items as void pointers and return -1 if the first is less than the second, 0 if equal, 1 if greater.
*/
void nccmp_darray_sort_custom(nccmp_darray_t *array, int (*compare)(void*, void*));

/* Sort char string items. */
void nccmp_darray_sort_str(nccmp_darray_t *array);

#endif
