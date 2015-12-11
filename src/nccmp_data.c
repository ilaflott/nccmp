#include <nccmp_constants.h>
#include <nccmp_data.h>
#include <nccmp_error.h>
#include <nccmp_format.h>
#include <nccmp_io.h>
#include <nccmp_odometer.h>
#include <nccmp_thread.h>
#include <math.h>
#include "nccmp_nest.h"

PRINT_SETUP()

/* Element compare and return first offset that differs.
   Arrays should have sentinals at end that are guaranteed to differ.
*/
#define CMP_TYPES(NAME1, TYPE1, NAME2, TYPE2)                                  \
size_t nccmp_cmp_##NAME1##_##NAME2(TYPE1* in1, TYPE2* in2) {                    \
    TYPE1 const *p1 = in1;                                                     \
    TYPE2 const *p2 = in2;                                                     \
    for(; *p1 == *p2; ++p1, ++p2) { };                                         \
    return p1 - in1;                                                           \
}
#define CMP_TYPES_MISSING(NAME1, TYPE1, NAME2, TYPE2)                          \
size_t nccmp_cmp_##NAME1##_##NAME2##_missing(TYPE1* in1, TYPE2* in2, TYPE1 m1, TYPE2 m2) {\
    TYPE1 const *p1 = in1;                                                     \
    TYPE2 const *p2 = in2;                                                     \
    for(; (*p1 == *p2) || ((*p1 == m1) && (*p2 == m2)); ++p1, ++p2) { };       \
    return p1 - in1;                                                           \
}
#define CMP_TYPES_NANEQUAL(NAME1, TYPE1, NAME2, TYPE2)                         \
size_t nccmp_cmp_##NAME1##_##NAME2##_nanequal(TYPE1* in1, TYPE2* in2) {         \
    TYPE1 const *p1 = in1;                                                     \
    TYPE2 const *p2 = in2;                                                     \
    for(; (*p1 == *p2) || (isnan(*p1) && isnan(*p2)); ++p1, ++p2) { };         \
    return p1 - in1;                                                           \
}
#define CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, NAME2, TYPE2)                 \
size_t nccmp_cmp_##NAME1##_##NAME2##_missing_nanequal(TYPE1* in1, TYPE2* in2, TYPE1 m1, TYPE2 m2) { \
    TYPE1 const *p1 = in1;                                                     \
    TYPE2 const *p2 = in2;                                                     \
    for(; (*p1 == *p2) || ((*p1 == m1) && (*p2 == m2)) || (isnan(*p1) && isnan(*p2)); ++p1, ++p2) { };       \
    return p1 - in1;                                                           \
}
#define CMP_TYPES_FIELD(NAME1, TYPE1, NAME2, TYPE2)                            \
size_t nccmp_cmp_field_##NAME1##_##NAME2(                                       \
        void *in1, size_t field_off1, size_t size1,                            \
        void *in2, size_t field_off2, size_t size2,                            \
        size_t first, size_t nitems) {                                          \
    unsigned char const *p1 = in1;                                             \
    unsigned char const *p2 = in2;                                             \
    size_t i = first;                                                           \
    for(; i < nitems; ++i) {                                                   \
        if ( *(TYPE1*)(p1 + size1 * i + field_off1) !=                         \
             *(TYPE2*)(p2 + size2 * i + field_off2) ) {                        \
            break;                                                             \
        }                                                                      \
    };                                                                         \
    return i;                                                                  \
}
#define CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, NAME2, TYPE2)                   \
size_t nccmp_cmp_field_##NAME1##_##NAME2##_nanequal(                            \
        void *in1, size_t field_off1, size_t size1,                            \
        void *in2, size_t field_off2, size_t size2,                            \
        size_t first, size_t nitems) {                                          \
    unsigned char const *p1 = in1;                                             \
    unsigned char const *p2 = in2;                                             \
    TYPE1 item1;                                                               \
    TYPE2 item2;                                                               \
    int i = first;                                                             \
    for(; i < nitems; ++i) {                                                   \
        item1 = *(TYPE1*)(p1 + size1*i + field_off1);                          \
        item2 = *(TYPE2*)(p2 + size2*i + field_off2);                          \
        if ( (item1 != item2) && !isnan(item1) && !isnan(item2) ) {            \
            break;                                                             \
        }                                                                      \
    };                                                                         \
    return i;                                                                  \
}
#define CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, NAME2, TYPE2)                      \
size_t nccmp_cmp_field_array_##NAME1##_##NAME2(                                 \
        void *in1, size_t field_off1, size_t comp_size1,                       \
        void *in2, size_t field_off2, size_t comp_size2, size_t array_offset,   \
        size_t nitems, size_t first_item) {                                    \
    unsigned char const *p1 = (unsigned char*)in1 + comp_size1 * array_offset; \
    unsigned char const *p2 = (unsigned char*)in2 + comp_size2 * array_offset; \
    int i = first_item;                                                        \
    for(; i < nitems; ++i) {                                                   \
        if ( *((TYPE1*)(p1 + field_off1) + i) != *((TYPE2*)(p2 + field_off2) + i) ) {    \
            break;                                                             \
        }                                                                      \
    };                                                                         \
    return i;                                                                  \
}
#define CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, NAME2, TYPE2)             \
size_t nccmp_cmp_field_array_##NAME1##_##NAME2##_nanequal(                      \
        void *in1, size_t field_off1, size_t comp_size1,                       \
        void *in2, size_t field_off2, size_t comp_size2, size_t array_offset,   \
        size_t nitems, size_t first_item) {                                    \
    unsigned char const *p1 = (unsigned char*)in1 + comp_size1 * array_offset; \
    unsigned char const *p2 = (unsigned char*)in2 + comp_size2 * array_offset; \
    TYPE1 item1;                                                               \
    TYPE2 item2;                                                               \
    int i = first_item;                                                        \
    for(; i < nitems; ++i) {                                                   \
        item1 = *((TYPE1*)(p1 + field_off1) + i);                              \
        item2 = *((TYPE2*)(p2 + field_off2) + i);                              \
        if ( (item1 != item2) && !isnan(item1) && !isnan(item2) ) {            \
            break;                                                             \
        }                                                                      \
    };                                                                         \
    return i;                                                                  \
}

#define CMP_TYPES_GEN(NAME1, TYPE1)                                            \
    CMP_TYPES(NAME1, TYPE1, text,       char)                                  \
    CMP_TYPES(NAME1, TYPE1, schar,      signed char)                           \
    CMP_TYPES(NAME1, TYPE1, uchar,      unsigned char)                         \
    CMP_TYPES(NAME1, TYPE1, short,      short)                                 \
    CMP_TYPES(NAME1, TYPE1, ushort,     unsigned short)                        \
    CMP_TYPES(NAME1, TYPE1, int,        int)                                   \
    CMP_TYPES(NAME1, TYPE1, uint,       unsigned int)                          \
    CMP_TYPES(NAME1, TYPE1, longlong,   long long)                             \
    CMP_TYPES(NAME1, TYPE1, ulonglong,  unsigned long long)                    \
    CMP_TYPES(NAME1, TYPE1, double,     double)                                \
    CMP_TYPES(NAME1, TYPE1, float,      float)
#define CMP_TYPES_MISSING_GEN(NAME1, TYPE1)                                    \
    CMP_TYPES_MISSING(NAME1, TYPE1, text,       char)                          \
    CMP_TYPES_MISSING(NAME1, TYPE1, schar,      signed char)                   \
    CMP_TYPES_MISSING(NAME1, TYPE1, uchar,      unsigned char)                 \
    CMP_TYPES_MISSING(NAME1, TYPE1, short,      short)                         \
    CMP_TYPES_MISSING(NAME1, TYPE1, ushort,     unsigned short)                \
    CMP_TYPES_MISSING(NAME1, TYPE1, int,        int)                           \
    CMP_TYPES_MISSING(NAME1, TYPE1, uint,       unsigned int)                  \
    CMP_TYPES_MISSING(NAME1, TYPE1, longlong,   long long)                     \
    CMP_TYPES_MISSING(NAME1, TYPE1, ulonglong,  unsigned long long)            \
    CMP_TYPES_MISSING(NAME1, TYPE1, double,     double)                        \
    CMP_TYPES_MISSING(NAME1, TYPE1, float,      float)
#define CMP_TYPES_NANEQUAL_GEN(NAME1, TYPE1)                                   \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, text,      char)                          \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, schar,     signed char)                   \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, uchar,     unsigned char)                 \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, short,     short)                         \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, ushort,    unsigned short)                \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, int,       int)                           \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, uint,      unsigned int)                  \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, longlong,  long long)                     \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, ulonglong, unsigned long long)            \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, double,    double)                        \
    CMP_TYPES_NANEQUAL(NAME1, TYPE1, float,     float)
#define CMP_TYPES_MISSING_NANEQUAL_GEN(NAME1, TYPE1)                           \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, text,      char)                  \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, schar,     signed char)           \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, uchar,     unsigned char)         \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, short,     short)                 \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, ushort,    unsigned short)        \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, int,       int)                   \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, uint,      unsigned int)          \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, longlong,  long long)             \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, ulonglong, unsigned long long)    \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, double,    double)                \
    CMP_TYPES_MISSING_NANEQUAL(NAME1, TYPE1, float,     float)
#define CMP_TYPES_FIELD_GEN(NAME1, TYPE1)                                      \
    CMP_TYPES_FIELD(NAME1, TYPE1, text,      char)                             \
    CMP_TYPES_FIELD(NAME1, TYPE1, schar,     signed char)                      \
    CMP_TYPES_FIELD(NAME1, TYPE1, uchar,     unsigned char)                    \
    CMP_TYPES_FIELD(NAME1, TYPE1, short,     short)                            \
    CMP_TYPES_FIELD(NAME1, TYPE1, ushort,    unsigned short)                   \
    CMP_TYPES_FIELD(NAME1, TYPE1, int,       int)                              \
    CMP_TYPES_FIELD(NAME1, TYPE1, uint,      unsigned int)                     \
    CMP_TYPES_FIELD(NAME1, TYPE1, longlong,  long long)                        \
    CMP_TYPES_FIELD(NAME1, TYPE1, ulonglong, unsigned long long)               \
    CMP_TYPES_FIELD(NAME1, TYPE1, double,    double)                           \
    CMP_TYPES_FIELD(NAME1, TYPE1, float,     float) 
#define CMP_TYPES_FIELD_NANEQUAL_GEN(NAME1, TYPE1)                             \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, text,      char)                    \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, schar,     signed char)             \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, uchar,     unsigned char)           \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, short,     short)                   \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, ushort,    unsigned short)          \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, int,       int)                     \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, uint,      unsigned int)            \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, longlong,  long long)               \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, ulonglong, unsigned long long)      \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, double,    double)                  \
    CMP_TYPES_FIELD_NANEQUAL(NAME1, TYPE1, float,     float)
#define CMP_TYPES_FIELD_ARRAY_GEN(NAME1, TYPE1)                                \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, text,      char)                       \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, schar,     signed char)                \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, uchar,     unsigned char)              \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, short,     short)                      \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, ushort,    unsigned short)             \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, int,       int)                        \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, uint,      unsigned int)               \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, longlong,  long long)                  \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, ulonglong, unsigned long long)         \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, double,    double)                     \
    CMP_TYPES_FIELD_ARRAY(NAME1, TYPE1, float,     float)
#define CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(NAME1, TYPE1)                       \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, text,      char)              \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, schar,     signed char)       \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, uchar,     unsigned char)     \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, short,     short)             \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, ushort,    unsigned short)    \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, int,       int)               \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, uint,      unsigned int)      \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, longlong,  long long)         \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, ulonglong, unsigned long long)\
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, double,    double)            \
    CMP_TYPES_FIELD_ARRAY_NANEQUAL(NAME1, TYPE1, float,     float)

CMP_TYPES_GEN(double,   double)
CMP_TYPES_GEN(float,    float)
CMP_TYPES_GEN(longlong, long long)
CMP_TYPES_GEN(int,      int)
CMP_TYPES_GEN(short,    short)
CMP_TYPES_GEN(text,     char)
CMP_TYPES_GEN(schar,    signed char)
CMP_TYPES_GEN(uchar,    unsigned char)
CMP_TYPES_GEN(uint,     unsigned int)
CMP_TYPES_GEN(ulonglong,unsigned long long)
CMP_TYPES_GEN(ushort,   unsigned short)
CMP_TYPES_MISSING_GEN(double,   double)
CMP_TYPES_MISSING_GEN(float,    float)
CMP_TYPES_MISSING_GEN(int,      int)
CMP_TYPES_MISSING_GEN(longlong, long long)
CMP_TYPES_MISSING_GEN(short,    short)
CMP_TYPES_MISSING_GEN(text,     char)
CMP_TYPES_MISSING_GEN(schar,    signed char)
CMP_TYPES_MISSING_GEN(uchar,    unsigned char)
CMP_TYPES_MISSING_GEN(uint,     unsigned int)
CMP_TYPES_MISSING_GEN(ulonglong,unsigned long long)
CMP_TYPES_MISSING_GEN(ushort,   unsigned short)
CMP_TYPES_MISSING_NANEQUAL_GEN(double,   double)
CMP_TYPES_MISSING_NANEQUAL_GEN(float,    float)
CMP_TYPES_MISSING_NANEQUAL_GEN(int,      int)
CMP_TYPES_MISSING_NANEQUAL_GEN(longlong, long long)
CMP_TYPES_MISSING_NANEQUAL_GEN(short,    short)
CMP_TYPES_MISSING_NANEQUAL_GEN(text,     char)
CMP_TYPES_MISSING_NANEQUAL_GEN(schar,    signed char)
CMP_TYPES_MISSING_NANEQUAL_GEN(uchar,    unsigned char)
CMP_TYPES_MISSING_NANEQUAL_GEN(uint,     unsigned int)
CMP_TYPES_MISSING_NANEQUAL_GEN(ulonglong,unsigned long long)
CMP_TYPES_MISSING_NANEQUAL_GEN(ushort,   unsigned short)
CMP_TYPES_NANEQUAL_GEN(double,   double)
CMP_TYPES_NANEQUAL_GEN(float,    float)
CMP_TYPES_NANEQUAL_GEN(int,      int)
CMP_TYPES_NANEQUAL_GEN(longlong, long long)
CMP_TYPES_NANEQUAL_GEN(short,    short)
CMP_TYPES_NANEQUAL_GEN(text,     char)
CMP_TYPES_NANEQUAL_GEN(schar,    signed char)
CMP_TYPES_NANEQUAL_GEN(uchar,    unsigned char)
CMP_TYPES_NANEQUAL_GEN(uint,     unsigned int)
CMP_TYPES_NANEQUAL_GEN(ulonglong,unsigned long long)
CMP_TYPES_NANEQUAL_GEN(ushort,   unsigned short)
CMP_TYPES_FIELD_GEN(double,   double)
CMP_TYPES_FIELD_GEN(float,    float)
CMP_TYPES_FIELD_GEN(longlong, long long)
CMP_TYPES_FIELD_GEN(int,      int)
CMP_TYPES_FIELD_GEN(short,    short)
CMP_TYPES_FIELD_GEN(text,     char)
CMP_TYPES_FIELD_GEN(schar,    signed char)
CMP_TYPES_FIELD_GEN(uchar,    unsigned char)
CMP_TYPES_FIELD_GEN(uint,     unsigned int)
CMP_TYPES_FIELD_GEN(ulonglong,unsigned long long)
CMP_TYPES_FIELD_GEN(ushort,   unsigned short)
CMP_TYPES_FIELD_NANEQUAL_GEN(double,   double)
CMP_TYPES_FIELD_NANEQUAL_GEN(float,    float)
CMP_TYPES_FIELD_NANEQUAL_GEN(int,      int)
CMP_TYPES_FIELD_NANEQUAL_GEN(longlong, long long)
CMP_TYPES_FIELD_NANEQUAL_GEN(short,    short)
CMP_TYPES_FIELD_NANEQUAL_GEN(text,     char)
CMP_TYPES_FIELD_NANEQUAL_GEN(schar,    signed char)
CMP_TYPES_FIELD_NANEQUAL_GEN(uchar,    unsigned char)
CMP_TYPES_FIELD_NANEQUAL_GEN(uint,     unsigned int)
CMP_TYPES_FIELD_NANEQUAL_GEN(ulonglong,unsigned long long)
CMP_TYPES_FIELD_NANEQUAL_GEN(ushort,   unsigned short)
CMP_TYPES_FIELD_ARRAY_GEN(double,   double)
CMP_TYPES_FIELD_ARRAY_GEN(float,    float)
CMP_TYPES_FIELD_ARRAY_GEN(longlong, long long)
CMP_TYPES_FIELD_ARRAY_GEN(int,      int)
CMP_TYPES_FIELD_ARRAY_GEN(short,    short)
CMP_TYPES_FIELD_ARRAY_GEN(text,     char)
CMP_TYPES_FIELD_ARRAY_GEN(schar,    signed char)
CMP_TYPES_FIELD_ARRAY_GEN(uchar,    unsigned char)
CMP_TYPES_FIELD_ARRAY_GEN(uint,     unsigned int)
CMP_TYPES_FIELD_ARRAY_GEN(ulonglong,unsigned long long)
CMP_TYPES_FIELD_ARRAY_GEN(ushort,   unsigned short)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(double,   double)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(float,    float)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(longlong, long long)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(int,      int)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(short,    short)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(text,     char)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(schar,    signed char)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(uchar,    unsigned char)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(uint,     unsigned int)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(ulonglong,unsigned long long)
CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN(ushort,   unsigned short)

#undef CMP_TYPES
#undef CMP_TYPES_MISSING
#undef CMP_TYPES_MISSING_NANEQUAL
#undef CMP_TYPES_NANEQUAL
#undef CMP_TYPES_GEN
#undef CMP_TYPES_MISSING_GEN
#undef CMP_TYPES_MISSING_NANEQUAL_GEN
#undef CMP_TYPES_NANEQUAL_GEN
#undef CMP_TYPES_FIELD
#undef CMP_TYPES_FIELD_GEN
#undef CMP_TYPES_FIELD_NANEQUAL
#undef CMP_TYPES_FIELD_NANEQUAL_GEN
#undef CMP_TYPES_FIELD_ARRAY
#undef CMP_TYPES_FIELD_ARRAY_GEN
#undef CMP_TYPES_FIELD_ARRAY_NANEQUAL
#undef CMP_TYPES_FIELD_ARRAY_NANEQUAL_GEN

#define COMPOUND_FIELD_OFFSET(COMP_SIZE, COMP_ITER, FIELD_OFFSET)              \
    (COMP_SIZE * COMP_ITER) + FIELD_OFFSET

#define COMPOUND_FIELD_PTR(PTR, COMP_SIZE, FIELD_OFFSET, CAST_TYPE, COMP_ITER) \
    (CAST_TYPE*)( ((unsigned char*)PTR) + COMPOUND_FIELD_OFFSET(COMP_SIZE, COMP_ITER, FIELD_OFFSET) )

#define COMPOUND_FIELD_VALUE(PTR, COMP_SIZE, FIELD_OFFSET, CAST_TYPE, COMP_ITER)         \
    *COMPOUND_FIELD_PTR(PTR, COMP_SIZE, FIELD_OFFSET, CAST_TYPE, COMP_ITER)

#define PRINT_COMPOUND_FIELD_VALUES(PTR, COMP_SIZE, FIELD_OFFSET, TYPE, NAME, ITER, STR, N)        \
    for(ITER = 0; ITER < N; ++ITER) {                                          \
        nccmp_##NAME##_to_str(COMPOUND_FIELD_VALUE(PTR, COMP_SIZE, FIELD_OFFSET, TYPE, ITER), STR, state->opts.precision);       \
        LOG_DEBUG("%s\n", STR);                                                \
    }

