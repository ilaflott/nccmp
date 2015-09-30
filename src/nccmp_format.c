#include <log.h>
#include <nccmp_error.h>
#include <nccmp_format.h>
#include <nccmp_utils.h>
#include <netcdf.h>
#include <nccmp_common.h>

#define GET_INDEX_STR(TYPE)                                                    \
void nccmp_get_index_str_##TYPE(int nitems, TYPE* items, int last, char* out, char fortran)    \
{                                                                              \
    int i;                                                                     \
    char tmp[NCCMP_MAX_TMP_STR_SIZE];                                          \
    strcpy(out, "");                                                           \
    if (fortran) {                                                             \
        sprintf(tmp, "%d", last + 1);                                          \
        strcat(out, tmp);                                                      \
                                                                               \
        for(i = nitems-2; i >= 0; --i) {                                       \
            sprintf(tmp, ",%d", (int)items[i]+1);                              \
            strcat(out, tmp);                                                  \
        }                                                                      \
    } else {                                                                   \
        for(i=0; i < nitems-1; ++i) {                                          \
            sprintf(tmp, "%d,", (int)items[i]);                                \
            strcat(out, tmp);                                                  \
        }                                                                      \
        sprintf(tmp, "%d", last);                                              \
        strcat(out, tmp);                                                      \
    }                                                                          \
}
GET_INDEX_STR(int)
GET_INDEX_STR(size_t)
#undef GET_INDEX_STR

#define GET_PRODUCT_INDEX_INVERSE_ARRAY(TYPE)                                  \
void nccmp_get_product_index_inverse_array_##TYPE(TYPE nitems, TYPE* limits, TYPE index, TYPE *result) \
{ \
    TYPE prod; \
    int i; \
    TYPE div, rem = index; \
 \
    for(i = 0; i < nitems; ++i) { \
        prod = nccmp_product_##TYPE(limits, i+1, nitems-1); \
        div = rem / prod; \
        rem = rem % prod; \
        result[i] = div;   \
    } \
}
GET_PRODUCT_INDEX_INVERSE_ARRAY(int)
GET_PRODUCT_INDEX_INVERSE_ARRAY(size_t)
#undef GET_PRODUCT_INDEX_INVERSE_ARRAY

#define GET_PRODUCT_INDEX_STR(NAME, TYPE) \
void nccmp_get_product_index_str_##NAME(TYPE nitems, TYPE* limits, TYPE index, char* out, char fortran) \
{ \
    TYPE items[NCCMP_MAX_TMP_STR_SIZE]; \
    \
    nccmp_get_product_index_inverse_array_##TYPE(nitems, limits, index, items); \
    nccmp_get_index_str_##TYPE(nitems, items, items[nitems-1], out, fortran); \
}
GET_PRODUCT_INDEX_STR(int, int)
GET_PRODUCT_INDEX_STR(size_t, size_t)
#undef GET_PRODUCT_INDEX_STR

#define TYPE_ARRAY_TO_STR(NAME, TYPE, FORMAT) \
void nccmp_##NAME##_array_to_str(TYPE* items, int nitems, char* result, const char* delim) \
{ \
    int i = 0; \
    char tmp[256]; \
    strcpy(result, ""); \
    strcpy(tmp, ""); \
    \
    for(; i < nitems; ++i) { \
        sprintf(tmp, FORMAT, items[i]); \
        strcat(result, tmp); \
        if ( (i+1) < nitems ) { \
            strcat(result, delim); \
        } \
    } \
}
TYPE_ARRAY_TO_STR(size_t, size_t, "%zu")
#undef TYPE_ARRAY_TO_STR

#define TYPE_TO_STR_GEN(NAME, TYPE)                                            \
void nccmp_##NAME##_to_str(TYPE v, char* out, const char* user_format) {               \
    if (user_format) {                                                         \
        sprintf(out, user_format, v);                                          \
    } else {                                                                   \
        sprintf(out, NCCMP_FORMAT_##NAME, v);                                  \
    }                                                                          \
}

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

