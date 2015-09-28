#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>
#include <string.h>

void
check_err(const int stat, const int line, const char *file) {
    if (stat != NC_NOERR) {
        (void)fprintf(stderr,"line %d of %s: %s\n", line, file, nc_strerror(stat));
        fflush(stderr);
        exit(1);
    }
}

#define M 2
#define N 3
#define MN 6
#define ND 2

typedef struct COMP1 {
    signed char sc[MN];
    unsigned char uc[MN];
    char c[MN];
    short s[MN];
    unsigned short us[MN];
    int i[MN];
    unsigned int ui[MN];
    long long ll[MN];
    unsigned long long ull[MN];
    float f[MN];
    double d[MN];
    char** t[MN];
} comp1;

int main(int argc, char** argv) {
    int  stat, ncid;  
    int j, k, typeid1, varid1;
    int dimids[4];
    signed char sc[] = {0,1,2,3,4,5};
    char c[] = {'a','b','c','d','e','f'};
    unsigned char uc[] = {0,1,2,3,4,5};
    short s[] = {0,1,2,3,4,5};
    int i[] = {0,1,2,3,4,5};
    unsigned int ui[] = {0,1,2,3,4,5};
    long long ll[] = {0,1,2,3,4,5};
    unsigned long long ull[] = {0,1,2,3,4,5};
    unsigned short us[] = {0,1,2,3,4,5};
    float f[] = {0,1,2,3,4,5};
    double d[] = {0,1,2,3,4,5};
    char *t[] = {"a", "bc", "def", "ghi", "jkl", "mno"};
    int dim_sizes[] = {M, N};
    size_t start[] = {0,0};
    size_t count[] = {1,1};
    comp1 data1[2];
    char fuzz = 0;
    char *filename;
    
    if (argc < 3 ) {
        fprintf(stderr, "usage: %s <fuzz> <out.nc>\n", argv[0]);
        exit(2);
    }
    
    fuzz = atoi(argv[1]);
    filename = argv[2];
  
    if (fuzz) {
        sc[5] = uc[5] = s[5] = us[5] = i[5] = ui[5] = ll[5] = ull[5] = f[5] = d[5] = 6;
        c[5] = 'g';
        t[5] = "pqr";
    }
    
    memcpy(data1[0].sc, sc, sizeof(signed char) * MN);
    memcpy(data1[1].sc, sc, sizeof(signed char) * MN);
    memcpy(data1[0].uc, uc, sizeof(signed char) * MN);
    memcpy(data1[1].uc, uc, sizeof(signed char) * MN);
    memcpy(data1[0].c, c, sizeof(char) * MN);
    memcpy(data1[1].c, c, sizeof(char) * MN);
    memcpy(data1[0].s, s, sizeof(short) * MN);
    memcpy(data1[1].s, s, sizeof(short) * MN);
    memcpy(data1[0].us, us, sizeof(unsigned short) * MN);
    memcpy(data1[1].us, us, sizeof(unsigned short) * MN);
    memcpy(data1[0].i, i, sizeof(int) * MN);
    memcpy(data1[1].i, i, sizeof(int) * MN);
    memcpy(data1[0].ui, ui, sizeof(unsigned int) * MN);
    memcpy(data1[1].ui, ui, sizeof(unsigned int) * MN);
    memcpy(data1[0].ll, ll, sizeof(long long) * MN);
    memcpy(data1[1].ll, ll, sizeof(long long) * MN);
    memcpy(data1[0].ull, ull, sizeof(unsigned long long) * MN);
    memcpy(data1[1].ull, ull, sizeof(unsigned long long) * MN);
    memcpy(data1[0].f, f, sizeof(float) * MN);
    memcpy(data1[1].f, f, sizeof(float) * MN);
    memcpy(data1[0].d, d, sizeof(double) * MN);
    memcpy(data1[1].d, d, sizeof(double) * MN);
    memcpy(data1[0].t, t, sizeof(t));
    memcpy(data1[1].t, t, sizeof(t));
    
    stat = nc_create(filename, NC_CLOBBER | NC_NETCDF4, & ncid);
    check_err(stat, __LINE__, __FILE__);
     
    stat = nc_def_compound(ncid, sizeof(comp1), "comp1", & typeid1);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "sc1", 
			 offsetof(comp1, sc), NC_BYTE,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "uc1", 
			 offsetof(comp1, uc), NC_UBYTE,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "c1", 
			 offsetof(comp1, c), NC_CHAR,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "s1", 
			 offsetof(comp1, s), NC_SHORT,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "us1", 
			 offsetof(comp1, us), NC_USHORT,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "i1", 
			 offsetof(comp1, i), NC_INT,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "ui1", 
			 offsetof(comp1, ui), NC_UINT,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "ll1", 
			 offsetof(comp1, ll), NC_INT64,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "ull1", 
			 offsetof(comp1, ull), NC_UINT64,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "f1", 
			 offsetof(comp1, f), NC_FLOAT,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "d1", 
			 offsetof(comp1, d), NC_DOUBLE,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "t1", 
			 offsetof(comp1, t), NC_STRING,
			 ND, dim_sizes);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_def_dim(ncid, "rec", NC_UNLIMITED, & dimids[0]);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_def_dim(ncid, "dim1", 2, & dimids[1]);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_def_var(ncid, "var1", typeid1, 2, dimids, & varid1);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_enddef(ncid);
    check_err(stat, __LINE__, __FILE__);

    for(j = 0; j < 2; ++j) {
        start[0] = j;
        for(k = 0; k < 2; ++k) {
            start[1] = k;
            stat = nc_put_vara(ncid, varid1, start, count, (void*) & data1);
            check_err(stat, __LINE__, __FILE__);
        }
    }

    return 0;
}