#define COMPOUND_FIELD_ARRAY_VALUE(PTR, COMP_SIZE, FIELD_OFFSET, CAST_TYPE, COMP_OFFSET, ITEM_OFFSET)        \
    *((CAST_TYPE*)( ((unsigned char*)PTR) + (COMP_SIZE * COMP_OFFSET) + FIELD_OFFSET ) + ITEM_OFFSET)

int nccmp_cmp_var_string(nccmp_state_t* state, int ncid1, int ncid2,
                   int idx1, int idx2, size_t* start, size_t* count,
                   size_t nitems, int rec, size_t* limits)
{
    char **s1, **s2;
    char idx_str[256];
    int i, status;
    int loop_done = 0;
    count_limit_type diff_count_status;
    int last_status = EXIT_SUCCESS;
    nccmp_var_t *v1, *v2;

    v1 = & state->vars1[idx1];
    v2 = & state->vars2[idx2];

    /* Library will allocate each string, we just provide pointers. */
    s1 = XMALLOC(char*, nitems);
    s2 = XMALLOC(char*, nitems);

    while(! loop_done) {
        status = nc_get_vara_string(ncid1, v1->varid, start, count, s1);
        HANDLE_NC_ERROR(status);
        status = nc_get_vara_string(ncid2, v2->varid, start, count, s2);
        HANDLE_NC_ERROR(status);

        for(i = 0; i < nitems; ++i) {
            if ( ( s1[i] && !s2[i]) ||
                 (!s1[i] &&  s2[i]) ||
                 (s1[i] && s2[i] && strcmp(s1[i], s2[i]))
               ) {
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }


                diff_count_status = nccmp_inc_diff_count(state, idx1);
                if (COUNT_OVER == diff_count_status) {
                    goto recover;
                }
                if (!state->opts.quiet) {
                    nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);
                    PRINT_DIFF(state->opts.debug, DIFF_MSG, v1->name, idx_str, s1[i], s2[i]);
                }
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force ) {
                    goto recover;
                }
            }
        }

        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;
    }

recover:
    if (s1) {
		status = nc_free_string(nitems, s1);
        HANDLE_NC_ERROR(status);
        XFREE(s1);
    }
    if (s2) {
		status = nc_free_string(nitems, s2);
        HANDLE_NC_ERROR(status);
        XFREE(s2);
    }

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_array_atomic_type_string(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int narrays, int rec, size_t* limits,
        void* items1, void* items2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int status, last_status = EXIT_SUCCESS;
    size_t array_size = 0;
    int array_i, i;
    char **strings1, **strings2;
    char array_idx_str[64], root_idx_str[64], idx_str[64];
    char field_path_str[NC_MAX_NAME];
    count_limit_type diff_count_status;
    nccmp_var_t *v1;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_array_atomic_type_string\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        goto recover;
    }

    v1 = & state->vars1[varid1];
    array_size = (size_t) nccmp_product_int(type1->fields->items[fieldid1].dim_sizes,
                                      0, type1->fields->items[fieldid1].num_dims-1);

    for(array_i = 0; array_i < narrays; ++array_i) {
        strings1 = (char**) ((unsigned char*)items1 + type1->root_size * array_i + type1->fields->items[fieldid1].offset);
        strings2 = (char**) ((unsigned char*)items2 + type2->root_size * array_i + type2->fields->items[fieldid2].offset);
        for(i = 0; i < array_size; ++i) {
            if ( ( strings1[i] && !strings2[i]) ||
                 (!strings1[i] &&  strings2[i]) ||
                 (strings1[i] && strings2[i] && strcmp(strings1[i], strings2[i])) )
            {
                if (!state->opts.warn[NCCMP_W_ALL]) {
                  last_status = EXIT_DIFFER;
                }
                diff_count_status = nccmp_inc_diff_count(state, varid1);
                if (COUNT_OVER == diff_count_status) {
                    goto recover;
                }
                if (!state->opts.quiet) {
                    nccmp_get_index_str_size_t(v1->ndims, start, array_i, root_idx_str,
                                         state->opts.fortran);
                    nccmp_get_product_index_str_int(type1->fields->items[fieldid1].num_dims,
                                              type1->fields->items[fieldid1].dim_sizes,
                                              i, array_idx_str,
                                              state->opts.fortran);
                    if (state->opts.fortran) {
                        sprintf(idx_str, "%s,%s", array_idx_str, root_idx_str);
                    } else {
                        sprintf(idx_str, "%s,%s", root_idx_str, array_idx_str);
                    }
                    field_path_str[0] = 0;
                    nccmp_build_user_type_field_path_str(& type1->fields->items[fieldid1], field_path_str);
                    PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD, v1->name, idx_str,
                               field_path_str, strings1[i], strings2[i]);
                }
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
                    goto recover;
                }
            }
        }
    }

recover:
    // Must free strings memory allocated by api.
    for(array_i = 0; array_i < narrays; ++array_i) {
        strings1 = (char**) ((unsigned char*)items1 + type1->root_size * array_i + type1->fields->items[fieldid1].offset);
        strings2 = (char**) ((unsigned char*)items2 + type2->root_size * array_i + type2->fields->items[fieldid2].offset);
		status = nc_free_string(array_size, strings1);
        HANDLE_NC_ERROR(status);
		status = nc_free_string(array_size, strings2);
        HANDLE_NC_ERROR(status);
    }

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_array_atomic_type(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int narrays, int rec, size_t* limits,
        void* items1, void* items2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    char print_hex = nccmp_is_precision_hex(& state->opts);
    count_limit_type diff_count_status;
    nccmp_var_t *v1 = & state->vars1[varid1];
    size_t diff_offset, item_offset;
    int array_i;
    char field_path_str[NC_MAX_NAME];
    size_t array_size = (size_t) nccmp_product_int(type1->fields->items[fieldid1].dim_sizes,
                            0, type1->fields->items[fieldid1].num_dims-1);
    char array_idxstr[32], root_idxstr[32], idx_str[32], str1[32], str2[32];
    nccmp_nc_type_t item1, item2;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_array_atomic_type\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        goto recover;
    }

#define CMP_FIELD_ATOMIC_ARRAY(NAME1, TYPE1, NAME2, TYPE2, NANEQUAL)           \
    for(array_i = 0; array_i < narrays; ++array_i) {                           \
        item_offset = 0;                                                       \
        while(1) {                                                             \
            if (NANEQUAL) {                                                    \
                diff_offset = nccmp_cmp_field_array_##NAME1##_##NAME2##_nanequal(        \
                                items1, type1->fields->items[fieldid1].offset, type1->root_size,   \
                                items2, type2->fields->items[fieldid2].offset, type2->root_size,   \
                                array_i, array_size, item_offset);             \
            } else {                                                           \
                diff_offset = nccmp_cmp_field_array_##NAME1##_##NAME2(         \
                                items1, type1->fields->items[fieldid1].offset, type1->root_size,   \
                                items2, type2->fields->items[fieldid2].offset, type2->root_size,   \
                                array_i, array_size, item_offset);             \
            }                                                                  \
            if (diff_offset >= array_size) {                                   \
                break;                                                         \
            }                                                                  \
            item_offset = diff_offset + 1;                                     \
            if (!state->opts.warn[NCCMP_W_ALL]) {                              \
              last_status = EXIT_DIFFER;                                       \
            } else {                                                           \
              last_status = EXIT_SUCCESS;                                      \
            }                                                                  \
            diff_count_status = nccmp_inc_diff_count(state, varid1);           \
            if (COUNT_OVER == diff_count_status) {                             \
                goto recover;                                                  \
            }                                                                  \
            if (!state->opts.quiet) {                                          \
                nccmp_get_index_str_size_t(v1->ndims, start, array_i, root_idxstr,           \
                                     state->opts.fortran);                                   \
                nccmp_get_product_index_str_int(type1->fields->items[fieldid1].num_dims,     \
                                          type1->fields->items[fieldid1].dim_sizes,          \
                                          diff_offset, array_idxstr,                         \
                                          state->opts.fortran);                              \
                if (state->opts.fortran) {                                                   \
                    sprintf(idx_str, "%s,%s", array_idxstr, root_idxstr);                    \
                } else {                                                                     \
                    sprintf(idx_str, "%s,%s", root_idxstr, array_idxstr);                    \
                }                                                                            \
                item1.m_##NAME1 = COMPOUND_FIELD_ARRAY_VALUE(items1, type1->root_size,       \
                    type1->fields->items[fieldid1].offset, TYPE1, array_i, diff_offset);     \
                item2.m_##NAME2 = COMPOUND_FIELD_ARRAY_VALUE(items2, type2->root_size,       \
                    type2->fields->items[fieldid2].offset, TYPE2, array_i, diff_offset);     \
                if (print_hex) {                                                             \
                    nccmp_##NAME1##_to_hex(item1.m_##NAME1, str1);                           \
                    nccmp_##NAME2##_to_hex(item2.m_##NAME2, str2);                           \
                } else {                                                                     \
                    nccmp_##NAME1##_to_str(item1.m_##NAME1, str1, state->opts.precision);    \
                    nccmp_##NAME2##_to_str(item2.m_##NAME2, str2, state->opts.precision);    \
                }                                                                            \
                field_path_str[0] = 0;                                                       \
                nccmp_build_user_type_field_path_str(& type1->fields->items[fieldid1], field_path_str);\
                PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD, v1->name, idx_str,             \
                      field_path_str, str1, str2);                                           \
            }                                                                                \
            if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {                 \
                goto recover;                                                                \
            }                                                                                \
        }                                                                                    \
    }

#define CMP_FIELD_ATOMIC_ARRAY_SELECT(...)                                     \
    if (state->opts.tolerance) {                                               \
        /* TODO: CMP_FIELD_ATOMIC_ARRAY_TOL(__VA_ARGS__);                   */ \
    } else {                                                                   \
        CMP_FIELD_ATOMIC_ARRAY(__VA_ARGS__);                                   \
    }

#define CMP_FIELD_ATOMIC_ARRAY_TYPE(NAME1, TYPE1, NANEQUAL)                    \
    switch(type2->fields->items[fieldid2].type_id) {                           \
    case NC_BYTE:   CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, schar,     signed char,       NANEQUAL);      break;   \
    case NC_CHAR:   CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, text,      char,              NANEQUAL);      break;   \
    case NC_DOUBLE: CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, double,    double,            state->opts.nanequal);break;       \
    case NC_FLOAT:  CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, float,     float,             state->opts.nanequal);break;       \
    case NC_INT:    CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, int,       int,               NANEQUAL);      break;   \
    case NC_INT64:  CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, longlong,  long long,         NANEQUAL);      break;   \
    case NC_SHORT:  CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, short,     short,             NANEQUAL);      break;   \
    case NC_UBYTE:  CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, uchar,     unsigned char,     NANEQUAL);      break;   \
    case NC_UINT:   CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, uint,      unsigned int,      NANEQUAL);      break;   \
    case NC_UINT64: CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, ulonglong, unsigned long long,NANEQUAL);      break;   \
    case NC_USHORT: CMP_FIELD_ATOMIC_ARRAY_SELECT(NAME1, TYPE1, ushort,    unsigned short,    NANEQUAL);      break;   \
    case NC_STRING:                                                            \
    default:                                                                   \
         LOG_ERROR("Unsupported second var field %s data type = %d\n",         \
                 state->types2->items[state->vars2[varid2].type].fields->items[fieldid2].name,     \
                 state->types2->items[state->vars2[varid2].type].fields->items[fieldid2].type_id); \
         last_status = EXIT_FAILED;                                            \
         break;                                                                \
    }

    switch(type1->fields->items[fieldid1].type_id) {
    case NC_BYTE:   CMP_FIELD_ATOMIC_ARRAY_TYPE(schar,    signed char,        0);             break;
    case NC_CHAR:   CMP_FIELD_ATOMIC_ARRAY_TYPE(text,     char,               0);             break;
    case NC_DOUBLE: CMP_FIELD_ATOMIC_ARRAY_TYPE(double,   double,             state->opts.nanequal);break;
    case NC_FLOAT:  CMP_FIELD_ATOMIC_ARRAY_TYPE(float,    float,              state->opts.nanequal);break;
    case NC_INT:    CMP_FIELD_ATOMIC_ARRAY_TYPE(int,      int,                0);             break;
    case NC_INT64:  CMP_FIELD_ATOMIC_ARRAY_TYPE(longlong, long long,          0);             break;
    case NC_SHORT:  CMP_FIELD_ATOMIC_ARRAY_TYPE(short,    short,              0);             break;
    case NC_UBYTE:  CMP_FIELD_ATOMIC_ARRAY_TYPE(uchar,    unsigned char,      0);             break;
    case NC_UINT:   CMP_FIELD_ATOMIC_ARRAY_TYPE(uint,     unsigned int,       0);             break;
    case NC_UINT64: CMP_FIELD_ATOMIC_ARRAY_TYPE(ulonglong,unsigned long long, 0);             break;
    case NC_USHORT: CMP_FIELD_ATOMIC_ARRAY_TYPE(ushort,   unsigned short,     0);             break;
    case NC_STRING:
        if (NC_STRING == type2->fields->items[fieldid2].type_id) {
            last_status = nccmp_cmp_var_user_type_compound_field_array_atomic_type_string(
                            state, ncid1, ncid2, varid1, varid2,
                            start, count, narrays, rec, limits,
                            items1, items2, type1, type2, fieldid1, fieldid2);
            break;
        }
    default:
        LOG_ERROR("Unsupported first var field %s data type = %d\n",
                type1->fields->items[fieldid1].name,
                type1->fields->items[fieldid1].type_id);
        last_status = EXIT_FAILED;
        break;
    }

#undef CMP_FIELD_ATOMIC_ARRAY
#undef CMP_FIELD_ATOMIC_ARRAY_TYPE
#undef CMP_FIELD_ATOMIC_ARRAY_SELECT

recover:
    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_array_user_type_compound(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int n_parent_comps, int rec, size_t* limits,
        void* data1, void* data2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    nccmp_nest_t *nest = 0;
    nccmp_nest_t *nest_next = 0;
    int parent_comp_i, array_size, i;
    void *child_comps1 = 0, *child_comps2 = 0;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_array_user_type_compound\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        goto recover;
    }

    nest = nccmp_create_nest();
    nest_next = nccmp_create_nest();

    // The root compound types.
    nest->type1 = type1;
    nest->type2 = type2;

    // Set the field comp types.
    nest_next->type1 = & type1->fields->items[fieldid1];
    nest_next->type2 = & type2->fields->items[fieldid2];
    nest_next->prev = nest;
    nest_next->num_dims = nest_next->type1->num_dims;

    array_size = nccmp_product_int(type1->fields->items[fieldid1].dim_sizes,
                            0, type1->fields->items[fieldid1].num_dims-1);

    for(parent_comp_i = 0; parent_comp_i < n_parent_comps; ++parent_comp_i) {
        for(i = 0; i < array_size; ++i) {
            nccmp_get_product_index_inverse_array_int(
                    nest_next->type1->num_dims,
                    nest_next->type1->dim_sizes,
                    i,
                    nest_next->index);
            child_comps1 = COMPOUND_FIELD_PTR(data1, type1->root_size,
                            0, void*, parent_comp_i);
            child_comps2 = COMPOUND_FIELD_PTR(data2, type2->root_size,
                            0, void*, parent_comp_i);
            last_status = nccmp_cmp_var_user_type_nest_compound(
                            state, ncid1, ncid2,
                            varid1, varid2, start, parent_comp_i,
                            child_comps1, child_comps2,
                            nest_next);
            if ((last_status && !state->opts.force) || nccmp_check_diff_count(state, varid1)) {
                goto recover;
            }
        }
    }

recover:
    nccmp_destroy_nest(nest);
    nccmp_destroy_nest(nest_next);

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_array_user_type_enum(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int narrays, int rec, size_t* limits,
        void* data1, void* data2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    int status, diff_count_status;
    int array_i, i;
    nc_type field_type_id1 = type1->fields->items[fieldid1].type_id;
    nc_type field_type_id2 = type2->fields->items[fieldid2].type_id;
    long long item1, item2;
    char idx_str[NC_MAX_NAME], str1[NC_MAX_NAME], str2[NC_MAX_NAME],
         field_path_str[NC_MAX_NAME], array_idx_str[64], root_idx_str[64];
    size_t array_size;
    void *enums1, *enums2;
    nccmp_var_t *v1;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_array_user_type_enum\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        goto recover;
    }

    idx_str[0] = 0;
    str1[0] = 0;
    str2[0] = 0;
    v1 = & state->vars1[varid1];

    array_size = (size_t) nccmp_product_int(type1->fields->items[fieldid1].dim_sizes,
                                      0, type1->fields->items[fieldid1].num_dims-1);

#define CMP_ENUM_ARRAY(TYPE1, TYPE2)                                           \
    for(array_i = 0; array_i < narrays; ++array_i) {                           \
        enums1 = COMPOUND_FIELD_PTR(data1, type1->root_size, type1->fields->items[fieldid1].offset, TYPE1, array_i);   \
        enums2 = COMPOUND_FIELD_PTR(data2, type2->root_size, type2->fields->items[fieldid2].offset, TYPE2, array_i);   \
        for(i = 0; i < array_size; ++i) {                                      \
            item1 = (long long) ((TYPE1*)enums1)[i];                           \
            status = nc_inq_enum_ident(ncid1, field_type_id1, item1, str1);    \
            HANDLE_NC_ERROR(status);                                           \
            item2 = (long long) ((TYPE2*)enums2)[i];                           \
            status = nc_inq_enum_ident(ncid2, field_type_id2, item2, str2);    \
            HANDLE_NC_ERROR(status);                                           \
            if (strcmp(str1, str2)) {                                          \
                if (!state->opts.warn[NCCMP_W_ALL]) {                          \
                    last_status = EXIT_DIFFER;                                 \
                }                                                              \
                diff_count_status = nccmp_inc_diff_count(state, varid1);       \
                if (COUNT_OVER == diff_count_status) {                         \
                    goto recover;                                              \
                }                                                              \
                if (!state->opts.quiet) {                                      \
                    nccmp_get_index_str_size_t(v1->ndims, start, array_i, root_idx_str, state->opts.fortran);    \
                    nccmp_get_product_index_str_int(type1->fields->items[fieldid1].num_dims,                     \
                                              type1->fields->items[fieldid1].dim_sizes,                          \
                                              i, array_idx_str,                                                  \
                                              state->opts.fortran);                                              \
                    if (state->opts.fortran) {                                                                   \
                        sprintf(idx_str, "%s,%s", array_idx_str, root_idx_str);                                  \
                    } else {                                                                                     \
                        sprintf(idx_str, "%s,%s", root_idx_str, array_idx_str);                                  \
                    }                                                                                            \
                    field_path_str[0] = 0;                                                                       \
                    nccmp_build_user_type_field_path_str(& type1->fields->items[fieldid1], field_path_str);      \
                    PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD, v1->name, idx_str,                             \
                            field_path_str, str1, str2);                                                         \
                }                                                                                                \
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {         \
                    goto recover;                                              \
                }                                                              \
            }                                                                  \
        }                                                                      \
    }

