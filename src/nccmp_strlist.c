/*
   Copyright (C) 2004-2007,2009,2010,2012-2021 Remik Ziemlinski

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

#include "log.h"
#include <nccmp_strlist.h>

int nccmp_add_to_strlist(nccmp_strlist_t* list, const char* string)
{
    return nccmp_add_to_strlist_len(list, string, strlen(string));
}

int nccmp_add_to_strlist_len(nccmp_strlist_t* list, const char* string, size_t len)
{
    int i;
    if (!nccmp_is_strlist_valid(list)) {
        return EXIT_FAILED;
    }

    for(i=0; i < list->capacity; ++i) {
        if (NULL == list->items[i]) {
            list->items[i] = XCALLOC(char, len + 1);
            if (!list->items[i]) {
                fprintf(stderr, "Failed to allocate memory for string of len = %zu", len);
                return EXIT_FAILED;
            }
            strncpy(list->items[i], string, len);
            break;
        }
    }

    if (i >= list->capacity) {
        return EXIT_FAILED;
    }

    list->size = nccmp_compute_size_of_strlist(list);

    return EXIT_SUCCESS;
}

int nccmp_append_to_strlist(nccmp_strlist_t** list, const char* string)
{
    int newcapacity;
    nccmp_strlist_t* newlist = NULL;

    if (string == NULL) {
        return EXIT_SUCCESS;
    }

    if (!nccmp_is_strlist_valid(*list)) {
        return EXIT_FAILED;
    }

    newcapacity = (*list)->capacity + 1;
    newlist = nccmp_new_strlist(newcapacity);
    nccmp_deep_copy_strlist(*list, newlist);

    newlist->items[newcapacity-1] = XMALLOC(char,strlen(string)+1);
    strcpy(newlist->items[newcapacity-1], string);

    nccmp_free_strlist(list);
    *list = newlist;
    (*list)->size = nccmp_compute_size_of_strlist(*list);

    return EXIT_SUCCESS;
}

int nccmp_clear_strlist(nccmp_strlist_t* list)
{
    int i;
    if (! nccmp_is_strlist_valid(list)) {
        return EXIT_FAILED;
    }

    for(i = 0; i < list->capacity; ++i) {
        if (NULL != list->items[i]) {
            XFREE(list->items[i]);
            list->items[i] = NULL;
        }
    }

    list->size = 0;
    return EXIT_SUCCESS;
}

int* nccmp_compute_strlist_lengths(nccmp_strlist_t* list)
{
    int *result = NULL;
    int i = 0;

    if (!list) {
        return result;
    }

    result = XCALLOC(int, list->size);
    for(i = 0; i < list->size; ++i) {
        result[i] = strlen(list->items[i]);
    }

    return result;
}

char nccmp_copy_with_prefix_strlist(nccmp_strlist_t* src, nccmp_strlist_t* dst, const char* prefix)
{
    int len = strlen(prefix);
    char found = 0;
    int i = 0;

    for( i = 0; i < src->size; ++i ) {
        if( strncmp(src->items[i], prefix, len) == 0 ) {
            nccmp_add_to_strlist(dst, src->items[i]);
            found = 1;
        }
    }

    return found;
}

char nccmp_copy_with_suffix_strlist(nccmp_strlist_t* src, nccmp_strlist_t* dst, const char* suffix)
{
    char found = 0;
    int i = 0;

    for( i = 0; i < src->size; ++i ) {
        if( nccmp_string_ends_with(src->items[i], suffix) ) {
            nccmp_add_to_strlist(dst, src->items[i]);
            found = 1;
        }
    }

    return found;
}

char nccmp_copy_with_substring_strlist(nccmp_strlist_t* src, nccmp_strlist_t* dst, const char* substring)
{
    char found = 0;
    int i = 0;

    for( i = 0; i < src->size; ++i ) {
        if( strstr(src->items[i], substring) ) {
            nccmp_add_to_strlist(dst, src->items[i]);
            found = 1;
        }
    }

    return found;
}

int nccmp_deep_copy_strlist(nccmp_strlist_t* src, nccmp_strlist_t* dst)
{
    int i, status, copied;

    if (src->size > dst->capacity) {
        return EXIT_FAILED;
    }

    status = nccmp_clear_strlist(dst);
    if (status) {
        fprintf(stderr, "Failed to clear destination list.");
        return status;
    }

    for(copied=0, i=0; i < src->capacity; ++i) {
        if (NULL == src->items[i]) {
            if (dst->items[i]) {
                XFREE(dst->items[i]);
                dst->items[i] = NULL;
            }
        } else {
            dst->items[i] = XMALLOC(char, strlen(src->items[i]) + 1);
            strcpy(dst->items[i], src->items[i]);
        }
        if (++copied >= src->size) {
            break;
        }
    }

    dst->size = src->size;

    return EXIT_SUCCESS;
}

int nccmp_diff_strlist(nccmp_strlist_t* list1, nccmp_strlist_t* list2, nccmp_strlist_t* diff)
{
    int i, status = EXIT_SUCCESS;

    if(!nccmp_is_strlist_valid(list1) || !nccmp_is_strlist_valid(list2) || !nccmp_is_strlist_valid(diff)) {
        return EXIT_FAILED;
    }

    status = nccmp_clear_strlist(diff);
    if (status) {
        return status;
    }

    if(0 == list1->size) {
        return status;
    } else if (0 == list2->size) {
        status = nccmp_deep_copy_strlist(list1, diff);
    } else {
        for(i = 0; i < list1->capacity; ++i) {
            if(  list1->items[i] &&
                (! nccmp_exists_in_strlist(list2, list1->items[i])) ) {
                status = nccmp_add_to_strlist(diff, list1->items[i]);
                if (status) {
                    break;
                }
            }
        }
    }

    diff->size = nccmp_compute_size_of_strlist(diff);

    return status;
}

void nccmp_debug_print_strlist(FILE* f, nccmp_strlist_t* list, char delim)
{
    int i;

    if (! nccmp_is_strlist_valid(list)) {
        return;
    }

    if (list->capacity > 0) {
        fprintf(f, "%s", list->items[0]);
    }

    for(i = 1; i < list->capacity; ++i) {
        fprintf(f, "%c%s", delim, list->items[i]);
    }
}

int nccmp_exists_in_strlist(nccmp_strlist_t* list, const char* str)
{
    return nccmp_index_in_strlist(list, str) != -1;
}

void nccmp_free_strlist(nccmp_strlist_t** list)
{
    int i;

    if ((NULL == *list) || !nccmp_is_strlist_valid(*list)) {
        return;
    }

    for(i = 0; i < (*list)->capacity; ++i) {
        XFREE( (*list)->items[i] );
    }

    XFREE( (*list)->items );
    XFREE(*list);
}

int nccmp_index_in_strlist(nccmp_strlist_t* list, const char* str)
{
    int i;
    if (! nccmp_is_strlist_valid(list) || (NULL == str)) {
        return -1;
    }

    for(i=0; i < list->size; ++i)
    {
        if (NULL == list->items[i]) {
            continue;
        }

        if(strcmp(list->items[i], str) == 0) {
            return i;
        }
    }

    for(i = list->size; i < list->capacity; ++i)
    {
        if (NULL == list->items[i]) {
            continue;
        }

        if(strcmp(list->items[i], str) == 0) {
            return i;
        }
    }

    return -1;
}

nccmp_strlist_t* nccmp_new_strlist(unsigned int size)
{
    nccmp_strlist_t * list = XMALLOC(nccmp_strlist_t, 1);
    int i;

    if (!list) {
        return NULL;
    }

    list->items = XMALLOC(char*, size);
    if (! list->items) {
        XFREE(list);
        return NULL;
    }

    for(i = 0; i < size; ++i) {
        list->items[i] = NULL;
    }

    list->capacity = size;
    list->size = 0;

    return list;
}

void nccmp_print_strlist(FILE* f, nccmp_strlist_t* list, char delim)
{
    int i;

    if (! nccmp_is_strlist_valid(list)) {
        return;
    }

    if (list->size > 0) {
        fprintf(f, "%s", list->items[0]);
    }

    for(i = 1; i < list->size; ++i) {
        fprintf(f, "%c%s", delim, list->items[i]);
    }
}

int nccmp_compute_size_of_strlist(nccmp_strlist_t* list)
{
    int n = 0;
    int i;

    for(i = 0; i < list->capacity; ++i) {
        if ((NULL != list->items[i]) && (strlen(list->items[i]) > 0 )) {
            ++n;
        }
    }

    return n;
}

int nccmp_split_strlist_by_delim(const char* string, nccmp_strlist_t* list, const char* delim)
{
    const char *first = string, *last;
    int status = EXIT_SUCCESS;
    int len;

    if (! first) {
        return status;
    }

    while ( (last = strpbrk(first, delim)) )
    {
        len = last - first;
        if (len > 0) {
            status = nccmp_add_to_strlist_len(list, first, len);
            if (EXIT_SUCCESS != status) {
                break;
            }
        }
        first = last + 1;
    }

    len = strlen(first); /* String remainder. */
    if (len > 0) {
        status = nccmp_add_to_strlist_len(list, first, len);
    }

    return status;
}