#define TYPE_TO_HEX(NAME, TYPE)                                                \
void nccmp_##NAME##_to_hex(TYPE v, char* out) {                                        \
    unsigned char *p = (unsigned char*)&v;                                     \
    int i;                                                                     \
    char tmp[3];                                                               \
    strcpy(out, "0x");                                                         \
    for(i = 0; i < sizeof(TYPE); ++i) {                                        \
        sprintf(tmp, "%02X", p[i]);                                            \
        strcat(out, tmp);                                                      \
    }                                                                          \
}
TYPE_TO_HEX(double,  double)
TYPE_TO_HEX(float,   float)
TYPE_TO_HEX(int,     int)
TYPE_TO_HEX(longlong,long long)
TYPE_TO_HEX(schar,   signed char)
TYPE_TO_HEX(short,   short)
TYPE_TO_HEX(text,    char)
TYPE_TO_HEX(uchar,   unsigned char)
TYPE_TO_HEX(uint,    unsigned int)
TYPE_TO_HEX(ulonglong, unsigned long long)
TYPE_TO_HEX(ushort,  unsigned short)
#undef TYPE_TO_HEX

void nccmp_bytes_to_hex_str(unsigned char* bytes, size_t n, char* out)
{
    int i;
    char tmp[3];
    strcpy(out, "0x");
    for(i = 0; i < n; ++i) {
        sprintf(tmp, "%02X", bytes[i]);
        strcat(out, tmp);
    }
}

char* nccmp_realloc_bytes_hex_str(char *ptr, int nbytes) {
    // Each byte will use 2 chars, plus single "0x" prefix, and null char.
    // For nccmp_bytes_to_hex_str().
    char *result = realloc(ptr, nbytes * 2 + 2 + 1);
    result[0] = 0;
    return result;
}

void nccmp_checksum_to_str(int value, char* result) {
    switch(value) {
    case NC_FLETCHER32: strcpy(result, "NC_FLETCHER32");    break;
    case NC_NOCHECKSUM: strcpy(result, "NC_NOCHECKSUM");    break;
    default:            strcpy(result, "UNKNOWN");          break;
    }
}

void nccmp_chunk_storage_to_str(int value, char* result) {
    switch(value) {
    case NC_CONTIGUOUS :    strcpy(result, "NC_CONTIGUOUS");    break;
    case NC_CHUNKED:        strcpy(result, "NC_CHUNKED");       break;
    default:                strcpy(result, "UNKNOWN");          break;
    }
}

void nccmp_endian_to_str(int value, char* result) {
    switch(value) {
    case NC_ENDIAN_LITTLE:  strcpy(result, "NC_ENDIAN_LITTLE");  break;
    case NC_ENDIAN_NATIVE:  strcpy(result, "NC_ENDIAN_NATIVE");  break;
    case NC_ENDIAN_BIG:     strcpy(result, "NC_ENDIAN_BIG");     break;
    default:                strcpy(result, "UNKNOWN");           break;
    }
}

int nccmp_is_precision_hex(nccmp_opt_t* opts)
{
    return opts->precision &&
           (strchr(opts->precision, 'X') || strchr(opts->precision, 'x'));
}