#define CMP_ENUM_TYPE_ARRAY(TYPE1)                                             \
    switch(type2->fields->items[fieldid2].base_type) {                         \
        case NC_BYTE:   CMP_ENUM_ARRAY(TYPE1, signed char);       break;       \
        case NC_CHAR:   CMP_ENUM_ARRAY(TYPE1, char);              break;       \
        case NC_INT:    CMP_ENUM_ARRAY(TYPE1, int);               break;       \
        case NC_INT64:  CMP_ENUM_ARRAY(TYPE1, long long);         break;       \
        case NC_SHORT:  CMP_ENUM_ARRAY(TYPE1, short);             break;       \
        case NC_UBYTE:  CMP_ENUM_ARRAY(TYPE1, unsigned char);     break;       \
        case NC_UINT:   CMP_ENUM_ARRAY(TYPE1, unsigned int);      break;       \
        case NC_UINT64: CMP_ENUM_ARRAY(TYPE1, unsigned long long);break;       \
        case NC_USHORT: CMP_ENUM_ARRAY(TYPE1, unsigned short);    break;       \
        default:                                                               \
             LOG_ERROR("Unsupported second enum data type = %d\n", type2->fields->items[fieldid2].base_type);\
             last_status = EXIT_FAILED;                                        \
             break;                                                            \
    }

    switch(type1->fields->items[fieldid1].base_type) {
        case NC_BYTE:   CMP_ENUM_TYPE_ARRAY(signed char);       break;
        case NC_CHAR:   CMP_ENUM_TYPE_ARRAY(char);              break;
        case NC_INT:    CMP_ENUM_TYPE_ARRAY(int);               break;
        case NC_INT64:  CMP_ENUM_TYPE_ARRAY(long long);         break;
        case NC_SHORT:  CMP_ENUM_TYPE_ARRAY(short);             break;
        case NC_UBYTE:  CMP_ENUM_TYPE_ARRAY(unsigned char);     break;
        case NC_UINT:   CMP_ENUM_TYPE_ARRAY(unsigned int);      break;
        case NC_UINT64: CMP_ENUM_TYPE_ARRAY(unsigned long long);break;
        case NC_USHORT: CMP_ENUM_TYPE_ARRAY(unsigned short);    break;
        default:
             LOG_ERROR("Unsupported first enum data type = %d\n", type1->fields->items[fieldid1].base_type);
             last_status = EXIT_FAILED;
             break;
    }

#undef CMP_ENUM_ARRAY
#undef CMP_ENUM_TYPE_ARRAY

recover:
    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_array_user_type_opaque(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int narrays, int rec, size_t* limits,
        void* data1, void* data2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    int nbytes, ndims, array_i, i, diff_count_status;
    char idx_str[NC_MAX_NAME], field_path_str[NC_MAX_NAME], array_idx_str[64], root_idx_str[64];
    char *str1 = 0, *str2 = 0;
    size_t array_size;
    unsigned char *items1, *items2;
    unsigned char *bytes1, *bytes2;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_array_user_type_opaque\n");
    }

    nbytes = type1->fields->items[fieldid1].size;
    ndims = state->vars1[varid1].ndims;
    array_size = (size_t) nccmp_product_int(type1->fields->items[fieldid1].dim_sizes,
                                      0, type1->fields->items[fieldid1].num_dims-1);

    if (nccmp_check_diff_count(state, varid1)) {
        goto recover;
    }

    idx_str[0] = 0;

    for(array_i = 0; array_i < narrays; ++array_i) {
        items1 = (unsigned char*) COMPOUND_FIELD_PTR(data1, type1->root_size, type1->fields->items[fieldid1].offset, unsigned char, array_i);
        items2 = (unsigned char*) COMPOUND_FIELD_PTR(data2, type2->root_size, type2->fields->items[fieldid2].offset, unsigned char, array_i);
        for(i = 0; i < array_size; ++i) {
            bytes1 = items1 + (nbytes * i);
            bytes2 = items2 + (nbytes * i);
            if (memcmp(bytes1, bytes2, nbytes)) {
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                diff_count_status = nccmp_inc_diff_count(state, varid1);
                if (COUNT_OVER == diff_count_status) {
                    goto recover;
                }
                if (!state->opts.quiet) {
                    nccmp_get_index_str_size_t(ndims, start, array_i, root_idx_str, state->opts.fortran);
                    nccmp_get_product_index_str_int(type1->fields->items[fieldid1].num_dims,
                                              type1->fields->items[fieldid1].dim_sizes,
                                              i, array_idx_str,
                                              state->opts.fortran);
                    if (state->opts.fortran) {
                        sprintf(idx_str, "%s,%s", array_idx_str, root_idx_str);
                    } else {
                        sprintf(idx_str, "%s,%s", root_idx_str, array_idx_str);
                    }
                    str1 = nccmp_realloc_bytes_hex_str(str1, nbytes);
                    str2 = nccmp_realloc_bytes_hex_str(str2, nbytes);
                    nccmp_bytes_to_hex_str(bytes1, nbytes, str1);
                    nccmp_bytes_to_hex_str(bytes2, nbytes, str2);
                    field_path_str[0] = 0;
                    nccmp_build_user_type_field_path_str(& type1->fields->items[fieldid1],
                            field_path_str);
                    PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD,
                               state->vars1[varid1].name, idx_str,
                               field_path_str, str1, str2);
                }
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
                    goto recover;
                }
            }
        }
    }

recover:
    XFREE(str1);
    XFREE(str2);

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_array_user_type_vlen(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int ncomps, int rec, size_t* limits,
        void* data1, void* data2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    nccmp_nest_t *nest = 0;
    nccmp_nest_t *nest_next = 0;
    int comp_i, array_size, i;
    void *vlens1 = 0, *vlens2 = 0;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_array_user_type_vlen\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        return last_status;
    }

    nest = nccmp_create_nest();
    nest_next = nccmp_create_nest();

    // The root compound types.
    nest->type1 = type1;
    nest->type2 = type2;

    // Set the field vlen types.
    nest_next->type1 = & type1->fields->items[fieldid1];
    nest_next->type2 = & type2->fields->items[fieldid2];
    nest_next->prev = nest;
    nest_next->num_dims = nest_next->type1->num_dims;

    array_size = nccmp_product_int(type1->fields->items[fieldid1].dim_sizes,
                            0, type1->fields->items[fieldid1].num_dims-1);

    for(comp_i = 0; comp_i < ncomps; ++comp_i) {
        for(i = 0; i < array_size; ++i) {
            // Convert flattened product index into slice array indices for formatted diff print.
            nccmp_get_product_index_inverse_array_int(
                    nest_next->type1->num_dims,
                    nest_next->type1->dim_sizes,
                    i,
                    nest_next->index);
            vlens1 = COMPOUND_FIELD_PTR(data1, type1->root_size,
                            type1->fields->items[fieldid1].offset, void*, comp_i);
            vlens2 = COMPOUND_FIELD_PTR(data2, type2->root_size,
                            type2->fields->items[fieldid2].offset, void*, comp_i);
            last_status = nccmp_cmp_var_user_type_nest_vlen(
                            state, ncid1, ncid2,
                            varid1, varid2, start, comp_i, vlens1, vlens2, nest_next);
            if ((last_status && !state->opts.force) || nccmp_check_diff_count(state, varid1)) {
                goto recover;
            }
        }
    }

recover:
    nccmp_destroy_nest(nest);
    nccmp_destroy_nest(nest_next);

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_array_user_type(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int nitems, int rec, size_t* limits,
        void* data1, void* data2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_array_user_type\n");
    }

#define CMP_FIELD_USER_TYPE_ARRAY(CLASS)                                       \
    last_status = nccmp_cmp_var_user_type_compound_field_array_user_type_##CLASS(        \
                    state, ncid1, ncid2, varid1, varid2, start, count, nitems, \
                    rec, limits, data1, data2, type1, type2, fieldid1, fieldid2);

    switch(type1->fields->items[fieldid1].user_class) {
    case NC_COMPOUND: CMP_FIELD_USER_TYPE_ARRAY(compound);    break;
    case NC_ENUM:     CMP_FIELD_USER_TYPE_ARRAY(enum);        break;
    case NC_OPAQUE:   CMP_FIELD_USER_TYPE_ARRAY(opaque);      break;
    case NC_VLEN:     CMP_FIELD_USER_TYPE_ARRAY(vlen);        break;
    default:
        LOG_ERROR("Unsupported user defined type class = %d\n",
                type1->fields->items[fieldid1].user_class);
        last_status = EXIT_FATAL;
        break;
    }

#undef CMP_FIELD_USER_TYPE_ARRAY

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_array(
        nccmp_state_t* state, int ncid1, int ncid2, int varid1, int varid2,
        size_t* start, size_t* count, int nitems, int rec, size_t* limits,
        void* data1, void* data2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_array\n");
    }

    if (NC_FIRSTUSERTYPEID > type1->fields->items[fieldid1].type_id) {
        return nccmp_cmp_var_user_type_compound_field_array_atomic_type(state, ncid1,
                ncid2, varid1, varid2, start, count, nitems, rec, limits,
                data1, data2, type1, type2, fieldid1, fieldid2);
    } else {
        return nccmp_cmp_var_user_type_compound_field_array_user_type(state, ncid1,
                ncid2, varid1, varid2, start, count, nitems, rec, limits,
                data1, data2, type1, type2, fieldid1, fieldid2);
    }

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_nonarray_atomic_type_string(
        nccmp_state_t* state, int ncid1, int ncid2, int varid1, int varid2,
        size_t* start, size_t* count, int nitems, int rec, size_t* limits,
        void* items1, void* items2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int status, last_status = EXIT_SUCCESS;
    char **s1, **s2;
    char idx_str[NC_MAX_NAME], field_path_str[NC_MAX_NAME];
    int i;
    count_limit_type diff_count_status;
    nccmp_var_t *v1 = & state->vars1[varid1];

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_nonarray_atomic_type_string\n");
    }

    diff_count_status = nccmp_check_diff_count(state, varid1);
    if (diff_count_status) {
        goto recover;
    }

    for(i = 0; i < nitems; ++i) {
        s1 = COMPOUND_FIELD_PTR(items1,
                                 type1->root_size,
                                 type1->fields->items[fieldid1].offset,
                                 char*,
                                 i);
        s2 = COMPOUND_FIELD_PTR(items2,
                                 type2->root_size,
                                 type2->fields->items[fieldid2].offset,
                                 char*,
                                 i);

        if ( ( s1[0] && !s2[0]) ||
             (!s1[0] &&  s2[0]) ||
             ( s1[0] && s2[0] && strcmp(s1[0], s2[0]) )
           ) {
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            // NB: No tolerance is checked for strings. Could use string distance in future.
            diff_count_status = nccmp_inc_diff_count(state, varid1);
            if (COUNT_OVER == diff_count_status) {
                goto recover;
            }
            if (!state->opts.quiet) {
                nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);
                strcpy(field_path_str, "");
                nccmp_build_user_type_field_path_str(& type1->fields->items[fieldid1], field_path_str);
                PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD, v1->name, idx_str, field_path_str, s1[0], s2[0]);
            }
            if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force ) {
                goto recover;
            }
        }
    }

recover:
    /* We must free strings internally allocated by library. */
    for(i = 0; i < nitems; ++i) {
        s1 = COMPOUND_FIELD_PTR(items1,
                                 type1->root_size,
                                 type1->fields->items[fieldid1].offset,
                                 char*,
                                 i);
		status = nc_free_string(1, s1);
        HANDLE_NC_ERROR(status);

        s2 = COMPOUND_FIELD_PTR(items2,
                                 type2->root_size,
                                 type2->fields->items[fieldid2].offset,
                                 char*,
                                 i);
		status = nc_free_string(1, s2);
        HANDLE_NC_ERROR(status);
    }

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_nonarray_atomic_type(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int nitems, int rec, size_t* limits,
        void* items1, void* items2,
        nccmp_user_type_t *comp_type1, nccmp_user_type_t *comp_type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    char print_hex = nccmp_is_precision_hex(& state->opts);
    count_limit_type diff_count_status;
    size_t diff_offset, next_offset = 0;
    char idx_str[32], str1[32], str2[32], field_path_str[NC_MAX_NAME];
    nccmp_var_t *v1 = & state->vars1[varid1];
    nccmp_nc_type_t item1, item2;
    double absdelta, pct;
    int i;
    nccmp_user_type_t *user_type;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_nonarray_atomic_type\n");
        LOG_DEBUG("comp_name1=%s parent_name1=%s field1=%s fieldid1=%d offset1=%zu "
                  "comp_name2=%s parent_name2=%s field2=%s fieldid2=%d offset2=%zu nitems=%d\n",
                comp_type1->name,
                comp_type1->parent ? comp_type1->parent->name : "",
                comp_type1->fields->items[fieldid1].name,
                fieldid1,
                comp_type1->fields->items[fieldid1].offset,
                comp_type2->name,
                comp_type2->parent ? comp_type2->parent->name : "",
                comp_type2->fields->items[fieldid2].name,
                fieldid2,
                comp_type2->fields->items[fieldid2].offset,
                nitems);
    }

    diff_count_status = nccmp_check_diff_count(state, varid1);

    if (diff_count_status) {
        goto recover;
    }

#define CMP_FIELD_ATOMIC(NAME1, TYPE1, NAME2, TYPE2, NANEQUAL)                 \
    while (1) {                                                                \
        if (NANEQUAL) {                                                        \
            diff_offset = nccmp_cmp_field_##NAME1##_##NAME2##_nanequal(        \
                            items1, comp_type1->fields->items[fieldid1].offset, comp_type1->root_size,       \
                            items2, comp_type2->fields->items[fieldid2].offset, comp_type2->root_size,       \
                            next_offset, nitems);                              \
        } else {                                                               \
            diff_offset = nccmp_cmp_field_##NAME1##_##NAME2(                   \
                            items1, comp_type1->fields->items[fieldid1].offset, comp_type1->root_size,       \
                            items2, comp_type2->fields->items[fieldid2].offset, comp_type2->root_size,       \
                            next_offset, nitems);                              \
        }                                                                      \
        if (diff_offset >= nitems) {                                           \
            break;                                                             \
        }                                                                      \
        /* Diff found. */                                                      \
        next_offset = diff_offset + 1;                                         \
        if (!state->opts.warn[NCCMP_W_ALL]) {                                  \
          last_status = EXIT_DIFFER;                                           \
        } else {                                                               \
          last_status = EXIT_SUCCESS;                                          \
        }                                                                      \
        diff_count_status = nccmp_inc_diff_count(state, varid1);               \
        if (COUNT_OVER == diff_count_status) {                                 \
            break;                                                             \
        }                                                                      \
        if (!state->opts.quiet) {                                              \
            nccmp_get_index_str_size_t(v1->ndims, start, diff_offset, idx_str, state->opts.fortran); \
            item1.m_##NAME1 = COMPOUND_FIELD_VALUE(items1, comp_type1->root_size,       \
                comp_type1->fields->items[fieldid1].offset, TYPE1, diff_offset);        \
            item2.m_##NAME2 = COMPOUND_FIELD_VALUE(items2, comp_type2->root_size,       \
                comp_type2->fields->items[fieldid2].offset, TYPE2, diff_offset);        \
            if (print_hex) {                                                            \
                nccmp_##NAME1##_to_hex(item1.m_##NAME1, str1);                          \
                nccmp_##NAME2##_to_hex(item2.m_##NAME2, str2);                          \
            } else {                                                                    \
                nccmp_##NAME1##_to_str(item1.m_##NAME1, str1, state->opts.precision);   \
                nccmp_##NAME2##_to_str(item2.m_##NAME2, str2, state->opts.precision);   \
            }                                                                           \
            strcpy(field_path_str, "");                                                 \
            nccmp_build_user_type_field_path_str(& comp_type1->fields->items[fieldid1], field_path_str); \
            PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD, v1->name, idx_str,            \
                  field_path_str, str1, str2);                                          \
        }                                                                               \
        /* Print only up to count of desired diffs. */                                  \
        if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {                \
            break;                                                                      \
        }                                                                               \
    }

