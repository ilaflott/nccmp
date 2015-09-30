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

#define B 3
#define M 2
#define M4 M*M*M*M
#define ND 4

#define UNK ((int)-1)
#define FOO ((int)0)
#define BAR ((int)1)
#define SIC ((int)2)
#define WUF ((int)3)
#define ZYP ((int)4)
#define NUM_ENUM 6
typedef int enum1;
typedef unsigned char opaq1[B];
typedef nc_vlen_t vlen1;

typedef struct COMP0 {
    long long ll[M4];
} comp0;

typedef struct COMP1 {
    int    i[M4];
    char** t[M4];
    enum1  e[M4];
    vlen1  v[M4];
    opaq1  o[M4];
    comp0  c[M4];
} comp1;

int main(int argc, char** argv) {
    int  stat, ncid;  
    int i, j, k, typeid0, typeid1, varid1, typeid_e, typeid_v, typeid_o;
    int dimids[4];
    int field_dim_sizes[] = {M, M, M, M};
    size_t start[] = {0,0,0,0};
    size_t count[] = {1,1,1,1};
    comp1 data1[2];
    char fuzz = 0;
    char *filename;
    int eval;
    char *string;
    short *shortp;
    
    if (argc < 3 ) {
        fprintf(stderr, "usage: %s <fuzz> <out.nc>\n", argv[0]);
        exit(2);
    }
    
    fuzz = atoi(argv[1]);
    filename = argv[2];

    stat = nc_create(filename, NC_CLOBBER | NC_NETCDF4, & ncid);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_def_compound(ncid, sizeof(comp0), "comp0", & typeid0);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid0, "ll",
             offsetof(comp0, ll), NC_INT64,
             ND, field_dim_sizes);
    check_err(stat, __LINE__, __FILE__);


    stat = nc_def_enum(ncid, NC_INT, "enum1", & typeid_e);
    check_err(stat, __LINE__, __FILE__);

    eval = UNK;
    stat = nc_insert_enum(ncid, typeid_e, "UNK", & eval);
    check_err(stat, __LINE__, __FILE__);

    eval = FOO;
    stat = nc_insert_enum(ncid, typeid_e, "FOO", & eval);
    check_err(stat, __LINE__, __FILE__);

    eval = BAR;
    stat = nc_insert_enum(ncid, typeid_e, "BAR", & eval);
    check_err(stat, __LINE__, __FILE__);

    eval = SIC;
    stat = nc_insert_enum(ncid, typeid_e, "SIC", & eval);
    check_err(stat, __LINE__, __FILE__);

    eval = WUF;
    stat = nc_insert_enum(ncid, typeid_e, "WUF", & eval);
    check_err(stat, __LINE__, __FILE__);

    eval = ZYP;
    stat = nc_insert_enum(ncid, typeid_e, "ZYP", & eval);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_def_vlen(ncid, "vlen1", NC_INT, & typeid_v);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_def_opaque(ncid, B, "opaq1", & typeid_o);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_def_compound(ncid, sizeof(comp1), "comp1", & typeid1);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "i",
             offsetof(comp1, i), NC_INT,
             ND, field_dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "t",
             offsetof(comp1, t), NC_STRING,
             ND, field_dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "e",
             offsetof(comp1, e), typeid_e,
             ND, field_dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "v",
             offsetof(comp1, v), typeid_v,
             ND, field_dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "o",
             offsetof(comp1, o), typeid_o,
             ND, field_dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_insert_array_compound(ncid, typeid1, "c",
             offsetof(comp1, c), typeid0,
             ND, field_dim_sizes);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_def_dim(ncid, "rec", NC_UNLIMITED, & dimids[0]);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_def_dim(ncid, "dim1", 1, & dimids[1]);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_def_dim(ncid, "dim2", 2, & dimids[2]);
    check_err(stat, __LINE__, __FILE__);

    stat = nc_def_var(ncid, "var1", typeid1, 3, dimids, & varid1);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_enddef(ncid);
    check_err(stat, __LINE__, __FILE__);

    string = calloc(B, sizeof(char));

    for(i = 0; i < 2; ++i) {
        for(j = 0; j < M4; ++j) {
            data1[i].i[j] = j + fuzz*100*(i+1);

            string[0] = 'a' + fuzz + i;
            string[1] = 'a' + j;
            data1[i].t[j] = calloc(B, sizeof(char));
            strcpy(data1[i].t[j], string);

            data1[i].e[j] = (FOO + j + fuzz + i) % NUM_ENUM + UNK;

            data1[i].v[j].len = fuzz;
            data1[i].v[j].p = malloc( sizeof(int) * fuzz );
            for(k = 0; k < fuzz; ++k) {
                ((int*)data1[i].v[j].p)[k] = k + j + fuzz;
            }

            memset(data1[i].o[j], 0, B);
            shortp = (short*) & (data1[i].o[j][1]);
            *shortp = fuzz + j + i*16;

            for(k = 0; k < M4; ++k) {
                data1[i].c[j].ll[k] = fuzz*1000000*(i+1) + 1000*(j+1) + k;
            }
        }

        start[2] = i;
        stat = nc_put_vara(ncid, varid1, start, count, (void*) & data1);
        check_err(stat, __LINE__, __FILE__);
    }

    free(string);

	stat = nc_close(ncid);
    check_err(stat, __LINE__, __FILE__);

    return 0;
}