char nccmp_string_ends_with(const char* string, const char* suffix)
{
    if (!string || !suffix) {
        return 0;
    }

    size_t lenstring = strlen(string);
    size_t lensuffix = strlen(suffix);

    if (lensuffix > lenstring) {
        return 0;
    }

    return strncmp(string + lenstring - lensuffix, suffix, lensuffix) == 0;
}

int nccmp_union_strlist(nccmp_strlist_t* list1, nccmp_strlist_t* list2, nccmp_strlist_t* listunion)
{
    int i;
    if (!nccmp_is_strlist_valid(listunion)) {
        return EXIT_FAILED;
    }

    if (nccmp_is_strlist_valid(list1)) {
        for(i = 0; i < list1->capacity; ++i)
        {
            if (NULL == list1->items[i]) {
                continue;
            }

            if (! nccmp_exists_in_strlist(listunion, list1->items[i]) ) {
                nccmp_add_to_strlist(listunion, list1->items[i]);
            }
        }
    }

    if (nccmp_is_strlist_valid(list2)) {
        for(i = 0; i < list2->capacity; ++i)
        {
            if (NULL == list2->items[i]) {
                continue;
            }

            if (! nccmp_exists_in_strlist(listunion, list2->items[i]) ) {
                nccmp_add_to_strlist(listunion, list2->items[i]);
            }
        }
    }

    listunion->size = nccmp_compute_size_of_strlist(listunion);

    return EXIT_SUCCESS;
}


int nccmp_is_strlist_valid(nccmp_strlist_t* list)
{
    if (NULL == list) {
        return 0;
    }

    if (NULL == list->items) {
        return 0;
    }

    return 1;
}