#define CMP_FIELD_ATOMIC_TOL(NAME1, TYPE1, NAME2, TYPE2, NANEQUAL)             \
    for(i = 0; i < nitems; ++i) {                                              \
        item1.m_##NAME1 = COMPOUND_FIELD_VALUE(items1, comp_type1->root_size,  \
            comp_type1->fields->items[fieldid1].offset, TYPE1, i);             \
        item2.m_##NAME2 = COMPOUND_FIELD_VALUE(items2, comp_type2->root_size,  \
            comp_type2->fields->items[fieldid2].offset, TYPE2, i);             \
        if (NANEQUAL) {                                                        \
            if ( isnan(item1.m_##NAME1) && isnan(item2.m_##NAME2) ) continue;  \
        }                                                                      \
        absdelta = fabs((double)(item1.m_##NAME1 - item2.m_##NAME2));          \
        if (state->opts.abstolerance ?                                         \
                (absdelta > state->opts.tolerance) :                           \
                (double)absdelta*100./(fabs((double)item1.m_##NAME1) > fabs((double)item2.m_##NAME2) ?       \
                        fabs((double)item1.m_##NAME1) :                        \
                        fabs((double)item2.m_##NAME2)) > state->opts.tolerance)\
        {                                                                      \
            if (!state->opts.warn[NCCMP_W_ALL]) {                              \
                last_status = EXIT_DIFFER;                                     \
            } else {                                                           \
                last_status = EXIT_SUCCESS;                                    \
            }                                                                  \
            diff_count_status = nccmp_inc_diff_count(state, varid1);           \
            if (COUNT_OVER == diff_count_status) {                             \
                break;                                                         \
            }                                                                  \
            if (!state->opts.quiet) {                                          \
                if ((v1->ndims == 1)  && (v1->has_rec_dim)) {                                           \
                    nccmp_get_index_str_size_t(v1->ndims, start, rec, idx_str, state->opts.fortran);    \
                } else {                                                                                \
                    nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);      \
                }                                                                                       \
                if (print_hex) {                                                                        \
                    nccmp_##NAME1##_to_hex(item1.m_##NAME1, str1);                                      \
                    nccmp_##NAME2##_to_hex(item2.m_##NAME2, str2);                                      \
                } else {                                                                                \
                    nccmp_##NAME1##_to_str(item1.m_##NAME1, str1, state->opts.precision);               \
                    nccmp_##NAME2##_to_str(item2.m_##NAME2, str2, state->opts.precision);               \
                }                                                                                       \
                pct = (double)absdelta * 100.0 /                                                        \
                      (fabs((double)item1.m_##NAME1) > fabs((double)item2.m_##NAME2) ?                  \
                        fabs((double)item1.m_##NAME1) :                                                 \
                        fabs((double)item2.m_##NAME2));                                                 \
                strcpy(field_path_str, "");                                                             \
                nccmp_build_user_type_field_path_str(& comp_type1->fields->items[fieldid1], field_path_str); \
                PRINT_DIFF(state->opts.debug, DIFF_MSG_PCT_FIELD, v1->name, idx_str,                    \
                           field_path_str, str1, str2, pct );                                           \
            }                                                                                           \
            if ( (COUNT_EQUAL ==  diff_count_status) || !state->opts.force ) {                          \
                break;                                                                                  \
            }                                                                                           \
        }                                                                                               \
    }

#define CMP_FIELD_ATOMIC_SELECT(...)                                           \
    if (state->opts.tolerance) {                                               \
        CMP_FIELD_ATOMIC_TOL(__VA_ARGS__);                                     \
    } else {                                                                   \
        CMP_FIELD_ATOMIC(__VA_ARGS__);                                         \
    }

#define CMP_FIELD_ATOMIC_TYPE(NAME1, TYPE1, NANEQUAL)                          \
    switch(comp_type2->fields->items[fieldid2].type_id) {                      \
    case NC_BYTE:   CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, schar,     signed char,   0); break;     \
    case NC_CHAR:   CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, text,      char,          0); break;     \
    case NC_DOUBLE: CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, double,    double,        state->opts.nanequal); break;      \
    case NC_FLOAT:  CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, float,     float,         state->opts.nanequal); break;      \
    case NC_INT:    CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, int,       int,           0); break;     \
    case NC_INT64:  CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, longlong,  long long,     0); break;     \
    case NC_SHORT:  CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, short,     short,         0); break;     \
    case NC_UBYTE:  CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, uchar,     unsigned char, 0); break;     \
    case NC_UINT:   CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, uint,      unsigned int,  0); break;     \
    case NC_UINT64: CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, ulonglong, unsigned long long,0); break; \
    case NC_USHORT: CMP_FIELD_ATOMIC_SELECT(NAME1, TYPE1, ushort,    unsigned short,    0); break; \
    case NC_STRING:                                                            \
    default:                                                                   \
         user_type = nccmp_get_user_type_by_id(state->types2->items, state->types2->num_items, state->vars2[varid2].type);       \
         LOG_ERROR("Unsupported second var field %s data type = %d\n",         \
            user_type->fields->items[fieldid2].name,                           \
            user_type->fields->items[fieldid2].type_id);                       \
         last_status = EXIT_FAILED;                                            \
         break;                                                                \
    }

    switch(comp_type1->fields->items[fieldid1].type_id) {
    case NC_BYTE:   CMP_FIELD_ATOMIC_TYPE(schar,    signed char,        0);                     break;
    case NC_CHAR:   CMP_FIELD_ATOMIC_TYPE(text,     char,               0);                     break;
    case NC_DOUBLE: CMP_FIELD_ATOMIC_TYPE(double,   double,             state->opts.nanequal);  break;
    case NC_FLOAT:  CMP_FIELD_ATOMIC_TYPE(float,    float,              state->opts.nanequal);  break;
    case NC_INT:    CMP_FIELD_ATOMIC_TYPE(int,      int,                0);                     break;
    case NC_INT64:  CMP_FIELD_ATOMIC_TYPE(longlong, long long,          0);                     break;
    case NC_SHORT:  CMP_FIELD_ATOMIC_TYPE(short,    short,              0);                     break;
    case NC_UBYTE:  CMP_FIELD_ATOMIC_TYPE(uchar,    unsigned char,      0);                     break;
    case NC_UINT:   CMP_FIELD_ATOMIC_TYPE(uint,     unsigned int,       0);                     break;
    case NC_UINT64: CMP_FIELD_ATOMIC_TYPE(ulonglong,unsigned long long, 0);                     break;
    case NC_USHORT: CMP_FIELD_ATOMIC_TYPE(ushort,   unsigned short,     0);                     break;
    case NC_STRING:
        if (NC_STRING == comp_type2->fields->items[fieldid2].type_id) {
            last_status = nccmp_cmp_var_user_type_compound_field_nonarray_atomic_type_string(
                            state, ncid1, ncid2, varid1, varid2,
                            start, count, nitems, rec, limits,
                            items1, items2, comp_type1, comp_type2, fieldid1, fieldid2);
            break;
        }
    default:
        LOG_ERROR("Unsupported first var field %s data type = %d\n",
                comp_type1->fields->items[fieldid1].name,
                comp_type1->fields->items[fieldid1].type_id);
        last_status = EXIT_FAILED;
        break;
    }

#undef CMP_FIELD_ATOMIC
#undef CMP_FIELD_ATOMIC_TOL
#undef CMP_FIELD_ATOMIC_SELECT
#undef CMP_FIELD_ATOMIC_TYPE

recover:
    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_nonarray_user_type_compound(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int nitems, int rec, size_t* limits,
        void* items1, void* items2, nccmp_user_type_t *parent_type1,
        nccmp_user_type_t *parent_type2, int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    nccmp_int_pairs_t *child_field_ids;
    nccmp_user_type_t *field_type1 = & parent_type1->fields->items[fieldid1];
    nccmp_user_type_t *field_type2 = & parent_type2->fields->items[fieldid2];

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_nonarray_user_type_compound\n");
    }

    child_field_ids = ncccmp_get_user_type_compound_field_index_pairs(field_type1, field_type2,
                        state->opts.debug);

    last_status = nccmp_cmp_var_user_type_compound_field_ids(state, ncid1, ncid2,
                    varid1, varid2, start, count, nitems, rec, limits,
                    items1, items2, field_type1, field_type2, child_field_ids);

    nccmp_destroy_int_pairs(child_field_ids);

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_nonarray_user_type_enum(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int nitems, int rec, size_t* limits,
        void* items1, void* items2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    int status, diff_count_status;
    int item_i;
    nc_type field_type_id1 = type1->fields->items[fieldid1].type_id;
    nc_type field_type_id2 = type2->fields->items[fieldid2].type_id;
    long long item1, item2;
    char idx_str[NC_MAX_NAME], str1[NC_MAX_NAME], str2[NC_MAX_NAME];

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_nonarray_user_type_enum\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        goto recover;
    }

    idx_str[0] = 0;
    str1[0] = 0;
    str2[0] = 0;

#define CMP_ENUM(TYPE1, TYPE2)                                                 \
    for(item_i = 0; item_i < nitems; ++item_i) {                               \
        item1 = (long long) COMPOUND_FIELD_VALUE(items1, type1->root_size, type1->fields->items[fieldid1].offset, TYPE1, item_i);\
        item2 = (long long) COMPOUND_FIELD_VALUE(items2, type2->root_size, type2->fields->items[fieldid2].offset, TYPE2, item_i);\
        status = nc_inq_enum_ident(ncid1, field_type_id1, item1, str1);        \
        HANDLE_NC_ERROR(status);                                               \
        status = nc_inq_enum_ident(ncid2, field_type_id2, item2, str2);        \
        HANDLE_NC_ERROR(status);                                               \
        if (strcmp(str1, str2)) {                                              \
            if (!state->opts.warn[NCCMP_W_ALL]) {                              \
                last_status = EXIT_DIFFER;                                     \
            }                                                                  \
            diff_count_status = nccmp_inc_diff_count(state, varid1);           \
            if (COUNT_OVER == diff_count_status) {                             \
                goto recover;                                                  \
            }                                                                  \
            if (!state->opts.quiet) {                                          \
                nccmp_get_index_str_size_t(state->vars1[varid1].ndims, start, item_i, idx_str, state->opts.fortran); \
                PRINT_DIFF(state->opts.debug, DIFF_MSG, state->vars1[varid1].name, idx_str, str1, str2);             \
            }                                                                  \
            if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {   \
                goto recover;                                                  \
            }                                                                  \
        }                                                                      \
    }

#define CMP_ENUM_TYPE(TYPE1)                                                   \
    switch(type2->fields->items[fieldid2].base_type) {                         \
    case NC_BYTE:   CMP_ENUM(TYPE1, signed char);       break;                 \
    case NC_CHAR:   CMP_ENUM(TYPE1, char);              break;                 \
    case NC_INT:    CMP_ENUM(TYPE1, int);               break;                 \
    case NC_INT64:  CMP_ENUM(TYPE1, long long);         break;                 \
    case NC_SHORT:  CMP_ENUM(TYPE1, short);             break;                 \
    case NC_UBYTE:  CMP_ENUM(TYPE1, unsigned char);     break;                 \
    case NC_UINT:   CMP_ENUM(TYPE1, unsigned int);      break;                 \
    case NC_UINT64: CMP_ENUM(TYPE1, unsigned long long);break;                 \
    case NC_USHORT: CMP_ENUM(TYPE1, unsigned short);    break;                 \
    default:                                                                   \
         LOG_ERROR("Unsupported second enum data type = %d\n", type2->fields->items[fieldid2].base_type);    \
         last_status = EXIT_FAILED;                                            \
         break;                                                                \
    }

    switch(type1->fields->items[fieldid1].base_type) {
    case NC_BYTE:   CMP_ENUM_TYPE(signed char);       break;
    case NC_CHAR:   CMP_ENUM_TYPE(char);              break;
    case NC_INT:    CMP_ENUM_TYPE(int);               break;
    case NC_INT64:  CMP_ENUM_TYPE(long long);         break;
    case NC_SHORT:  CMP_ENUM_TYPE(short);             break;
    case NC_UBYTE:  CMP_ENUM_TYPE(unsigned char);     break;
    case NC_UINT:   CMP_ENUM_TYPE(unsigned int);      break;
    case NC_UINT64: CMP_ENUM_TYPE(unsigned long long);break;
    case NC_USHORT: CMP_ENUM_TYPE(unsigned short);    break;
    default:
         LOG_ERROR("Unsupported first enum data type = %d\n", type1->fields->items[fieldid1].base_type);
         last_status = EXIT_FAILED;
         break;
    }

#undef CMP_ENUM
#undef CMP_ENUM_TYPE

recover:
    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_nonarray_user_type_opaque(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int nitems, int rec, size_t* limits,
        void* data1, void* data2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    unsigned char *items1 = (unsigned char*) data1;
    unsigned char *items2 = (unsigned char*) data2;
    unsigned char *bytes1, *bytes2;
    int item_i;
    int nbytes = type1->fields->items[fieldid1].size;
    int ndims = state->vars1[varid1].ndims;
    char idx_str[NC_MAX_NAME], *str1 = 0, *str2 = 0;
    int diff_count_status;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_nonarray_user_type_opaque\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        goto recover;
    }

    idx_str[0] = 0;

    for(item_i = 0; item_i < nitems; ++item_i) {
        bytes1 = items1 + type1->root_size * item_i + type1->fields->items[fieldid1].offset;
        bytes2 = items2 + type2->root_size * item_i + type2->fields->items[fieldid2].offset;
        if (memcmp(bytes1, bytes2, nbytes)) {
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            diff_count_status = nccmp_inc_diff_count(state, varid1);
            if (COUNT_OVER == diff_count_status) {
                goto recover;
            }
            if (!state->opts.quiet) {
                nccmp_get_index_str_size_t(ndims, start, item_i, idx_str, state->opts.fortran);
                str1 = nccmp_realloc_bytes_hex_str(str1, nbytes);
                str2 = nccmp_realloc_bytes_hex_str(str2, nbytes);
                nccmp_bytes_to_hex_str(bytes1, nbytes, str1);
                nccmp_bytes_to_hex_str(bytes2, nbytes, str2);
                PRINT_DIFF(state->opts.debug, DIFF_MSG, state->vars1[varid1].name, idx_str, str1, str2);
            }
            if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
                goto recover;
            }
        }
    }

recover:
    XFREE(str1);
    XFREE(str2);

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_nonarray_user_type_vlen(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int nitems, int rec, size_t* limits,
        void* items1, void* items2, nccmp_user_type_t *type1,
        nccmp_user_type_t *type2, int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    nccmp_nest_t *nest = nccmp_create_nest();
    nccmp_nest_t *nest_next = nccmp_create_nest();
    int comp_i;
    void *vlen1, *vlen2;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_nonarray_user_type_vlen\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        return last_status;
    }

    nest->type1 = type1;
    nest->type2 = type2;
    // Set the vlen types.
    nest_next->type1 = & type1->fields->items[fieldid1];
    nest_next->type2 = & type2->fields->items[fieldid2];
    nest_next->prev = nest;

    for(comp_i = 0; comp_i < nitems; ++comp_i) {
        vlen1 = COMPOUND_FIELD_PTR(items1, type1->size,
                        type1->fields->items[fieldid1].offset, void*, comp_i);
        vlen2 = COMPOUND_FIELD_PTR(items2, type2->size,
                        type2->fields->items[fieldid2].offset, void*, comp_i);
        last_status = nccmp_cmp_var_user_type_nest_vlen(
                    state, ncid1, ncid2,
                    varid1, varid2, start, comp_i, vlen1, vlen2, nest_next);
        if ((last_status && !state->opts.force) || nccmp_check_diff_count(state, varid1)) {
            goto recover;
        }
    }

recover:
    nccmp_destroy_nest(nest);
    nccmp_destroy_nest(nest_next);

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_nonarray_user_type(nccmp_state_t* state,
        int ncid1, int ncid2, int varid1, int varid2, size_t* start,
        size_t* count, int nitems, int rec, size_t* limits,
        void* data1, void* data2, nccmp_user_type_t *type1, nccmp_user_type_t *type2,
        int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_nonarray_user_type\n");
    }

#define CMP_FIELD_USER_TYPE_NONARRAY(CLASS)                                    \
    last_status = nccmp_cmp_var_user_type_compound_field_nonarray_user_type_##CLASS(     \
                    state, ncid1, ncid2, varid1, varid2, start, count, nitems, \
                    rec, limits, data1, data2, type1, type2, fieldid1, fieldid2);

    switch(type1->fields->items[fieldid1].user_class) {
    case NC_COMPOUND: CMP_FIELD_USER_TYPE_NONARRAY(compound);    break;
    case NC_ENUM:     CMP_FIELD_USER_TYPE_NONARRAY(enum);        break;
    case NC_OPAQUE:   CMP_FIELD_USER_TYPE_NONARRAY(opaque);      break;
    case NC_VLEN:     CMP_FIELD_USER_TYPE_NONARRAY(vlen);        break;
    default:
        LOG_ERROR("Unsupported user defined type class = %d in first file.\n",
                type1->fields->items[fieldid1].user_class);
        last_status = EXIT_FATAL;
        break;
    }

#undef CMP_FIELD_USER_TYPE_NONARRAY

    return last_status;
}

int nccmp_cmp_var_user_type_compound_field_nonarray(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count, int nitems,
        int rec, size_t* limits, void* data1, void* data2,
        nccmp_user_type_t *type1, nccmp_user_type_t *type2, int fieldid1, int fieldid2)
{
    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_nonarray\n");
    }

    if (NC_FIRSTUSERTYPEID > type1->fields->items[fieldid1].type_id) {
        return nccmp_cmp_var_user_type_compound_field_nonarray_atomic_type(state, ncid1,
                ncid2, varid1, varid2, start, count, nitems, rec, limits,
                data1, data2, type1, type2, fieldid1, fieldid2);
    } else {
        return nccmp_cmp_var_user_type_compound_field_nonarray_user_type(state, ncid1,
                ncid2, varid1, varid2, start, count, nitems, rec, limits,
                data1, data2, type1, type2, fieldid1, fieldid2);
    }
}

int nccmp_cmp_var_user_type_compound_field(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count, int nitems,
        int rec, size_t* limits, void* data1, void* data2,
        nccmp_user_type_t *type1, nccmp_user_type_t *type2, int fieldid1, int fieldid2)
{
    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field\n");
    }

    if (type1->fields->items[fieldid1].num_dims &&
        type2->fields->items[fieldid2].num_dims) {
        return nccmp_cmp_var_user_type_compound_field_array(state, ncid1, ncid2,
                    varid1, varid2, start, count, nitems, rec, limits,
                    data1, data2, type1, type2, fieldid1, fieldid2);
    } else {
        return nccmp_cmp_var_user_type_compound_field_nonarray(state, ncid1, ncid2,
                    varid1, varid2, start, count, nitems, rec, limits,
                    data1, data2, type1, type2, fieldid1, fieldid2);
    }
}

int nccmp_cmp_var_user_type_compound_field_ids(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits, void* data1, void* data2,
        nccmp_user_type_t* comp_type1, nccmp_user_type_t* comp_type2, nccmp_int_pairs_t *fieldids)
{
    int status, last_status = EXIT_SUCCESS;
    int i;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound_field_ids\n");
    }

    for(i = 0; i < fieldids->size; ++i) {
        status = nccmp_cmp_var_user_type_compound_field(state, ncid1, ncid2,
                    varid1, varid2, start, count, nitems, rec, limits,
                    data1, data2, comp_type1, comp_type2,
                    fieldids->items[i].first, fieldids->items[i].second);

        last_status = status ? status : last_status;
        if(last_status && !state->opts.force) goto recover;
    }

recover:

    return last_status;
}

int nccmp_cmp_var_user_type_compound(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits)
{
    int status, last_status = EXIT_SUCCESS;
    unsigned char *data1 = NULL, *data2 = NULL;
    nccmp_int_pairs_t *fieldids = NULL;
    int loop_done = 0;
    nc_type comp_typeid1 = state->vars1[varid1].type;
    nc_type comp_typeid2 = state->vars2[varid2].type;
    nccmp_user_type_t *comp_type1 = nccmp_get_user_type_by_id(state->types1->items,
                                        state->types1->num_items, comp_typeid1);
    nccmp_user_type_t *comp_type2 = nccmp_get_user_type_by_id(state->types2->items,
                                        state->types2->num_items, comp_typeid2);

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_compound\n");
    }

    fieldids = ncccmp_get_user_type_compound_field_index_pairs(comp_type1, comp_type2,
                    state->opts.debug);

    if (0 >= fieldids->size) {
        goto recover;
    }

    data1 = XMALLOC(unsigned char, comp_type1->size * nitems);
    data2 = XMALLOC(unsigned char, comp_type2->size * nitems);

    while(! loop_done) {
        status = nc_get_vara(ncid1, varid1, start, count, data1);
        HANDLE_NC_ERROR(status);
        status = nc_get_vara(ncid2, varid2, start, count, data2);
        HANDLE_NC_ERROR(status);

        status = nccmp_cmp_var_user_type_compound_field_ids(state, ncid1, ncid2,
                        varid1, varid2, start, count, nitems, rec, limits,
                        data1, data2, comp_type1, comp_type2, fieldids);
        last_status = status ? status : last_status;
        if (last_status && !state->opts.force) {
            break;
        }
        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), state->vars1[varid1].ndims-2) == 0;
    }


recover:
    XFREE(data1);
    XFREE(data2);
    nccmp_destroy_int_pairs(fieldids);

    return last_status;
}

