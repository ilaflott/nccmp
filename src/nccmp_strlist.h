/*
   Copyright (C) 2004-2007, remik @ fastmail . com

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#ifndef NCCMP_STRLIST_H
#define NCCMP_STRLIST_H 1

#include "nccmp_common.h"

typedef struct nccmp_strlist_t {
	char** items;        /* Array of string pointers. Empty slots are NULL. */
	unsigned int capacity; /* Maximum number of string slots (pointers). */
	unsigned int size;     /* Number of actual non-null, non-empty strings. */
} nccmp_strlist_t;

/* Does deep copy of string to first available slot.
   Fails if list is full (no NULL entries).
   Returns 0 if success. */
int nccmp_add_to_strlist(nccmp_strlist_t* list, const char* string);

/* Add string that is not null-terminated. */
int nccmp_add_to_strlist_len(nccmp_strlist_t* list, const char* string, size_t len);

/* Grows list and adds string to new last slot. */
int nccmp_append_to_strlist(nccmp_strlist_t** list, const char* string);

/* Frees memory for each string, but not list of strings. Returns 0 if success. */
int nccmp_clear_strlist(nccmp_strlist_t* list);

/* Compute string lengths and return new array. Caller must free the result array. */
int* nccmp_compute_strlist_lengths(nccmp_strlist_t* list);

/* Print all pointers allocated. */
void nccmp_debug_print_strlist(FILE* f, nccmp_strlist_t* list, char delim);

/* Deep copy. Requires 'dst' capacity greater than or equal to 'src'.
   Strings in 'listdst' after 'nsrc' position are left intact.*/
int nccmp_deep_copy_strlist(nccmp_strlist_t* src, nccmp_strlist_t* dst);

/* Compute string list simple difference (not symmetric) list1 - list2.
   Example: {a,b} - {a,c} = {b}
   'list1' items are returned unless exist in 'list2'.
   'listdiff' must already be allocated to
   accommodate size(list1) + size(list2) worst case.
*/
int nccmp_diff_strlist(nccmp_strlist_t* list1, nccmp_strlist_t* list2, nccmp_strlist_t* diff);

/* Check membership of string value in list. Returns 0 if false, 1 if true. */
int nccmp_exists_in_strlist(nccmp_strlist_t* list, const char* str);

/* Returns -1 if string not found, otherwise 0-base index. */
int nccmp_index_in_strlist(nccmp_strlist_t* list, const char* str);

/* Free all memory, and then sets list to NULL. */
void nccmp_free_strlist(nccmp_strlist_t** list);

/* Create new list with desired size. Returns NULL if failed. */
nccmp_strlist_t* nccmp_new_strlist(unsigned int size);

/* Dumps delimited strings to a stream of non-null items. */
void nccmp_print_strlist(FILE* stream, nccmp_strlist_t* list, char delim);

/* Get number of strings in list, which don't have to occupy every array position contiguously. */
int nccmp_compute_size_of_strlist(nccmp_strlist_t* list);

/* Parses delimited string and populates list.
   list must be pre-allocated and contents might be overwritten if not empty.
   Returns 0 if success. */
int nccmp_split_strlist_by_delim(const char* string, nccmp_strlist_t* list, const char* delim);

/* Compute string list union; 'listunion' must already be allocated to
   accommodate size(list1) + size(list2)
   does not assume 'listunion' is empty, therefore can use as a copy function.
*/
int nccmp_union_strlist(nccmp_strlist_t* list1, nccmp_strlist_t* list2, nccmp_strlist_t* listunion);

/* Returns 1 if pointer is valid and members are valid. */
int nccmp_is_strlist_valid(nccmp_strlist_t* list);

#endif