void nccmp_pretty_print_att(int ncid, const char* varname, 
    int varid, const char* name, char* result, int maxstrlen)
{
    int status, i;
    nc_type type;
    size_t len, substrlen;
    char               tmpstr[NC_MAX_NAME];
    char               *pt = 0;
    char               **pst = 0;
    double             *pd = 0;
    float              *pf = 0;
    int                *pi = 0;
    long long          *pll = 0;
    short              *ps = 0;
    signed char        *psc = 0;
    unsigned char      *puc = 0;
    unsigned int       *pui = 0;
    unsigned long long *pull = 0;
    unsigned short     *pus = 0;
	
    status = nc_inq_att(ncid, varid, name, &type, &len);
    if (status != NC_NOERR) {
        if (varid == NC_GLOBAL) {
            LOG_ERROR("Failed to query global attributes %s\n", name);
        } else {
            LOG_ERROR("Failed to query attribute %s for variable %s\n", name, varname);
        }
        return;
    }
    strcpy(result, "");
    if (len < 1) {
        return;
    }
    if (len > maxstrlen) {
        LOG_WARN("Attribute contents larger than string buffer for printing. Will truncate %s:%s.\n", varname, name);
        len = maxstrlen;
    }
    #define STRINGIFY(PTR, TYPE, FUN, FMT, DELIM) {                            \
        PTR = XMALLOC(TYPE, len);                                              \
        status = nc_get_att_##FUN(ncid, varid, name, PTR);                     \
        if (status != NC_NOERR)    {                                           \
            break;                                                             \
        }                                                                      \
        for(i=0; i < len; ++i) {                                               \
            sprintf(tmpstr, FMT, PTR[i]);                                      \
            if ( (strlen(result) + strlen(tmpstr)) >= maxstrlen ) {            \
                strcat(result, "...");                                         \
                break;                                                         \
            }                                                                  \
            strcat(result, tmpstr);                                            \
            if ( (i+1) < len ) {                                               \
                strcat(result, DELIM);                                         \
            }                                                                  \
        }                                                                      \
    }

    switch(type)
    {
    case NC_BYTE:   STRINGIFY(psc,  signed char,        schar,     "0x%02X", ","); break;
    case NC_CHAR:   STRINGIFY(pt,   char,               text,      "%c",     "");  break;
    case NC_DOUBLE: STRINGIFY(pd,   double,             double,    "%.17g",  ","); break;
    case NC_FLOAT:  STRINGIFY(pf,   float,              float,     "%.9g",   ","); break;
    case NC_INT:    STRINGIFY(pi,   int,                int,       "%d",     ","); break;
    case NC_INT64:  STRINGIFY(pll,  long long,          longlong,  "%lld",   ","); break;
    case NC_SHORT:  STRINGIFY(ps,   short,              short,     "%d",     ","); break;
    case NC_UBYTE:  STRINGIFY(puc,  unsigned char,      uchar,     "0x%02X", ","); break;
    case NC_UINT:   STRINGIFY(pui,  unsigned int,       uint,      "%d",     ","); break;
    case NC_UINT64: STRINGIFY(pull, unsigned long long, ulonglong, "%llu",   ","); break;
    case NC_USHORT: STRINGIFY(pus,  unsigned short,     ushort,    "%d",     ","); break;
    case NC_STRING:
        pst = XCALLOC(char*, len);
        /* Library will allocate each string's memory. We must free them though. */
        status = nc_get_att_string(ncid, varid, name, pst);
        if (status != NC_NOERR)    {
            HANDLE_NC_ERROR(status);
        }
        for(i=0; i < len; ++i) {
            if (pst[i]) {
                substrlen = strlen(pst[i]);
            } else {
                substrlen = 0;
            }
            if ( (strlen(result) + substrlen + 4) >= maxstrlen) {
                strcat(result, "...");
                break;
            }
            strcat(result, "\"");
            if (pst[i]) {
                strcat(result, pst[i]);
            }
            strcat(result, "\"");
            if ( (i+1) < len ) {
                strcat(result, ",");
            }
        }
        break;
    default:
        LOG_WARN("Pretty printing attribute type %d not supported.\n", type);
        break;
    }

    #undef STRINGIFY
    XFREE(pd);
    XFREE(pf);
    XFREE(pi);
    XFREE(pll);
    XFREE(ps);
    XFREE(psc);
    XFREE(pt);
    XFREE(puc);
    XFREE(pui);
    XFREE(pull);
    XFREE(pus);
    if (pst) {
        /* Release memory allocated by library. */
		status = nc_free_string(len, pst);
		HANDLE_NC_ERROR(status);
    }
    XFREE(pst);
}