int nccmp_cmp_var_user_type_enum(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits)
{
    int status, last_status = EXIT_SUCCESS;
    nccmp_var_t *v1, *v2;
    nc_type base_type1, base_type2;
    nccmp_nc_type_ptr_t items1, items2;
    char str1[NC_MAX_NAME], str2[NC_MAX_NAME], idx_str[32];

    count_limit_type diff_count_status;
    int loop_done = 0;
    int item_i;

    diff_count_status = nccmp_check_diff_count(state, varid1);
    if (diff_count_status) {
        return last_status;
    }

    v1 = & state->vars1[varid1];
    v2 = & state->vars2[varid2];

    status = nc_inq_enum(ncid1, v1->type, 0 /*name*/, & base_type1,  0 /*base_size*/, 0 /*num_members*/);
    HANDLE_NC_ERROR(status);
    status = nc_inq_enum(ncid2, v2->type, 0 /*name*/, & base_type2,  0 /*base_size*/, 0 /*num_members*/);
    HANDLE_NC_ERROR(status);

#define CMP_ENUM(NAME1, TYPE1, NAME2, TYPE2) {                                 \
    items1.p_##NAME1 = (TYPE1*)malloc(sizeof(TYPE1) * nitems);                 \
    items2.p_##NAME2 = (TYPE2*)malloc(sizeof(TYPE2) * nitems);                 \
                                                                               \
    while(! loop_done) {                                                       \
        status = nc_get_vara(ncid1, v1->varid, start, count, items1.p_##NAME1);\
        HANDLE_NC_ERROR(status);                                               \
        status = nc_get_vara(ncid2, v2->varid, start, count, items2.p_##NAME2);\
        HANDLE_NC_ERROR(status);                                               \
        for(item_i = 0; item_i < nitems; ++item_i) {                           \
            status = nc_inq_enum_ident(ncid1, v1->type, (long long)items1.p_##NAME1[item_i], str1);\
            HANDLE_NC_ERROR(status);                                           \
            status = nc_inq_enum_ident(ncid2, v2->type, (long long)items2.p_##NAME2[item_i], str2);\
            HANDLE_NC_ERROR(status);                                           \
            if (strcmp(str1, str2)) {                                          \
                if (!state->opts.warn[NCCMP_W_ALL]) {                          \
                    last_status = EXIT_DIFFER;                                 \
                }                                                              \
                diff_count_status = nccmp_inc_diff_count(state, varid1);       \
                if (COUNT_OVER == diff_count_status) {                         \
                    goto recover_##NAME1##_##NAME2;                            \
                }                                                              \
                if (!state->opts.quiet) {                                      \
                    nccmp_get_index_str_size_t(v1->ndims, start, item_i, idx_str, state->opts.fortran); \
                    PRINT_DIFF(state->opts.debug, DIFF_MSG, v1->name, idx_str, str1, str2);             \
                }                                                                                       \
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {                        \
                    goto recover_##NAME1##_##NAME2;                            \
                }                                                              \
            }                                                                  \
        }                                                                      \
                                                                               \
        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;   \
    }                                                                          \
recover_##NAME1##_##NAME2:                                                     \
    free(items1.p_##NAME1);                                                    \
    free(items2.p_##NAME2);                                                    \
}

#define CMP_ENUM_TYPE(NAME1, TYPE1)                                            \
    switch(base_type2) {                                                       \
    case NC_BYTE:   CMP_ENUM(NAME1, TYPE1, schar,    signed char);       break;\
    case NC_CHAR:   CMP_ENUM(NAME1, TYPE1, text,     char);              break;\
    case NC_INT:    CMP_ENUM(NAME1, TYPE1, int,      int);               break;\
    case NC_INT64:  CMP_ENUM(NAME1, TYPE1, longlong, long long);         break;\
    case NC_SHORT:  CMP_ENUM(NAME1, TYPE1, short,    short);             break;\
    case NC_UBYTE:  CMP_ENUM(NAME1, TYPE1, uchar,    unsigned char);     break;\
    case NC_UINT:   CMP_ENUM(NAME1, TYPE1, uint,     unsigned int);      break;\
    case NC_UINT64: CMP_ENUM(NAME1, TYPE1, ulonglong,unsigned long long);break;\
    case NC_USHORT: CMP_ENUM(NAME1, TYPE1, ushort,   unsigned short);    break;\
    default:                                                                   \
         LOG_ERROR("Unsupported second enum data type = %d\n", base_type2);    \
         last_status = EXIT_FAILED;                                            \
         break;                                                                \
    }

    switch(base_type1) {
    case NC_BYTE:   CMP_ENUM_TYPE(schar,    signed char);       break;
    case NC_CHAR:   CMP_ENUM_TYPE(text,     char);              break;
    case NC_INT:    CMP_ENUM_TYPE(int,      int);               break;
    case NC_INT64:  CMP_ENUM_TYPE(longlong, long long);         break;
    case NC_SHORT:  CMP_ENUM_TYPE(short,    short);             break;
    case NC_UBYTE:  CMP_ENUM_TYPE(uchar,    unsigned char);     break;
    case NC_UINT:   CMP_ENUM_TYPE(uint,     unsigned int);      break;
    case NC_UINT64: CMP_ENUM_TYPE(ulonglong,unsigned long long);break;
    case NC_USHORT: CMP_ENUM_TYPE(ushort,   unsigned short);    break;
    default:
         LOG_ERROR("Unsupported first enum data type = %d\n", base_type1);
         last_status = EXIT_FAILED;
         break;
    }

#undef CMP_ENUM
#undef CMP_ENUM_TYPE

    return last_status;
}

int nccmp_cmp_var_user_type_nest_atomic_string(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest)
{
    int status, last_status = EXIT_SUCCESS;
    char **s1, **s2;
    char idx_str[NC_MAX_NAME], nest_str[NC_MAX_NAME];
    int diff_count_status;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_atomic_string\n");
    }

    s1 = (char**) data1 + nest->index[0];
    s2 = (char**) data2 + nest->index[0];

    if ( ( s1[0] && !s2[0]) ||
         (!s1[0] &&  s2[0]) ||
         ( s1[0] &&  s2[0] && strcmp(s1[0], s2[0]))
       )
    {
        if (!state->opts.warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        diff_count_status = nccmp_inc_diff_count(state, varid1);
        if (COUNT_OVER == diff_count_status) {
            goto recover;
        }
        if (!state->opts.quiet) {
            idx_str[0] = 0;
            nccmp_get_index_str_size_t(state->vars1[varid1].ndims, start, item, idx_str, state->opts.fortran);
            nest_str[0] = 0;
            nccmp_build_nest_str(nest, state->opts.fortran, nest_str);
            PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD,
                       state->vars1[varid1].name, idx_str, nest_str, s1[0], s2[0]);
        }
        if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
            goto recover;
        }
    }

recover:
	status = nc_free_string(1, s1);
	HANDLE_NC_ERROR(status);
	status = nc_free_string(1, s2);
	HANDLE_NC_ERROR(status);

    return last_status;
}

int nccmp_cmp_var_user_type_nest_atomic_item(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest)
{
    int last_status = EXIT_SUCCESS;
    char idx_str[64], nest_str[64], str1[64], str2[64];
    nccmp_nc_type_t value1, value2;
    int diff_count_status;
    nc_type base_type1, base_type2;
    int cardinal_index;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_atomic\n");
    }

    base_type1 = nest->type1->base_type ? nest->type1->base_type : nest->type1->type_id;
    base_type2 = nest->type2->base_type ? nest->type2->base_type : nest->type2->type_id;

#define CMP_VLEN(TYPE1, NAME1, TYPE2, NAME2)                                   \
    cardinal_index = nccmp_nest_cardinal_index(nest);                          \
    value1.m_##NAME1 = *( (TYPE1*) data1 + cardinal_index );                   \
    value2.m_##NAME2 = *( (TYPE2*) data2 + cardinal_index );                   \
    if (value1.m_##NAME1 != value2.m_##NAME2) {                                \
        if (!state->opts.warn[NCCMP_W_ALL]) {                                  \
            last_status = EXIT_DIFFER;                                         \
        }                                                                      \
        diff_count_status = nccmp_inc_diff_count(state, varid1);               \
        if (COUNT_OVER == diff_count_status) {                                 \
            goto recover;                                                      \
        }                                                                      \
        if (!state->opts.quiet) {                                              \
            nest_str[0] = 0;                                                                                    \
            nccmp_get_index_str_size_t(state->vars1[varid1].ndims, start, item, idx_str, state->opts.fortran);  \
            nccmp_build_nest_str(nest, state->opts.fortran, nest_str);                                          \
            nccmp_##NAME1##_to_str(value1.m_##NAME1, str1, state->opts.precision);                              \
            nccmp_##NAME2##_to_str(value2.m_##NAME2, str2, state->opts.precision);                              \
            PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD,                                                       \
                       state->vars1[varid1].name, idx_str, nest_str, str1, str2);                               \
        }                                                                                                       \
        if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {       \
            goto recover;                                                      \
        }                                                                      \
    }

#define CMP_VLEN_TYPE_ONE(TYPE1, NAME1)                                        \
        switch(base_type2) {                                                   \
            case NC_BYTE:   CMP_VLEN(TYPE1, NAME1, signed char,         schar);     break;         \
            case NC_CHAR:   CMP_VLEN(TYPE1, NAME1, char,                text);      break;         \
            case NC_DOUBLE: CMP_VLEN(TYPE1, NAME1, double,              double);    break;         \
            case NC_FLOAT:  CMP_VLEN(TYPE1, NAME1, float,               float);     break;         \
            case NC_INT:    CMP_VLEN(TYPE1, NAME1, int,                 int);       break;         \
            case NC_INT64:  CMP_VLEN(TYPE1, NAME1, long long,           longlong);  break;         \
            case NC_SHORT:  CMP_VLEN(TYPE1, NAME1, short,               short);     break;         \
            case NC_UBYTE:  CMP_VLEN(TYPE1, NAME1, unsigned char,       uchar);     break;         \
            case NC_UINT:   CMP_VLEN(TYPE1, NAME1, unsigned int,        uint);      break;         \
            case NC_UINT64: CMP_VLEN(TYPE1, NAME1, unsigned long long,  ulonglong); break;         \
            case NC_USHORT: CMP_VLEN(TYPE1, NAME1, unsigned short,      ushort);    break;         \
            default:                                                           \
                 LOG_ERROR("Unsupported base_type2=%d\n", base_type2);         \
                 last_status = EXIT_FAILED;                                    \
                 break;                                                        \
        }

    switch(base_type1) {
    case NC_BYTE:   CMP_VLEN_TYPE_ONE(signed char,          schar);     break;
    case NC_CHAR:   CMP_VLEN_TYPE_ONE(char,                 text);      break;
    case NC_DOUBLE: CMP_VLEN_TYPE_ONE(double,               double);    break;
    case NC_FLOAT:  CMP_VLEN_TYPE_ONE(float,                float);     break;
    case NC_INT:    CMP_VLEN_TYPE_ONE(int,                  int);       break;
    case NC_INT64:  CMP_VLEN_TYPE_ONE(long long,            longlong);  break;
    case NC_SHORT:  CMP_VLEN_TYPE_ONE(short,                short);     break;
    case NC_UBYTE:  CMP_VLEN_TYPE_ONE(unsigned char,        uchar);     break;
    case NC_UINT:   CMP_VLEN_TYPE_ONE(unsigned int,         uint);      break;
    case NC_UINT64: CMP_VLEN_TYPE_ONE(unsigned long long,   ulonglong); break;
    case NC_USHORT: CMP_VLEN_TYPE_ONE(unsigned short,       ushort);    break;
    case NC_STRING:
        if (NC_STRING == base_type2) {
            last_status = nccmp_cmp_var_user_type_nest_atomic_string(state,
                    ncid1, ncid2, varid1, varid2, start, item, data1, data2,
                    nest);
            break;
        }
    default:
         LOG_ERROR("Unsupported base_type1=%d\n", base_type1);
         last_status = EXIT_FAILED;
         break;
    }

#undef CMP_VLEN_TYPE_ONE
#undef CMP_VLEN
recover:
    return last_status;
}

int nccmp_cmp_var_user_type_nest_atomic(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest)
{
    int last_status = EXIT_SUCCESS;
    int multidim_i, multidim_size;
    char tmp_index_str[256];

    // If this is a compound field array, loop over each item.
    multidim_size = nccmp_product_int(nest->type1->dim_sizes, 0, nest->type1->num_dims-1);
    for(multidim_i = 0;  multidim_i <  multidim_size; ++ multidim_i) {
        nccmp_get_product_index_inverse_array_int(
                nest->type1->num_dims,
                nest->type1->dim_sizes,
                multidim_i,
                nest->index);
        tmp_index_str[0] = 0;
        nccmp_build_nest_index_str(nest, 0 /*fortran*/, tmp_index_str);
        if (state->opts.debug) {
            LOG_DEBUG("%s multidim_i=%d index=%s\n", nest->type1->name, multidim_i, tmp_index_str);
        }
        nccmp_cmp_var_user_type_nest_atomic_item(state, ncid1, ncid2,
                varid1, varid2, start, item, data1, data2, nest);
    }
    return last_status;
}

int nccmp_cmp_var_user_type_nest_compound_field(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, unsigned char *data1, unsigned char *data2,
        nccmp_nest_t *nest, int fieldid1, int fieldid2)
{
    int last_status = EXIT_SUCCESS;
    nccmp_user_type_t *field1, *field2;
    nccmp_nest_t *nest_next = nccmp_create_nest();

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_compound_field\n");
    }

    field1 = & nest->type1->fields->items[fieldid1];
    field2 = & nest->type2->fields->items[fieldid2];
    nest_next->type1 = field1;
    nest_next->type2 = field2;
    nest_next->num_dims = field1->num_dims;
    nest_next->prev = nest;
    data1 += field1->offset;
    data2 += field2->offset;

    last_status = nccmp_cmp_var_user_type_nest_select(state, ncid1, ncid2,
                        varid1, varid2, start, item, data1, data2,
                        nest_next);
    nccmp_destroy_nest(nest_next);

    return last_status;
}

int nccmp_cmp_var_user_type_nest_compound(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, unsigned char *data1, unsigned char *data2,
        nccmp_nest_t *nest)
{
    int i, status, last_status = EXIT_SUCCESS;
    nccmp_int_pairs_t *fieldids = NULL;
    int cardinal_index;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_compound\n");
    }

    fieldids = ncccmp_get_user_type_compound_field_index_pairs(
                nest->type1, nest->type2, state->opts.debug);

    cardinal_index = nccmp_nest_cardinal_index(nest);

    data1 += nest->type1->size * cardinal_index;
    data2 += nest->type2->size * cardinal_index;

    for(i = 0; i < fieldids->size; ++i) {
        status = nccmp_cmp_var_user_type_nest_compound_field(
                    state, ncid1, ncid2,
                    varid1, varid2, start, item, data1, data2,
                    nest,
                    fieldids->items[i].first,
                    fieldids->items[i].second);
        last_status = status ? status : last_status;
        if ((last_status && !state->opts.force) || nccmp_check_diff_count(state, varid1)) {
            goto recover;
        }
    }

recover:
    nccmp_destroy_int_pairs(fieldids);

    return last_status;
}

int nccmp_cmp_var_user_type_nest_enum(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest)
{
    int status, last_status = EXIT_SUCCESS;
    int diff_count_status;
    char idx_str[32], str1[NC_MAX_NAME], str2[NC_MAX_NAME], nest_str[NC_MAX_NAME];
    long long value1, value2;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_enum\n");
    }

    if (nccmp_check_diff_count(state, varid1)) {
        goto recover;
    }

    idx_str[0] = 0;
    str1[0] = 0;
    str2[0] = 0;

#define CMP_ENUM(TYPE1, TYPE2)                                                 \
        value1 = (long long) *( (TYPE1*) data1 + nest->index[0] );             \
        value2 = (long long) *( (TYPE2*) data2 + nest->index[0] );             \
        status = nc_inq_enum_ident(ncid1, nest->type1->type_id, value1, str1); \
        HANDLE_NC_ERROR(status);                                               \
        status = nc_inq_enum_ident(ncid2, nest->type2->type_id, value2, str2); \
        HANDLE_NC_ERROR(status);                                               \
        if (strcmp(str1, str2)) {                                              \
            if (!state->opts.warn[NCCMP_W_ALL]) {                              \
                last_status = EXIT_DIFFER;                                     \
            }                                                                  \
            diff_count_status = nccmp_inc_diff_count(state, varid1);           \
            if (COUNT_OVER == diff_count_status) {                             \
                goto recover;                                                  \
            }                                                                  \
            if (!state->opts.quiet) {                                          \
                nccmp_get_index_str_size_t(state->vars1[varid1].ndims, start, item, idx_str, state->opts.fortran);  \
                nest_str[0] = 0;                                                                                    \
                nccmp_build_nest_str(nest, state->opts.fortran, nest_str);                                          \
                PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD, state->vars1[varid1].name,                            \
                           idx_str, nest_str, str1, str2);                                                          \
            }                                                                                                       \
            if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {   \
                goto recover;                                                  \
            }                                                                  \
        }

#define CMP_ENUM_TYPE(TYPE1)                                                   \
    switch(nest->type2->base_type) {                                           \
        case NC_BYTE:   CMP_ENUM(TYPE1, signed char);       break;             \
        case NC_CHAR:   CMP_ENUM(TYPE1, char);              break;             \
        case NC_INT:    CMP_ENUM(TYPE1, int);               break;             \
        case NC_INT64:  CMP_ENUM(TYPE1, long long);         break;             \
        case NC_SHORT:  CMP_ENUM(TYPE1, short);             break;             \
        case NC_UBYTE:  CMP_ENUM(TYPE1, unsigned char);     break;             \
        case NC_UINT:   CMP_ENUM(TYPE1, unsigned int);      break;             \
        case NC_UINT64: CMP_ENUM(TYPE1, unsigned long long);break;             \
        case NC_USHORT: CMP_ENUM(TYPE1, unsigned short);    break;             \
        default:                                                               \
             LOG_ERROR("Unsupported second enum data type = %d\n", nest->type2->base_type);        \
             last_status = EXIT_FAILED;                                        \
             break;                                                            \
    }

    switch(nest->type1->base_type) {
        case NC_BYTE:   CMP_ENUM_TYPE(signed char);       break;
        case NC_CHAR:   CMP_ENUM_TYPE(char);              break;
        case NC_INT:    CMP_ENUM_TYPE(int);               break;
        case NC_INT64:  CMP_ENUM_TYPE(long long);         break;
        case NC_SHORT:  CMP_ENUM_TYPE(short);             break;
        case NC_UBYTE:  CMP_ENUM_TYPE(unsigned char);     break;
        case NC_UINT:   CMP_ENUM_TYPE(unsigned int);      break;
        case NC_UINT64: CMP_ENUM_TYPE(unsigned long long);break;
        case NC_USHORT: CMP_ENUM_TYPE(unsigned short);    break;
        default:
             LOG_ERROR("Unsupported first enum data type = %d\n", nest->type1->base_type);
             last_status = EXIT_FAILED;
             break;
    }

#undef CMP_ENUM
#undef CMP_ENUM_TYPE

recover:

    return last_status;
}

int nccmp_cmp_var_user_type_nest_opaque(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest)
{
    int last_status = EXIT_SUCCESS;
    unsigned char *bytes1, *bytes2;
    int nbytes, diff_count_status;
    char idx_str[32], *str1 = 0, *str2 = 0, nest_str[NC_MAX_NAME];

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_opaque\n");
    }

    nbytes = nest->type1->size;
    bytes1 = (unsigned char*) data1 + nest->index[0] * nbytes;
    bytes2 = (unsigned char*) data2 + nest->index[0] * nbytes;

    if (memcmp(bytes1, bytes2, nbytes)) {
        if (!state->opts.warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        diff_count_status = nccmp_inc_diff_count(state, varid1);
        if (COUNT_OVER == diff_count_status) {
            goto recover;
        }
        if (!state->opts.quiet) {
            nccmp_get_index_str_size_t(state->vars1[varid1].ndims, start, item, idx_str, state->opts.fortran);
            str1 = nccmp_realloc_bytes_hex_str(str1, nbytes);
            str2 = nccmp_realloc_bytes_hex_str(str2, nbytes);
            nccmp_bytes_to_hex_str(bytes1, nbytes, str1);
            nccmp_bytes_to_hex_str(bytes2, nbytes, str2);
            nest_str[0] = 0;
            nccmp_build_nest_str(nest, state->opts.fortran, nest_str);
            PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD, state->vars1[varid1].name,
                       idx_str, nest_str, str1, str2);
        }
        if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
            goto recover;
        }
    }

recover:
    XFREE(str1);
    XFREE(str2);

    return last_status;
}

/* Process a vlen, which might be an item in another structure. */
int nccmp_cmp_var_user_type_nest_vlen(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest)
{
    int status, last_status = EXIT_SUCCESS;
    nc_vlen_t *vlen1, *vlen2;
    char idx_str[32], nest_str[NC_MAX_NAME];
    int diff_count_status, minlen = 0, vlen_i, i;
    char **strings;
    nccmp_nest_t *nest_next = nccmp_create_nest();
    nccmp_user_type_t *atomic1 = 0, *atomic2 = 0;
    int cardinal_index;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_vlen\n");
    }

    cardinal_index = nccmp_nest_cardinal_index(nest);
    vlen1 = ((nc_vlen_t*) data1) + cardinal_index;
    vlen2 = ((nc_vlen_t*) data2) + cardinal_index;

    if (vlen1->len != vlen2->len) {
        if (!state->opts.warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        diff_count_status = nccmp_inc_diff_count(state, varid1);
        if (COUNT_OVER == diff_count_status) {
            goto recover;
        }
        if (!state->opts.quiet) {
            nccmp_get_index_str_size_t(state->vars1[varid1].ndims, start, item, idx_str, state->opts.fortran);
            nest_str[0] = 0;
            nccmp_build_nest_str(nest, state->opts.fortran, nest_str);
            PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD_VLEN,
                       state->vars1[varid1].name, idx_str, nest_str,
                       vlen1->len, vlen2->len);
        }
        if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
            goto recover;
        }
    }

    if (nest->type1->base_type < NC_FIRSTUSERTYPEID) {
        atomic1 = nccmp_create_user_type(1);
        atomic1->type_id = nest->type1->base_type;
        atomic2 = nccmp_create_user_type(1);
        atomic2->type_id = nest->type2->base_type;
        nest_next->type1 = atomic1;
        nest_next->type2 = atomic2;
    } else {
        nest_next->type1 = nccmp_get_user_type_by_id(
                                state->types1->items,
                                state->types1->num_items,
                                nest->type1->base_type);
        nest_next->type2 = nccmp_get_user_type_by_id(
                                state->types2->items,
                                state->types2->num_items,
                                nest->type2->base_type);
    }
    nest_next->prev = nest;
    nest_next->num_dims = 1;
    minlen = nccmp_min_size_t(vlen1->len, vlen2->len);
    for(vlen_i = 0; vlen_i < minlen; ++vlen_i) {
        nest_next->index[0] = vlen_i;
        last_status = nccmp_cmp_var_user_type_nest_select(state, ncid1, ncid2,
                        varid1, varid2, start, item,
                        vlen1->p, vlen2->p, nest_next);
        if ((last_status && !state->opts.force) || nccmp_check_diff_count(state, varid1)) {
            goto recover;
        }
    }

