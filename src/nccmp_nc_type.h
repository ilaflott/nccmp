#ifndef NCCMP_NC_TYPE_H
#define NCCMP_NC_TYPE_H

#include <netcdf.h>

typedef struct nccmp_nc_type_t {
    nc_type            type;
    char               m_text;
    double             m_double;
    float              m_float;
    int                m_int;
    long long          m_longlong;
    short              m_short;
    signed char        m_schar;
    unsigned int       m_uint;
    unsigned char      m_uchar;
    unsigned long long m_ulonglong;
    unsigned short     m_ushort;
} nccmp_nc_type_t;

typedef struct nccmp_nc_type_ptr_t {
    char               *p_text;
    char              **p_string;
    double             *p_double;
    float              *p_float;
    int                *p_int;
    long long          *p_longlong;
    short              *p_short;
    signed char        *p_schar;
    unsigned int       *p_uint;
    unsigned char      *p_uchar;
    unsigned long long *p_ulonglong;
    unsigned short     *p_ushort;
    void               *p_void;
} nccmp_nc_type_ptr_t;

void nccmp_nc_type_name_to_str(nc_type type, char* str, int ncid, int debug);

#endif
