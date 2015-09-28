#ifndef NCCMP_FORMAT_H
#define NCCMP_FORMAT_H

#include <nccmp_opt.h>

#define NCCMP_MAX_TMP_STR_SIZE 256

#define NCCMP_FORMAT_double    "%g"
#define NCCMP_FORMAT_float     "%g"
#define NCCMP_FORMAT_int       "%d"
#define NCCMP_FORMAT_longlong  "%lld"
#define NCCMP_FORMAT_schar     "0x%02X"
#define NCCMP_FORMAT_short     "%d"
#define NCCMP_FORMAT_size_t    "%zu"
#define NCCMP_FORMAT_text      "%c"
#define NCCMP_FORMAT_uchar     "0x%02X"
#define NCCMP_FORMAT_uint      "%d"
#define NCCMP_FORMAT_ulonglong "%llu"
#define NCCMP_FORMAT_ushort    "%d"

/* Creates a string representation of a value into pre-allocated char array. */
#define TYPE_TO_STR_GEN(NAME, TYPE) void nccmp_##NAME##_to_str(TYPE v, char* out, const char* user_format);
TYPE_TO_STR_GEN(double,     double)
TYPE_TO_STR_GEN(float,      float)
TYPE_TO_STR_GEN(int,        int)
TYPE_TO_STR_GEN(longlong,   long long)
TYPE_TO_STR_GEN(schar,      signed char)
TYPE_TO_STR_GEN(short,      short)
TYPE_TO_STR_GEN(text,       char)
TYPE_TO_STR_GEN(uchar,      unsigned char)
TYPE_TO_STR_GEN(uint,       unsigned int)
TYPE_TO_STR_GEN(ulonglong,  unsigned long long)
TYPE_TO_STR_GEN(ushort,     unsigned short)
TYPE_TO_STR_GEN(size_t,     size_t)
#undef TYPE_TO_STR_GEN

/* Creates a hex representation of a value into pre-allocated char array. */
#define TYPE_TO_HEX(NAME, TYPE) void nccmp_##NAME##_to_hex(TYPE v, char* out);
TYPE_TO_HEX(double,     double)
TYPE_TO_HEX(float,      float)
TYPE_TO_HEX(int,        int)
TYPE_TO_HEX(longlong,   long long)
TYPE_TO_HEX(schar,      signed char)
TYPE_TO_HEX(short,      short)
TYPE_TO_HEX(text,       char)
TYPE_TO_HEX(uchar,      unsigned char)
TYPE_TO_HEX(uint,       unsigned int)
TYPE_TO_HEX(ulonglong,  unsigned long long)
TYPE_TO_HEX(ushort,     unsigned short)
#undef TYPE_TO_HEX

void nccmp_bytes_to_hex_str(unsigned char* bytes, size_t n, char* out);

char* nccmp_realloc_bytes_hex_str(char *str, int nbytes);

void nccmp_checksum_to_str(int value, char* result);

void nccmp_chunk_storage_to_str(int value, char* result);

void nccmp_endian_to_str(int value, char* result);

/*
    Returns formatted string of dimension indices.
    Intended to print out locations of value differences.

    @param fortran: If should use fortran index convention.
    @param last: Extra digit not in items array to print.
    @param[out] out: Should be pre-allocated large enough for output.
*/
#define GET_INDEX_STR(TYPE)                                                    \
    void nccmp_get_index_str_##TYPE(int nitems, TYPE* items, int last, char* out, char fortran);
GET_INDEX_STR(int)
GET_INDEX_STR(size_t)
#undef GET_INDEX_STR

#define GET_PRODUCT_INDEX_INVERSE_ARRAY(TYPE)                                  \
    void nccmp_get_product_index_inverse_array_##TYPE(TYPE nitems, TYPE* limits, TYPE index, TYPE *result);
GET_PRODUCT_INDEX_INVERSE_ARRAY(int)
GET_PRODUCT_INDEX_INVERSE_ARRAY(size_t)
#undef GET_PRODUCT_INDEX_INVERSE_ARRAY

/*
    Inversely compute multidim position of a flattened index.
    Example,
        limits = {10,20,30}
        index = 5999
        fortran = 0
        out = "9 19 29"

*/
#define GET_PRODUCT_INDEX_STR(NAME, TYPE)                                      \
    void nccmp_get_product_index_str_##NAME(TYPE nitems, TYPE* limits, TYPE index, char* out, char fortran);
GET_PRODUCT_INDEX_STR(int, int)
GET_PRODUCT_INDEX_STR(size_t, size_t)
#undef GET_PRODUCT_INDEX_STR

int nccmp_is_precision_hex(nccmp_opt_t* opts);

/* Pretty prints attribute values into a string.
   Assumes 'str' is pre-allocated large enough to hold output string,
   defined by 'maxstrlen'.
*/
void nccmp_pretty_print_att(int ncid, const char* varname, int varid, 
    const char* name, char* result, int maxstrlen);

#define TYPE_ARRAY_TO_STR(NAME, TYPE, FORMAT)                                  \
    void nccmp_##NAME##_array_to_str(TYPE* items, int nitems, char* result, const char* delim);
TYPE_ARRAY_TO_STR(size_t, size_t, "%zu")
#undef TYPE_ARRAY_TO_STR

#endif