recover:
    // Uncompared strings must still be freed (those beyond minlen).
    if (NC_STRING == nest_next->type1->type_id) {
        for(i = minlen; i < vlen1->len; ++i) {
            strings = (char**) vlen1->p + i;
			status = nc_free_string(1, strings);
			HANDLE_NC_ERROR(status);
        }
    }
    if (NC_STRING == nest_next->type2->type_id) {
        for(i = minlen; i < vlen2->len; ++i) {
            strings = (char**) vlen2->p + i;
			status = nc_free_string(1, strings);
			HANDLE_NC_ERROR(status);			
        }
    }

    nccmp_destroy_nest(nest_next);

    if (vlen1) {
        status = nc_free_vlen(vlen1);
        HANDLE_NC_ERROR(status);
    }

    if (vlen2) {
        status = nc_free_vlen(vlen2);
        HANDLE_NC_ERROR(status);
    }

    nccmp_destroy_user_type(atomic1);
    nccmp_destroy_user_type(atomic2);

    return last_status;
}

int nccmp_cmp_var_user_type_nest_select(nccmp_state_t* state,
        int ncid1, int ncid2,
        int varid1, int varid2, size_t* start,
        int item, void *data1, void *data2, nccmp_nest_t *nest)
{
    int last_status = EXIT_SUCCESS;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_select\n");
    }

    if (nest->type1->type_id < NC_FIRSTUSERTYPEID) {
        last_status = nccmp_cmp_var_user_type_nest_atomic(
                        state, ncid1, ncid2, varid1, varid2,
                        start, item, data1, data2, nest);
    } else {
        switch(nest->type1->user_class) {
            case NC_COMPOUND:
                last_status = nccmp_cmp_var_user_type_nest_compound(
                                state, ncid1, ncid2, varid1, varid2,
                                start, item, data1, data2, nest);
                break;
            case NC_ENUM:
                last_status = nccmp_cmp_var_user_type_nest_enum(
                                state, ncid1, ncid2, varid1, varid2,
                                start, item, data1, data2, nest);
                break;
            case NC_OPAQUE:
                last_status = nccmp_cmp_var_user_type_nest_opaque(
                                state, ncid1, ncid2, varid1, varid2,
                                start, item, data1, data2, nest);
                break;
            case NC_VLEN:
                last_status = nccmp_cmp_var_user_type_nest_vlen(
                                state, ncid1, ncid2, varid1, varid2,
                                start, item, data1, data2, nest);
                break;
            default:
                break;
        }
    }

    return last_status;
}

int nccmp_cmp_var_user_type_nest_vlen_start(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int item, int rec, size_t* limits,
        unsigned char *data1, unsigned char *data2)
{
    int status, last_status = EXIT_SUCCESS;
    nccmp_nest_t *nest;
    nc_vlen_t *vlen1, *vlen2;
    int diff_count_status, minlen, vlen_i;
    char idx_str[NC_MAX_NAME], nest_str[NC_MAX_NAME];
    nccmp_nest_t *nest_next;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest_vlen_start\n");
    }

    nest = nccmp_create_nest();
    nest->type1 = nccmp_get_user_type_by_id(
                                    state->types1->items,
                                    state->types1->num_items,
                                    state->vars1[varid1].type);
    nest->type2 = nccmp_get_user_type_by_id(
                                    state->types2->items,
                                    state->types2->num_items,
                                    state->vars2[varid2].type);
    nest_next = nccmp_create_nest();
    nest_next->type1 = nccmp_get_user_type_by_id(
                            state->types1->items,
                            state->types1->num_items,
                            nest->type1->base_type);
    nest_next->type2 = nccmp_get_user_type_by_id(
                            state->types2->items,
                            state->types2->num_items,
                            nest->type2->base_type);
    nest_next->prev = nest;
    nest_next->num_dims = 1;

    vlen1 = (nc_vlen_t*) data1;
    vlen2 = (nc_vlen_t*) data2;

    if (vlen1->len != vlen2->len) {
        if (!state->opts.warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        diff_count_status = nccmp_inc_diff_count(state, varid1);
        if (COUNT_OVER == diff_count_status) {
            goto recover;
        }
        if (!state->opts.quiet) {
            nccmp_get_index_str_size_t(state->vars1[varid1].ndims, start, item, idx_str, state->opts.fortran);
            nest_str[0] = 0;
            nccmp_build_nest_str(nest, state->opts.fortran, nest_str);
            PRINT_DIFF(state->opts.debug, DIFF_MSG_FIELD_VLEN,
                       state->vars1[varid1].name, idx_str, nest_str, vlen1->len, vlen2->len);
        }
        if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
            goto recover;
        }
    }

    minlen = nccmp_min_size_t(vlen1->len, vlen2->len);

    for(vlen_i = 0; vlen_i < minlen; ++vlen_i) {
        nest_next->index[0] = vlen_i;
        last_status = nccmp_cmp_var_user_type_nest_select(state, ncid1, ncid2,
                        varid1, varid2, start, item, vlen1->p, vlen2->p, nest_next);
        if ((last_status && !state->opts.force) || nccmp_check_diff_count(state, varid1)) {
            goto recover;
        }
    }

recover:
    nccmp_destroy_nest(nest_next);
    nccmp_destroy_nest(nest);

    if (vlen1) {
        status = nc_free_vlen(vlen1);
        HANDLE_NC_ERROR(status);
    }

    if (vlen2) {
        status = nc_free_vlen(vlen2);
        HANDLE_NC_ERROR(status);
    }

    return last_status;
}

int nccmp_cmp_var_user_type_nest(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits)
{
    int status, last_status = EXIT_SUCCESS;
    int item, loop_done = 0;
    unsigned char *items1, *items2;
    size_t size1, size2;
    nc_type type_id1, type_id2;
    nccmp_user_type_t *type1, *type2;
    nccmp_var_t *v1, *v2;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_nest\n");
    }
    v1 = & state->vars1[varid1];
    v2 = & state->vars2[varid2];

    type_id1 = v1->type;
    type_id2 = v2->type;

    type1 = nccmp_get_user_type_by_id(state->types1->items,
                                      state->types1->num_items,
                                      type_id1);
    type2 = nccmp_get_user_type_by_id(state->types2->items,
                                      state->types2->num_items,
                                      type_id2);
    size1 = type1->size;
    size2 = type2->size;
    items1 = XMALLOC(unsigned char, size1);
    items2 = XMALLOC(unsigned char, size2);

#define CMP_NEST(NAME)                                                         \
    while(! loop_done) {                                                       \
        status = nc_get_vara(ncid1, v1->varid, start, count, items1);          \
        HANDLE_NC_ERROR(status);                                               \
        status = nc_get_vara(ncid2, v2->varid, start, count, items2);          \
        HANDLE_NC_ERROR(status);                                               \
        for(item = 0; item < nitems; ++item) {                                 \
            last_status = nccmp_cmp_var_user_type_nest_##NAME##_start(state, ncid1, ncid2,         \
                    varid1, varid2, start, count, item, rec, limits,           \
                    items1 + size1 * item, items2 + size2 * item);             \
            if (last_status && !state->opts.force) {                           \
                goto recover;                                                  \
            }                                                                  \
        }                                                                      \
        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;   \
    }

    switch(type1->user_class) {
    case NC_VLEN:     CMP_NEST(vlen);     break;
    default:
        LOG_ERROR("Unsupported type=%s for nested comparisons.\n", type1->name);
        break;
    }

#undef CMP_NEST

recover:
    XFREE(items1);
    XFREE(items2);

    return last_status;
}

int nccmp_cmp_var_user_type_opaque(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits)
{
    int status, last_status = EXIT_SUCCESS;
    nccmp_var_t *v1, *v2;
    unsigned char *items1, *items2;
    size_t base_size;
    char str1[NC_MAX_NAME], str2[NC_MAX_NAME], idx_str[32];

    size_t diff_offset, cmplen, offset, aligned, nbytes;
    count_limit_type diff_count_status;
    int loop_done = 0;

    diff_count_status = nccmp_check_diff_count(state, varid1);
    if (diff_count_status) {
        return last_status;
    }

    v1 = & state->vars1[varid1];
    v2 = & state->vars2[varid2];

    status = nc_inq_opaque(ncid1, v1->type, 0 /*name*/, & base_size);
    HANDLE_NC_ERROR(status);

    // Treat as one contiguous byte stream.
    nbytes = base_size * nitems;
    items1 = XMALLOC(unsigned char, nbytes + 1);
    items2 = XMALLOC(unsigned char, nbytes + 1);

    while(! loop_done) {
        status = nc_get_vara(ncid1, v1->varid, start, count, items1);
        HANDLE_NC_ERROR(status);
        status = nc_get_vara(ncid2, v2->varid, start, count, items2);
        HANDLE_NC_ERROR(status);
        items1[nbytes] = 0;
        items2[nbytes] = 1;

        offset = 0;
        while (1) {
            diff_offset = nccmp_cmp_uchar_uchar(items1+offset, items2+offset);
            cmplen = nbytes - offset;
            if (diff_offset >= cmplen)    {
                break;
            }
            aligned = (offset + diff_offset) / base_size;
            offset = (aligned + 1) * base_size;
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            } else {
                last_status = EXIT_SUCCESS;
            }
            diff_count_status = nccmp_inc_diff_count(state, varid1);
            if (COUNT_OVER == diff_count_status) {
                goto recover;
            }
            if (!state->opts.quiet) {
                nccmp_get_index_str_size_t(v1->ndims, start, (offset-base_size)/base_size, idx_str, state->opts.fortran);
                nccmp_bytes_to_hex_str(items1 + offset - base_size, base_size, str1);
                nccmp_bytes_to_hex_str(items2 + offset - base_size, base_size, str2);
                PRINT_DIFF(state->opts.debug, DIFF_MSG, v1->name, idx_str, str1, str2);
            }
            if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
                goto recover;
            }
        }

        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;
    }
recover:
    XFREE(items1);
    XFREE(items2);

    return last_status;
}

int nccmp_cmp_var_user_type_vlen_atomic_string(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits, nc_vlen_t *items1,
        nc_vlen_t *items2)
{
    int i, j, minlen, status, last_status = EXIT_SUCCESS;
    char **s1, **s2;
    char idx_str[NC_MAX_NAME];
    int loop_done = 0;
    count_limit_type diff_count_status;
    nccmp_var_t *v1, *v2;
    char should_exit = 0;
    char j_str[32];

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type_vlen_atomic_string\n");
    }

    j_str[0] = 0;
    v1 = & state->vars1[varid1];
    v2 = & state->vars2[varid2];

    while(!loop_done && !should_exit) {
        status = nc_get_vara(ncid1, v1->varid, start, count, items1);
        HANDLE_NC_ERROR(status);
        status = nc_get_vara(ncid2, v2->varid, start, count, items2);
        HANDLE_NC_ERROR(status);

        for(i = 0; i < nitems; ++i) {
            if (items1[i].len != items2[i].len) {
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                diff_count_status = nccmp_inc_diff_count(state, varid1);
                if (COUNT_OVER == diff_count_status) {
                    should_exit = 1;
                    goto recover;
                }
                if (!state->opts.quiet) {
                    nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);
                    PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : POSITION : [%s] : VARIABLE LENGTH : %zu <> %zu\n",
                               v1->name, idx_str, items1[i].len, items2[i].len);
                }
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
                    should_exit = 1;
                    goto recover;
                }
            }

            s1 = (char**) items1[i].p;
            s2 = (char**) items2[i].p;
            minlen = nccmp_min_size_t(items1[i].len, items2[i].len);
            for(j = 0; j < minlen; ++j) {
                if ( ( s1[j] && !s2[j]) ||
                     (!s1[j] &&  s2[j]) ||
                     ( s1[j] &&  s2[j] && strcmp(s1[j], s2[j]))
                   ) {
                    if (!state->opts.warn[NCCMP_W_ALL]) {
                        last_status = EXIT_DIFFER;
                    }
                    diff_count_status = nccmp_inc_diff_count(state, varid1);
                    if (COUNT_OVER == diff_count_status) {
                        should_exit = 1;
                        goto recover;
                    }
                    if (!state->opts.quiet) {
                        nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);
                        sprintf(j_str, "%d", state->opts.fortran ? j + 1 : j);
                        PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : POSITION : [%s,%s] : VALUES : %s <> %s\n",
                                   v1->name,
                                   state->opts.fortran ? j_str : idx_str,
                                   state->opts.fortran ? idx_str : j_str,
                                   s1[j], s2[j]);
                    }
                    if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force ) {
                        should_exit = 1;
                        goto recover;
                    }
                }
            }
        }
        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;
recover:
        // Free api allocated strings.
        for(i = 0; i < nitems; ++i) {
            s1 = (char**) items1[i].p;
            s2 = (char**) items2[i].p;
			status = nc_free_string(items1[i].len, s1);
			HANDLE_NC_ERROR(status);
			status = nc_free_string(items2[i].len, s2);
			HANDLE_NC_ERROR(status);
        }
    }

    return last_status;
}

int nccmp_cmp_var_user_type_vlen_atomic(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits)
{
    int i, j, status, last_status = EXIT_SUCCESS;
    nccmp_var_t *v1, *v2;
    nc_vlen_t *items1 = 0, *items2 = 0;
    size_t minlen;
    char str1[NC_MAX_NAME], str2[NC_MAX_NAME], idx_str[32];
    nccmp_nc_type_ptr_t list1, list2;
    nc_type base_type1, base_type2;
    count_limit_type diff_count_status;
    int loop_done = 0;
    char j_str[32];

    diff_count_status = nccmp_check_diff_count(state, varid1);
    if (diff_count_status) {
        return last_status;
    }

    j_str[0] = 0;
    v1 = & state->vars1[varid1];
    v2 = & state->vars2[varid2];

    status = nc_inq_vlen(ncid1, v1->type, 0 /*name*/, 0 /*datum_size*/, & base_type1);
    HANDLE_NC_ERROR(status);
    status = nc_inq_vlen(ncid2, v2->type, 0 /*name*/, 0 /*datum_size*/, & base_type2);
    HANDLE_NC_ERROR(status);

#define CMP_VLEN(TYPE1, NAME1, TYPE2, NAME2)                                   \
    while(! loop_done) {                                                       \
        status = nc_get_vara(ncid1, v1->varid, start, count, items1);          \
        HANDLE_NC_ERROR(status);                                               \
        status = nc_get_vara(ncid2, v2->varid, start, count, items2);          \
        HANDLE_NC_ERROR(status);                                               \
                                                                               \
        for(i = 0; i < nitems; ++i) {                                          \
            if (items1[i].len != items2[i].len) {                              \
                if (!state->opts.warn[NCCMP_W_ALL]) {                          \
                    last_status = EXIT_DIFFER;                                 \
                }                                                              \
                diff_count_status = nccmp_inc_diff_count(state, varid1);       \
                if (COUNT_OVER == diff_count_status) {                         \
                    goto recover;                                              \
                }                                                              \
                if (!state->opts.quiet) {                                      \
                    nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);                              \
                    PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : POSITION : [%s] : VARIABLE LENGTH : %zu <> %zu\n",  \
                               v1->name, idx_str, items1[i].len, items2[i].len);                                                \
                }                                                                                                               \
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {                                                \
                    goto recover;                                              \
                }                                                              \
            }                                                                  \
                                                                               \
            list1.p_##NAME1 = (TYPE1*) items1[i].p;                            \
            list2.p_##NAME2 = (TYPE2*) items2[i].p;                            \
            minlen = nccmp_min_size_t(items1[i].len, items2[i].len);           \
            for(j = 0; j < minlen; ++j) {                                      \
                if ( *(list1.p_##NAME1 + j) != *(list2.p_##NAME2 + j)) {       \
                    if (!state->opts.warn[NCCMP_W_ALL]) {                      \
                        last_status = EXIT_DIFFER;                             \
                    }                                                          \
                    diff_count_status = nccmp_inc_diff_count(state, varid1);   \
                    if (COUNT_OVER == diff_count_status) {                     \
                        goto recover;                                          \
                    }                                                          \
                    if (!state->opts.quiet) {                                                                               \
                        nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);                      \
                        nccmp_##NAME1##_to_str(*(list1.p_##NAME1 + j), str1, state->opts.precision);                        \
                        nccmp_##NAME2##_to_str(*(list2.p_##NAME2 + j), str2, state->opts.precision);                        \
                        sprintf(j_str, "%d", state->opts.fortran ? j + 1 : j);                                              \
                        PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : POSITION : [%s,%s] : VALUES : %s <> %s\n",  \
                                v1->name,                                                                                   \
                                state->opts.fortran ? j_str : idx_str,                                                      \
                                state->opts.fortran ? idx_str : j_str,                                                      \
                                str1, str2);                                                                                \
                    }                                                                                                       \
                    if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {                                        \
                        goto recover;                                          \
                    }                                                          \
                }                                                              \
            }                                                                  \
        }                                                                      \
                                                                               \
        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;   \
    }

    items1 = XMALLOC(nc_vlen_t, nitems);
    items2 = XMALLOC(nc_vlen_t, nitems);

#define CMP_VLEN_TYPE_ONE(TYPE1, NAME1)                                        \
        switch(base_type2) {                                                   \
            case NC_BYTE:   CMP_VLEN(TYPE1, NAME1, signed char,        schar);     break;\
            case NC_CHAR:   CMP_VLEN(TYPE1, NAME1, char,               text);      break;\
            case NC_DOUBLE: CMP_VLEN(TYPE1, NAME1, double,             double);    break;\
            case NC_FLOAT:  CMP_VLEN(TYPE1, NAME1, float,              float);     break;\
            case NC_INT:    CMP_VLEN(TYPE1, NAME1, int,                int);       break;\
            case NC_INT64:  CMP_VLEN(TYPE1, NAME1, long long,          longlong);  break;\
            case NC_SHORT:  CMP_VLEN(TYPE1, NAME1, short,              short);     break;\
            case NC_UBYTE:  CMP_VLEN(TYPE1, NAME1, unsigned char,      uchar);     break;\
            case NC_UINT:   CMP_VLEN(TYPE1, NAME1, unsigned int,       uint);      break;\
            case NC_UINT64: CMP_VLEN(TYPE1, NAME1, unsigned long long, ulonglong); break;\
            case NC_USHORT: CMP_VLEN(TYPE1, NAME1, unsigned short,     ushort);    break;\
            default:                                                           \
                 LOG_ERROR("Unsupported variable-length data type = %d\n", base_type2);  \
                 last_status = EXIT_FAILED;                                    \
                 break;                                                        \
        }

    switch(base_type1) {
        case NC_BYTE:   CMP_VLEN_TYPE_ONE(signed char,        schar);     break;
        case NC_CHAR:   CMP_VLEN_TYPE_ONE(char,               text);      break;
        case NC_DOUBLE: CMP_VLEN_TYPE_ONE(double,             double);    break;
        case NC_FLOAT:  CMP_VLEN_TYPE_ONE(float,              float);     break;
        case NC_INT:    CMP_VLEN_TYPE_ONE(int,                int);       break;
        case NC_INT64:  CMP_VLEN_TYPE_ONE(long long,          longlong);  break;
        case NC_SHORT:  CMP_VLEN_TYPE_ONE(short,              short);     break;
        case NC_UBYTE:  CMP_VLEN_TYPE_ONE(unsigned char,      uchar);     break;
        case NC_UINT:   CMP_VLEN_TYPE_ONE(unsigned int,       uint);      break;
        case NC_UINT64: CMP_VLEN_TYPE_ONE(unsigned long long, ulonglong); break;
        case NC_USHORT: CMP_VLEN_TYPE_ONE(unsigned short,     ushort);    break;
        case NC_STRING:
            if (NC_STRING == base_type2) {
                last_status = nccmp_cmp_var_user_type_vlen_atomic_string(state, ncid1,
                        ncid2, varid1, varid2, start, count, nitems, rec, limits,
                        items1, items2);
                break;
            }
        default:
             LOG_ERROR("Unsupported variable-length data type = %d\n", base_type1);
             last_status = EXIT_FAILED;
             break;
    }

#undef CMP_VLEN
#undef CMP_VLEN_TYPE_ONE

recover:
    if (items1) {
        status = nc_free_vlens(nitems, items1);
        HANDLE_NC_ERROR(status);
        XFREE(items1);
    }
    if (items2) {
        status = nc_free_vlens(nitems, items2);
        HANDLE_NC_ERROR(status);
        XFREE(items2);
    }

    return last_status;
}

int nccmp_cmp_var_user_type_vlen_enum(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits)
{
    nc_type enum_base_type1, enum_base_type2, vlen_base_type1, vlen_base_type2;
    count_limit_type diff_count_status;
    nc_vlen_t *items1 = NULL, *items2 = NULL;
    int loop_done = 0;
    int i, j, status, last_status = EXIT_SUCCESS;
    nccmp_var_t *v1 = NULL, *v2 = NULL;
    char str1[NC_MAX_NAME], str2[NC_MAX_NAME], idx_str[32];
    size_t minlen;
    nccmp_nc_type_ptr_t list1, list2;
    char j_str[32];

    j_str[0] = 0;
    v1 = & state->vars1[varid1];
    v2 = & state->vars2[varid2];

    status = nc_inq_vlen(ncid1, v1->type, 0 /*name*/, 0 /*datum_size*/, & vlen_base_type1);
    HANDLE_NC_ERROR(status);
    status = nc_inq_vlen(ncid2, v2->type, 0 /*name*/, 0 /*datum_size*/, & vlen_base_type2);
    HANDLE_NC_ERROR(status);

    status = nc_inq_enum(ncid1, vlen_base_type1, 0 /*name*/, & enum_base_type1,  0 /*base_size*/, 0 /*num_members*/);
    HANDLE_NC_ERROR(status);
    status = nc_inq_enum(ncid2, vlen_base_type2, 0 /*name*/, & enum_base_type2,  0 /*base_size*/, 0 /*num_members*/);
    HANDLE_NC_ERROR(status);

    items1 = XMALLOC(nc_vlen_t, nitems);
    items2 = XMALLOC(nc_vlen_t, nitems);

#define CMP_ENUM(NAME1, TYPE1, NAME2, TYPE2) {                                 \
    while(! loop_done) {                                                       \
        status = nc_get_vara(ncid1, v1->varid, start, count, items1);          \
        HANDLE_NC_ERROR(status);                                               \
        status = nc_get_vara(ncid2, v2->varid, start, count, items2);          \
        HANDLE_NC_ERROR(status);                                               \
                                                                               \
        for(i = 0; i < nitems; ++i) {                                          \
            if (items1[i].len != items2[i].len) {                              \
                if (!state->opts.warn[NCCMP_W_ALL]) {                          \
                    last_status = EXIT_DIFFER;                                 \
                }                                                              \
                diff_count_status = nccmp_inc_diff_count(state, varid1);       \
                if (COUNT_OVER == diff_count_status) {                         \
                    goto recover;                                              \
                }                                                              \
                if (!state->opts.quiet) {                                                                                           \
                    nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);                                  \
                    PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : POSITION : [%s] : VARIABLE LENGTH : %zu <> %zu\n",      \
                               v1->name, idx_str, items1[i].len, items2[i].len);                                                    \
                }                                                                                                                   \
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {                                                    \
                    goto recover;                                              \
                }                                                              \
            }                                                                  \
                                                                               \
            list1.p_##NAME1 = (TYPE1*) items1[i].p;                            \
            list2.p_##NAME2 = (TYPE2*) items2[i].p;                            \
            minlen = nccmp_min_size_t(items1[i].len, items2[i].len);           \
            for(j = 0; j < minlen; ++j) {                                      \
                status = nc_inq_enum_ident(ncid1, vlen_base_type1, (long long) list1.p_##NAME1[j], str1);    \
                HANDLE_NC_ERROR(status);                                       \
                status = nc_inq_enum_ident(ncid2, vlen_base_type2, (long long) list2.p_##NAME2[j], str2);    \
                HANDLE_NC_ERROR(status);                                       \
                if (strcmp(str1, str2)) {                                      \
                    if (!state->opts.warn[NCCMP_W_ALL]) {                      \
                        last_status = EXIT_DIFFER;                             \
                    }                                                          \
                    diff_count_status = nccmp_inc_diff_count(state, varid1);   \
                    if (COUNT_OVER == diff_count_status) {                     \
                        goto recover;                                          \
                    }                                                          \
                    if (!state->opts.quiet) {                                  \
                        nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);                      \
                        sprintf(j_str, "%d", state->opts.fortran ? j + 1 : j);                                              \
                        PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : POSITION : [%s,%s] : VALUES : %s <> %s\n",  \
                                   v1->name,                                                                                \
                                   state->opts.fortran ? j_str : idx_str,                                                   \
                                   state->opts.fortran ? idx_str : j_str,                                                   \
                                   str1, str2);                                                                             \
                    }                                                                                                       \
                    if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {                                        \
                        goto recover;                                          \
                    }                                                          \
                }                                                              \
            }                                                                  \
        }                                                                      \
        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;   \
    }                                                                          \
}

#define CMP_ENUM_TYPE(NAME1, TYPE1)                                            \
    switch(enum_base_type2) {                                                  \
    case NC_BYTE:   CMP_ENUM(NAME1, TYPE1, schar,    signed char);       break;\
    case NC_CHAR:   CMP_ENUM(NAME1, TYPE1, text,     char);              break;\
    case NC_INT:    CMP_ENUM(NAME1, TYPE1, int,      int);               break;\
    case NC_INT64:  CMP_ENUM(NAME1, TYPE1, longlong, long long);         break;\
    case NC_SHORT:  CMP_ENUM(NAME1, TYPE1, short,    short);             break;\
    case NC_UBYTE:  CMP_ENUM(NAME1, TYPE1, uchar,    unsigned char);     break;\
    case NC_UINT:   CMP_ENUM(NAME1, TYPE1, uint,     unsigned int);      break;\
    case NC_UINT64: CMP_ENUM(NAME1, TYPE1, ulonglong,unsigned long long);break;\
    case NC_USHORT: CMP_ENUM(NAME1, TYPE1, ushort,   unsigned short);    break;\
    default:                                                                   \
         LOG_ERROR("Unsupported second enum data type = %d\n", enum_base_type2);         \
         last_status = EXIT_FAILED;                                            \
         break;                                                                \
    }

    switch(enum_base_type1) {
    case NC_BYTE:   CMP_ENUM_TYPE(schar,    signed char);       break;
    case NC_CHAR:   CMP_ENUM_TYPE(text,     char);              break;
    case NC_INT:    CMP_ENUM_TYPE(int,      int);               break;
    case NC_INT64:  CMP_ENUM_TYPE(longlong, long long);         break;
    case NC_SHORT:  CMP_ENUM_TYPE(short,    short);             break;
    case NC_UBYTE:  CMP_ENUM_TYPE(uchar,    unsigned char);     break;
    case NC_UINT:   CMP_ENUM_TYPE(uint,     unsigned int);      break;
    case NC_UINT64: CMP_ENUM_TYPE(ulonglong,unsigned long long);break;
    case NC_USHORT: CMP_ENUM_TYPE(ushort,   unsigned short);    break;
    default:
         LOG_ERROR("Unsupported first enum data type = %d\n", enum_base_type1);
         last_status = EXIT_FAILED;
         break;
    }

#undef CMP_ENUM
#undef CMP_ENUM_TYPE

recover:
    if (items1) {
        status = nc_free_vlens(nitems, items1);
        HANDLE_NC_ERROR(status);
        XFREE(items1);
    }
    if (items2) {
        status = nc_free_vlens(nitems, items2);
        HANDLE_NC_ERROR(status);
        XFREE(items2);
    }

    return last_status;
}

int nccmp_cmp_var_user_type_vlen_opaque(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int num_vlens, int rec, size_t* limits)
{
    int vlen_i, opaq_i, byte_i, status, num_opaq_min, last_status = EXIT_SUCCESS;
    count_limit_type diff_count_status;
    int loop_done = 0;
    nc_type opaque_type_id;
    nccmp_var_t *v1, *v2;
    size_t num_bytes;
    unsigned char *bytes1, *bytes2, *opaq1, *opaq2;
    char str1[NC_MAX_NAME], str2[NC_MAX_NAME], idx_str[32], vlen_idx_str[32];
    nc_vlen_t *vlens1, *vlens2;

    diff_count_status = nccmp_check_diff_count(state, varid1);
    if (diff_count_status) {
        return last_status;
    }

    v1 = & state->vars1[varid1];
    v2 = & state->vars2[varid2];

    status = nc_inq_vlen(ncid1, v1->type, 0 /*name*/, 0 /*datum_size*/, & opaque_type_id);
    HANDLE_NC_ERROR(status);

    status = nc_inq_opaque(ncid1, opaque_type_id, 0 /*name*/, & num_bytes);
    HANDLE_NC_ERROR(status);

    vlens1 = XMALLOC(nc_vlen_t, num_vlens);
    vlens2 = XMALLOC(nc_vlen_t, num_vlens);

    while(! loop_done) {
        status = nc_get_vara(ncid1, v1->varid, start, count, vlens1);
        HANDLE_NC_ERROR(status);
        status = nc_get_vara(ncid2, v2->varid, start, count, vlens2);
        HANDLE_NC_ERROR(status);

        for(vlen_i = 0; vlen_i < num_vlens; ++vlen_i) {
            if (vlens1[vlen_i].len != vlens2[vlen_i].len) {
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                diff_count_status = nccmp_inc_diff_count(state, varid1);
                if (COUNT_OVER == diff_count_status) {
                    goto recover;
                }
                if (!state->opts.quiet) {
                    nccmp_get_index_str_size_t(v1->ndims, start, vlen_i, idx_str, state->opts.fortran);
                    PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : POSITION : [%s] : VARIABLE LENGTH : %zu <> %zu\n",
                               v1->name, idx_str, vlens1[vlen_i].len, vlens2[vlen_i].len);
                }
                if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
                    goto recover;
                }
            }
            num_opaq_min = nccmp_min_size_t(vlens1[vlen_i].len, vlens2[vlen_i].len);
            for(opaq_i = 0; opaq_i < num_opaq_min; ++opaq_i) {
                opaq1 = vlens1[vlen_i].p;
                opaq2 = vlens2[vlen_i].p;
                bytes1 = opaq1 + (opaq_i * num_bytes);
                bytes2 = opaq2 + (opaq_i * num_bytes);
                for(byte_i = 0; byte_i < num_bytes; ++byte_i) {
                    if (bytes1[byte_i] != bytes2[byte_i]) {
                        if (!state->opts.warn[NCCMP_W_ALL]) {
                            last_status = EXIT_DIFFER;
                        }
                        diff_count_status = nccmp_inc_diff_count(state, varid1);
                        if (COUNT_OVER == diff_count_status) {
                            goto recover;
                        }
                        if (!state->opts.quiet) {
                            // Example: Z Y X
                            nccmp_get_index_str_size_t(v1->ndims, start, vlen_i, idx_str, state->opts.fortran);
                            if (state->opts.fortran) {
                                sprintf(vlen_idx_str, "%d,", opaq_i);
                                strcat(vlen_idx_str, idx_str);
                            } else {
                                sprintf(vlen_idx_str, ",%d", opaq_i);
                                strcat(idx_str, vlen_idx_str);
                            }
                            nccmp_bytes_to_hex_str(bytes1, num_bytes, str1);
                            nccmp_bytes_to_hex_str(bytes2, num_bytes, str2);
                            PRINT_DIFF(state->opts.debug, DIFF_MSG, v1->name, idx_str, str1, str2);
                        }
                        if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {
                            goto recover;
                        }
                        // Stop comparing rest of opaque blob if one byte differs.
                        break;
                    }
                }
            }
        }

        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;
    }

recover:
    if (vlens1) {
        status = nc_free_vlens(num_vlens, vlens1);
        HANDLE_NC_ERROR(status);
        XFREE(vlens1);
    }
    if (vlens2) {
        status = nc_free_vlens(num_vlens, vlens2);
        HANDLE_NC_ERROR(status);
        XFREE(vlens2);
    }

    return last_status;
}

int nccmp_cmp_var_user_type_vlen(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits)
{
    int last_status = EXIT_SUCCESS;
    nc_type base_type;
    nccmp_var_t *v1 = & state->vars1[varid1];
    nccmp_user_type_t *user_type;
    int status = nc_inq_vlen(ncid1, v1->type, 0 /*name*/, 0 /*datum_size*/, & base_type);
    HANDLE_NC_ERROR(status);

#define CMP_VLEN_TYPE(NAME)                                                    \
        last_status = nccmp_cmp_var_user_type_vlen_##NAME(state, ncid1, ncid2, \
                varid1, varid2, start, count, nitems, rec, limits);

    if (NC_FIRSTUSERTYPEID > base_type) {
        CMP_VLEN_TYPE(atomic);
    } else {
        user_type = nccmp_get_user_type_by_id(state->types1->items, state->types1->num_items, base_type);
        if (!user_type) {
            LOG_ERROR("No type found for base_type=%d\n", base_type);
            last_status = EXIT_FATAL;
        } else {

            switch(user_type->user_class) {
            case NC_ENUM:       CMP_VLEN_TYPE(enum);       break;
            case NC_OPAQUE:     CMP_VLEN_TYPE(opaque);     break;
            case NC_COMPOUND:
            case NC_VLEN:
                last_status = nccmp_cmp_var_user_type_nest(state, ncid1, ncid2,
                                varid1, varid2, start, count, nitems, rec, limits);
                break;
            default:
                LOG_ERROR("Unsupported vlen base_type=%d\n", base_type);
                last_status = EXIT_FATAL;
            }
        }
    }

#undef CMP_VLEN_TYPE
    return last_status;
}

int nccmp_cmp_var_user_type(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        int nitems, int rec, size_t* limits)
{
    int last_status = EXIT_SUCCESS;
    nccmp_user_type_t *type1;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_var_user_type\n");
    }

    type1 = nccmp_get_user_type_by_id(state->types1->items,
            state->types1->num_items, state->vars1[varid1].type);
    if (!type1) {
        LOG_ERROR("Failed to find user type with id = %d\n",
                    state->vars1[varid1].type);
        last_status = EXIT_FATAL;
        goto recover;
    }

#define CMP_USER_TYPE(CLASS)                                                   \
    last_status = nccmp_cmp_var_user_type_##CLASS(state, ncid1, ncid2,         \
                    varid1, varid2, start, count, nitems, rec, limits);

    switch(type1->user_class) {
    case NC_COMPOUND: CMP_USER_TYPE(compound);    break;
    case NC_ENUM:     CMP_USER_TYPE(enum);        break;
    case NC_OPAQUE:   CMP_USER_TYPE(opaque);      break;
    case NC_VLEN:     CMP_USER_TYPE(vlen);        break;
    default:
        LOG_ERROR("Unsupported user defined type class = %d in first file.\n",
                type1->user_class);
        last_status = EXIT_FATAL;
        break;
    }

#undef CMP_USER_TYPE

recover:
    return last_status;
}

int nccmp_cmp_var_iterator_init(nccmp_state_t* state, int varid,
        size_t* start, size_t* count,
        size_t* iterator_max, size_t* nitems, int rec)
{
    int i;
    int last_status = EXIT_SUCCESS;
    const nccmp_var_t* v1 = nccmp_find_var_by_id(state->vars1, state->nvars1, varid);
    
    if (!v1) {
        LOG_ERROR("Failed to find var with id=%d\n", varid);
        last_status = EXIT_FAILURE;
        goto recover;
    }
    
    for(i = 0; i < v1->ndims; ++i) {
        start[i] = 0;
        iterator_max[i] = v1->dimlens[i] - 1;
    }

    if (v1->has_rec_dim && (rec >= 0)) {
        start[0] = rec;
    }

    /* Read in slab for last dimension at-a-time only.
       We'll then loop over all the other slower/leftmost iterating dimensions. */
    for(i = 0; i < v1->ndims-1; ++i) {
        count[i] = 1;
    }

    /* We'll always read in entire last dimension
       except if only dimension is record. */
    if ((1 == v1->ndims) && v1->has_rec_dim) {
        *nitems = 1;
    } else {
        *nitems = v1->dimlens[v1->ndims-1];
    }

    /* Number of values to process per loop batch. */
    count[v1->ndims-1] = *nitems;
    
recover:
    return last_status;
}

int nccmp_cmp_var_atomic_type(nccmp_state_t* state,  int ncid1, int ncid2,
        int varid1, int varid2, size_t* start, size_t* count,
        size_t nitems, int rec, size_t* limits)
{
    int status, last_status = EXIT_SUCCESS;
    size_t diff_offset, cmplen, offset;
    double absdelta, pct;
    int i;
    char idx_str[NCCMP_MAX_STRINGS], str1[NCCMP_MAX_STRINGS], str2[NCCMP_MAX_STRINGS];

    const nccmp_var_t *v1, *v2;
    nccmp_nc_type_t item1, item2;
    nccmp_nc_type_ptr_t items1, items2;
    nc_type type1, type2;

    char print_hex = nccmp_is_precision_hex(& state->opts);
    char do_missing = 0;
    count_limit_type diff_count_status;
    int loop_done = 0;

    diff_count_status = nccmp_check_diff_count(state, varid1);
    if (diff_count_status) {
        goto recover;
    }

    strcpy(str1, "");
    strcpy(str2, "");
    v1 = nccmp_find_var_by_id(state->vars1, state->nvars1, varid1);
    v2 = nccmp_find_var_by_id(state->vars2, state->nvars2, varid2);
    type1 = nccmp_get_var_type(v1);
    type2 = nccmp_get_var_type(v2);
              
    if (state->opts.missing && v1->has_missing && v2->has_missing) {
        do_missing = 1;
    }

#define CMP_VAR(NAME1, TYPE1, NAME2, TYPE2, NANEQUAL) {                        \
    items1.p_##NAME1 = XMALLOC(TYPE1, nitems + 1);                             \
    items2.p_##NAME2 = XMALLOC(TYPE2, nitems + 1);                             \
                                                                               \
    while(! loop_done) {                                                       \
          status = nc_get_vara_##NAME1(ncid1, v1->varid, start, count, items1.p_##NAME1);\
          HANDLE_NC_ERROR(status);                                             \
          status = nc_get_vara_##NAME2(ncid2, v2->varid, start, count, items2.p_##NAME2);\
          HANDLE_NC_ERROR(status);                                             \
          /* Sentinels. */                                                     \
          items1.p_##NAME1[nitems] = 0;                                        \
          items2.p_##NAME2[nitems] = 1;                                        \
          offset = 0;                                                          \
          while (1) {                                                          \
            if (do_missing) {                                                  \
              if (NANEQUAL) {                                                  \
                diff_offset = nccmp_cmp_##NAME1##_##NAME2##_missing_nanequal(items1.p_##NAME1+offset, items2.p_##NAME2+offset, v1->missing.m_##NAME1, v2->missing.m_##NAME2);      \
              } else {                                                         \
                diff_offset = nccmp_cmp_##NAME1##_##NAME2##_missing(items1.p_##NAME1+offset, items2.p_##NAME2+offset, v1->missing.m_##NAME1, v2->missing.m_##NAME2);     \
              }                                                                \
            } else {                                                           \
              if (NANEQUAL) {                                                  \
                diff_offset = nccmp_cmp_##NAME1##_##NAME2##_nanequal(items1.p_##NAME1+offset, items2.p_##NAME2+offset);\
              } else {                                                         \
                diff_offset = nccmp_cmp_##NAME1##_##NAME2(items1.p_##NAME1+offset, items2.p_##NAME2+offset); \
              }                                                                \
            }                                                                  \
                                                                               \
            cmplen = nitems - offset;                                          \
            if (diff_offset >= cmplen)    {                                    \
                break;    /* nitems slice compare loop done. */                \
            }                                                                  \
            /* Diff found. */                                                  \
            offset += diff_offset + 1;                                         \
            if (!state->opts.warn[NCCMP_W_ALL]) {                              \
              last_status = EXIT_DIFFER;                                       \
            } else {                                                           \
              last_status = EXIT_SUCCESS;                                      \
            }                                                                  \
            item1.m_##NAME1 = items1.p_##NAME1[offset-1];                      \
            item2.m_##NAME2 = items2.p_##NAME2[offset-1];                      \
            if (state->opts.statistics) {                                      \
                nccmp_update_stats(state, varid1, (double)item1.m_##NAME1 - (double)item2.m_##NAME2); \
            }                                                                  \
            diff_count_status = nccmp_inc_diff_count(state, varid1);           \
            if (COUNT_OVER == diff_count_status) {                             \
                goto recover_cmp_##NAME1##_##NAME2;                            \
            }                                                                  \
            if (!state->opts.quiet) {                                          \
                nccmp_get_index_str_size_t(v1->ndims, start, offset-1, idx_str, state->opts.fortran);   \
                if (print_hex) {                                                                        \
                    nccmp_##NAME1##_to_hex(item1.m_##NAME1, str1);                                      \
                    nccmp_##NAME2##_to_hex(item2.m_##NAME2, str2);                                      \
                } else {                                                                                \
                    nccmp_##NAME1##_to_str(item1.m_##NAME1, str1, state->opts.precision);               \
                    nccmp_##NAME2##_to_str(item2.m_##NAME2, str2, state->opts.precision);               \
                }                                                                                       \
                PRINT_DIFF(state->opts.debug, DIFF_MSG, v1->name, idx_str, str1, str2);                 \
            }                                                                  \
            /* Print only up to count of desired diffs. */                     \
            if ( (COUNT_EQUAL == diff_count_status) || !state->opts.force) {   \
                goto recover_cmp_##NAME1##_##NAME2;                            \
            }                                                                  \
          } /* end while(1) */                                                 \
                                                                               \
      /* Increment all but first (if record) and last dimensions. */           \
      loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;     \
    }                                                                          \
    recover_cmp_##NAME1##_##NAME2:                                             \
        free(items1.p_##NAME1);                                                \
        free(items2.p_##NAME2);                                                \
} /* end CMP_VAR */

#define CMP_VAR_TOL(NAME1, TYPE1, NAME2, TYPE2, NANEQUAL) {                    \
    items1.p_##NAME1 = (TYPE1*)malloc(sizeof(TYPE1) * (nitems + 1));           \
    items2.p_##NAME2 = (TYPE2*)malloc(sizeof(TYPE2) * (nitems + 1));           \
                                                                               \
    while(! loop_done) {                                                       \
            status = nc_get_vara_##NAME1(ncid1, v1->varid, start, count, items1.p_##NAME1);        \
            HANDLE_NC_ERROR(status);                                           \
            status = nc_get_vara_##NAME2(ncid2, v2->varid, start, count, items2.p_##NAME2);        \
            HANDLE_NC_ERROR(status);                                           \
            for(i = 0; i < nitems; ++i) {                                      \
                if (do_missing) {                                              \
                    if ((v1->missing.m_##NAME1 == items1.p_##NAME1[i]) &&      \
                        (v2->missing.m_##NAME2 == items2.p_##NAME2[i])) continue;        \
                }                                                              \
                if (NANEQUAL) {                                                \
                    if ( isnan(items1.p_##NAME1[i]) && isnan(items2.p_##NAME2[i]) ) continue;      \
                }                                                              \
                absdelta = fabs((double)(items1.p_##NAME1[i] - items2.p_##NAME2[i]));    \
                if (state->opts.abstolerance ?                                 \
                        (absdelta > state->opts.tolerance) :                   \
                        (double)absdelta*100./(fabs((double)items1.p_##NAME1[i]) > fabs((double)items2.p_##NAME2[i]) ? \
                                                   fabs((double)items1.p_##NAME1[i]) :   \
                                                   fabs((double)items2.p_##NAME2[i])) > state->opts.tolerance)         \
                {                                                                   \
                    if (!state->opts.warn[NCCMP_W_ALL]) {                           \
                        last_status = EXIT_DIFFER;                                  \
                    } else {                                                        \
                        last_status = EXIT_SUCCESS;                                 \
                    }                                                               \
                    item1.m_##NAME1 = items1.p_##NAME1[i];                          \
                    item2.m_##NAME2 = items2.p_##NAME2[i];                          \
                    if (state->opts.statistics) {                                   \
                        nccmp_update_stats(state, varid1, (double)item1.m_##NAME1 - (double)item2.m_##NAME2); \
                    }                                                               \
                    diff_count_status = nccmp_inc_diff_count(state, varid1);        \
                    if (COUNT_OVER == diff_count_status) {                          \
                        goto recover_tol_##NAME1##_##NAME2;                         \
                    }                                                               \
                    if (!state->opts.quiet) {                                       \
                        if ((v1->ndims == 1)  && (v1->has_rec_dim)) {               \
                            nccmp_get_index_str_size_t(v1->ndims, start, rec, idx_str, state->opts.fortran); \
                        } else {                                                    \
                            nccmp_get_index_str_size_t(v1->ndims, start, i, idx_str, state->opts.fortran);   \
                        }                                                           \
                        if (print_hex) {                                            \
                            nccmp_##NAME1##_to_hex(item1.m_##NAME1, str1);          \
                            nccmp_##NAME2##_to_hex(item2.m_##NAME2, str2);          \
                        } else {                                                    \
                            nccmp_##NAME1##_to_str(item1.m_##NAME1, str1, state->opts.precision);       \
                            nccmp_##NAME2##_to_str(item2.m_##NAME2, str2, state->opts.precision);       \
                        }                                                                               \
                        pct = (double)absdelta*100./(fabs((double)items1.p_##NAME1[i]) > fabs((double)items2.p_##NAME2[i]) ? fabs((double)items1.p_##NAME1[i]) : fabs((double)items2.p_##NAME2[i]));       \
                        PRINT_DIFF(state->opts.debug, DIFF_MSG_PCT, v1->name, idx_str, str1, str2, pct);\
                    }                                                                                   \
                    if ( (COUNT_EQUAL ==  diff_count_status) || !state->opts.force ) {                  \
                        goto recover_tol_##NAME1##_##NAME2;                         \
                    }                                                               \
                } /* diff */                                                        \
            } /* for nitems */                                                      \
        loop_done = nccmp_odometer(start, limits, (int) (rec >= 0), v1->ndims-2) == 0;   \
    } /* while not done */                                                          \
                                                                                    \
    recover_tol_##NAME1##_##NAME2:                                                  \
      free(items1.p_##NAME1);                                                       \
      free(items2.p_##NAME2);                                                       \
} /* end CMP_VAR_TOL */

#define CMP(...) if (state->opts.tolerance) { CMP_VAR_TOL(__VA_ARGS__); } else { CMP_VAR(__VA_ARGS__); }

#define CMP_TYPE1(NAME1, TYPE1, NANEQUAL)                                      \
    switch(type2) {                                                            \
        case NC_BYTE:   CMP(NAME1, TYPE1, schar,     signed char,       NANEQUAL);      break;     \
        case NC_CHAR:   CMP(NAME1, TYPE1, text,      char,              NANEQUAL);      break;     \
        case NC_DOUBLE: CMP(NAME1, TYPE1, double,    double,            state->opts.nanequal); break;        \
        case NC_FLOAT:  CMP(NAME1, TYPE1, float,     float,             state->opts.nanequal); break;        \
        case NC_INT:    CMP(NAME1, TYPE1, int,       int,               NANEQUAL);      break;     \
        case NC_INT64:  CMP(NAME1, TYPE1, longlong,  long long,         NANEQUAL);      break;     \
        case NC_SHORT:  CMP(NAME1, TYPE1, short,     short,             NANEQUAL);      break;     \
        case NC_UBYTE:  CMP(NAME1, TYPE1, uchar,     unsigned char,     NANEQUAL);      break;     \
        case NC_UINT:   CMP(NAME1, TYPE1, uint,      unsigned int,      NANEQUAL);      break;     \
        case NC_UINT64: CMP(NAME1, TYPE1, ulonglong, unsigned long long,NANEQUAL);      break;     \
        case NC_USHORT: CMP(NAME1, TYPE1, ushort,    unsigned short,    NANEQUAL);      break;     \
        case NC_STRING:                                                        \
        default:                                                               \
             LOG_ERROR("Unsupported second var data type = %d\n", type2);      \
             last_status = EXIT_FAILED;                                        \
             break;                                                            \
    }


    switch(type1) {
        case NC_BYTE:   CMP_TYPE1(schar,    signed char,        0)              break;
        case NC_CHAR:   CMP_TYPE1(text,     char,               0);             break;
        case NC_DOUBLE: CMP_TYPE1(double,   double,             state->opts.nanequal); break;
        case NC_FLOAT:  CMP_TYPE1(float,    float,              state->opts.nanequal); break;
        case NC_INT:    CMP_TYPE1(int,      int,                0);             break;
        case NC_INT64:  CMP_TYPE1(longlong, long long,          0);             break;
        case NC_SHORT:  CMP_TYPE1(short,    short,              0);             break;
        case NC_UBYTE:  CMP_TYPE1(uchar,    unsigned char,      0);             break;
        case NC_UINT:   CMP_TYPE1(uint,     unsigned int,       0);             break;
        case NC_UINT64: CMP_TYPE1(ulonglong,unsigned long long, 0);             break;
        case NC_USHORT: CMP_TYPE1(ushort,   unsigned short,     0);             break;
        case NC_STRING:
            if (NC_STRING == type2) {
                last_status = nccmp_cmp_var_string(state, ncid1, ncid2, varid1, varid2,
                                start, count, nitems, rec, limits);
                break;
            }
        default:
            LOG_ERROR("Unsupported first var data type = %d\n", type1);
            last_status = EXIT_FAILED;
            break;
    }

#undef CMP
#undef CMP_TYPE1
#undef CMP_VAR
#undef CMP_VAR_TOL

recover:
    return last_status;
}

int nccmp_cmp_var_start(nccmp_state_t* state, int ncid1, int ncid2,
        int varid1, int varid2,
        size_t* start, size_t* count, size_t* limits,
        int rec, size_t nitems)
{
    if (NC_FIRSTUSERTYPEID <= state->vars1[varid1].type) {
        return nccmp_cmp_var_user_type(state,
                    ncid1, ncid2,
                    varid1, varid2,
                    start, count,
                    nitems, rec,
                    limits);
    } else {
        return nccmp_cmp_var_atomic_type(state,
                    ncid1, ncid2,
                    varid1, varid2,
                    start, count,
                    nitems, rec,
                    limits);
    }
}

/* Do comparison of variable with 'name'.
   rec: 0-base record dimension index to process; -1 if not applicable.
   Returns comparison result success or failure.
*/
int nccmp_cmp_var(nccmp_state_t* state, const char* name, int ncid1,
        int ncid2, int rec, int thread_id)
{
    const nccmp_var_t *v1,*v2;
    size_t *start, *count;
    size_t nitems;
    size_t *limits;
    int last_status = EXIT_SUCCESS;
    
    if (state->opts.verbose) {
        if (-1 == rec) {
            LOG_INFO("Thread %d comparing non-record data for variable %s.\n", thread_id, name);
        } else {
            LOG_INFO("Thread %d comparing data for variable %s at record %d.\n", thread_id, name, (int)rec);
        }
    }

    start = XCALLOC(size_t, NC_MAX_DIMS);
    count = XCALLOC(size_t, NC_MAX_DIMS);
    limits = XCALLOC(size_t, NC_MAX_DIMS);
    
    v1 = nccmp_find_var_by_name(state->vars1, state->nvars1, name);
    v2 = nccmp_find_var_by_name(state->vars2, state->nvars2, name);
    
    if (!v1 || !v2) {
        last_status = EXIT_FATAL;
        goto recover;
    }
    
    if (v1->has_rec_dim && (rec < 0)) {
        if (state->opts.debug) {
            LOG_DEBUG("Empty record will not be compared.\n");
        }
        goto recover;
    }

    if (v1->len != v2->len) {
        if (!state->opts.quiet) {
            PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : SIZE : %d <> %d\n", name, (int)v1->len, (int)v2->len);
        }
        if (!state->opts.warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        goto recover;
    }

    last_status = nccmp_cmp_var_iterator_init(state, v1->varid, 
        start, count, limits, & nitems, rec);
    if (last_status) {
        goto recover;
    }
    
    last_status = nccmp_cmp_var_start(state, ncid1, ncid2, 
        v1->varid, v2->varid, start, count, limits, rec, nitems);
        
recover:
    XFREE(start);
    XFREE(count);
    XFREE(limits);
    
    return last_status;
}

int nccmp_cmp_vars(nccmp_state_t* state, nccmp_strlist_t* var_names, int ncid1, int ncid2, int thread_id)
{
    int i, idx1, idx2, status, last_status = EXIT_SUCCESS;
    int rec_iter, rec_first, rec_last;

    if (state->opts.debug) {
        LOG_DEBUG("cmp_vars\n");
    }
    for(i = 0; i < var_names->size; ++i) {
        idx1 = find_var(var_names->items[i], state->vars1, state->nvars1);
        idx2 = find_var(var_names->items[i], state->vars2, state->nvars2);

        /* Compare recs only if lengths are equal and not zero. */
        if ( state->vars1[idx1].has_rec_dim &&
             state->vars2[idx2].has_rec_dim &&
            (state->nrecs1 == state->nrecs2) &&
            (state->nrecs1 + state->nrecs2) ) {
            rec_first = 0;
            rec_last = state->nrecs1 - 1;
        } else {
            rec_first = -1;
            rec_last = -1;
        }

        for(rec_iter = rec_first + thread_id;
            rec_iter <= rec_last;
            rec_iter += state->opts.threads)
        {
            status = nccmp_cmp_var(state, var_names->items[i], ncid1, ncid2,
                        rec_iter, thread_id);
            last_status = status ? status : last_status;
            if (last_status && !state->opts.force) {
                goto recover;
            }
        }
    }

recover:
    return last_status;
}

int nccmp_data(nccmp_state_t* state, nccmp_strlist_t* var_names,
               nccmp_group_t* group1, nccmp_group_t* group2)
{
    int status, last_status = EXIT_SUCCESS;
    nccmp_strlist_t* validated_var_names;

    if (state->opts.verbose) {
        LOG_INFO("Comparing data.\n");
    }

    status = validate_var_names(& state->opts, var_names, & validated_var_names,
            group1->id, group2->id, state->vars1, state->vars2,
            state->nvars1, state->nvars2);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    nccmp_init_var_diff_counts(state);
    nccmp_init_group_stats(state->stats, group1->id, state->nvars1);
    state->current_var_stats = nccmp_find_group_var_stats_array(state->stats, group1->id);

    nccmp_destroy_user_types(state->types1);
    state->types1 = nccmp_load_user_types(group1->id, state->opts.debug);

    nccmp_destroy_user_types(state->types2);
    state->types2 = nccmp_load_user_types(group2->id, state->opts.debug);

    status = nccmp_data_start_threads(state, validated_var_names,
                group1->fullname, group2->fullname);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

recover:
    nccmp_free_strlist(& validated_var_names);

    if (state->opts.verbose) {
        LOG_INFO("Finished comparing data.\n");
    }

    return last_status;
}

#if HAVE_PTHREAD
void* nccmp_data_pthread_task(void* void_args)
{
    nccmp_thread_args_t* args = (nccmp_thread_args_t*) void_args;
    args->result = nccmp_cmp_vars(args->state, args->var_names, args->ncid1, args->ncid2, args->id);
    return 0;
}

int nccmp_data_start_pthreads(nccmp_state_t* state, nccmp_strlist_t* var_names,
                             const char* group_name1, const char* group_name2)
{
    int thread, status, status_thread, last_status = EXIT_SUCCESS;
    pthread_attr_t attr_joinable;
    pthread_t* threads = 0;
    nccmp_thread_args_t* args;
    nccmp_group_t *group1, *group2;

    pthread_attr_init(& attr_joinable);
    pthread_attr_setdetachstate(& attr_joinable, PTHREAD_CREATE_JOINABLE);
    threads = XMALLOC(pthread_t, state->opts.threads);
    args = XMALLOC(nccmp_thread_args_t, state->opts.threads);

    for(thread = 0; thread < state->opts.threads; ++thread) {
        if (state->opts.debug) {
            LOG_DEBUG("Creating thread %d\n", thread);
        }

        group1 = nccmp_find_group_by_name(& state->thread_groups1[thread], group_name1);
        if (!group1) {
            LOG_ERROR("Failed to find group %s\n", group_name1);
            last_status = EXIT_FATAL;
            goto recover;
        }
        group2 = nccmp_find_group_by_name(& state->thread_groups2[thread], group_name2);
        if (!group2) {
            LOG_ERROR("Failed to find group %s\n", group_name2);
            last_status = EXIT_FATAL;
            goto recover;
        }

        args[thread].var_names = var_names;
        args[thread].state = state;
        args[thread].ncid1 = group1->id;
        args[thread].ncid2 = group2->id;
        args[thread].id = thread;

        status = pthread_create(& threads[thread], & attr_joinable,
                                nccmp_data_pthread_task, (void*) & (args[thread]) );
        if (status) {
            LOG_ERROR("pthread_create() failed with status = %d\n", status);
            exit(-1);
        }
    }

    pthread_attr_destroy(& attr_joinable);
    for(thread = 0; thread < state->opts.threads; ++thread) {
        status_thread = pthread_join(threads[thread], NULL);
        if (status_thread) {
            LOG_ERROR("pthread_join() failed with status_thread = %d\n", status_thread);
            exit(-1);
        }
        if (state->opts.debug) {
            LOG_DEBUG("Thread %d returned with result = %d\n", thread, args[thread].result);
        }
        last_status = args[thread].result ? args[thread].result : last_status;
    }

recover:
    XFREE(threads);
    XFREE(args);

    return last_status;
}
#endif /* HAVE_PTHREAD */

int nccmp_data_start_threads(nccmp_state_t* state, nccmp_strlist_t* var_names,
                             const char* group_name1, const char* group_name2)
{
    nccmp_group_t *group1, *group2;

    if (state->opts.threads > 1) {
        #if HAVE_PTHREAD
        return nccmp_data_start_pthreads(state, var_names, group_name1, group_name2);
        #endif
    } else {
        group1 = nccmp_find_group_by_name(& state->thread_groups1[0], group_name1);
        group2 = nccmp_find_group_by_name(& state->thread_groups2[0], group_name2);

        if (group1 && group2) {
            return nccmp_cmp_vars(state, var_names, group1->id, group2->id, 0);
        }
    }
    return EXIT_FATAL;
}
