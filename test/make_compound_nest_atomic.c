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

typedef struct COMP1 {
    signed char sc1;
    unsigned char uc1;
    char c1;
    short s1;
    unsigned short us1;
    int i1;
    unsigned int ui1;
    long long ll1;
    unsigned long long ull1;
    float f1;
    double d1;
    char* t1[1];
} comp1;

typedef struct COMP2 {
    comp1 com1;
    signed char sc2;
    unsigned char uc2;
    char c2;
    short s2;
    unsigned short us2;
    int i2;
    unsigned int ui2;
    long long ll2;
    unsigned long long ull2;
    float f2;
    double d2;
    char* t2[1];
} comp2;

typedef struct COMP3 {
    comp2 com2;
    signed char sc3;
    unsigned char uc3;
    char c3;
    short s3;
    unsigned short us3;
    int i3;
    unsigned int ui3;
    long long ll3;
    unsigned long long ull3;
    float f3;
    double d3;
    char* t3[1];
} comp3;

int main(int argc, char** argv) {
    comp3 com3;
    comp2 com2;
    comp1 com1;
    char fuzz;
    const char *filename;
    int ncid, stat, j, k;
    int typeid1, dimids[2];
    int typeid2;
    int typeid3, varid3;
    size_t start[2], count[2];
    size_t dimlens[] = {2,2};
    char *strings[] = {"ab"};
    
    if (argc < 3 ) {
        fprintf(stderr, "usage: %s <fuzz> <out.nc>\n", argv[0]);
        exit(2);
    }
    
    fuzz = atoi(argv[1]);
    filename = argv[2];
  
    com3.sc3 = 0;
    com3.uc3 = 0;
    com3.c3 = 'a';
    com3.s3 = 0;
    com3.us3 = 0;
    com3.i3 = 0;
    com3.ui3 = 0;
    com3.ll3 = 0;
    com3.ull3 = 0;
    com3.f3 = 0;
    com3.d3 = 0;

    com2.sc2 = 0;
    com2.uc2 = 0;
    com2.c2 = 'a';
    com2.s2 = 0;
    com2.us2 = 0;
    com2.i2 = 0;
    com2.ui2 = 0;
    com2.ll2 = 0;
    com2.ull2 = 0;
    com2.f2 = 0;
    com2.d2 = 0;

    com1.sc1 = 0;
    com1.uc1 = 0;
    com1.c1 = 'a';
    com1.s1 = 0;
    com1.us1 = 0;
    com1.i1 = 0;
    com1.ui1 = 0;
    com1.ll1 = 0;
    com1.ull1 = 0;
    com1.f1 = 0;
    com1.d1 = 0;
    
    memcpy(& com2.com1, & com1, sizeof(com1));
    memcpy(& com3.com2, & com2, sizeof(com2));

    com3.t3[0] = malloc(3);
    com3.com2.t2[0] = malloc(3);
    com3.com2.com1.t1[0] = malloc(3);
    strcpy(com3.t3[0], strings[0]);
    strcpy(com3.com2.t2[0], strings[0]);
    strcpy(com3.com2.com1.t1[0], strings[0]);
        
    stat = nc_create(filename, NC_CLOBBER | NC_NETCDF4, & ncid);
    check_err(stat, __LINE__, __FILE__);
     
    stat = nc_def_compound(ncid, sizeof(comp1), "comp1", & typeid1);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_insert_compound(ncid, typeid1, "sc1", offsetof(comp1, sc1), NC_BYTE);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid1, "uc1", offsetof(comp1, uc1), NC_UBYTE);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid1, "c1", offsetof(comp1, c1), NC_CHAR);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid1, "s1", offsetof(comp1, s1), NC_SHORT);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid1, "us1", offsetof(comp1, us1), NC_USHORT);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid1, "i1", offsetof(comp1, i1), NC_INT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid1, "ui1", offsetof(comp1, ui1), NC_UINT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid1, "ll1", offsetof(comp1, ll1), NC_INT64);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid1, "ull1", offsetof(comp1, ull1), NC_UINT64);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid1, "f1", offsetof(comp1, f1), NC_FLOAT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid1, "d1", offsetof(comp1, d1), NC_DOUBLE);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid1, "t1", offsetof(comp1, t1), NC_STRING);
    check_err(stat, __LINE__, __FILE__);    

    /* --------------------- */
    
    stat = nc_def_compound(ncid, sizeof(comp2), "comp2", & typeid2);
    check_err(stat, __LINE__, __FILE__);

    /* Define nested members first to keep the type layout aligned with the C struct. */
    stat = nc_insert_compound(ncid, typeid2, "com1", offsetof(comp2, com1), typeid1);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_insert_compound(ncid, typeid2, "sc2", offsetof(comp2, sc2), NC_BYTE);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid2, "uc2", offsetof(comp2, uc2), NC_UBYTE);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid2, "c2", offsetof(comp2, c2), NC_CHAR);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid2, "s2", offsetof(comp2, s2), NC_SHORT);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid2, "us2", offsetof(comp2, us2), NC_USHORT);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid2, "i2", offsetof(comp2, i2), NC_INT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid2, "ui2", offsetof(comp2, ui2), NC_UINT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid2, "ll2", offsetof(comp2, ll2), NC_INT64);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid2, "ull2", offsetof(comp2, ull2), NC_UINT64);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid2, "f2", offsetof(comp2, f2), NC_FLOAT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid2, "d2", offsetof(comp2, d2), NC_DOUBLE);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid2, "t2", offsetof(comp2, t2), NC_STRING);
    check_err(stat, __LINE__, __FILE__);    

    /* ---------------------------------- */
    
    stat = nc_def_compound(ncid, sizeof(comp3), "comp3", & typeid3);
    check_err(stat, __LINE__, __FILE__);

    /* Define nested members first to keep the type layout aligned with the C struct. */
    stat = nc_insert_compound(ncid, typeid3, "com2", offsetof(comp3, com2), typeid2);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_insert_compound(ncid, typeid3, "sc3", offsetof(comp3, sc3), NC_BYTE);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid3, "uc3", offsetof(comp3, uc3), NC_UBYTE);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid3, "c3", offsetof(comp3, c3), NC_CHAR);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid3, "s3", offsetof(comp3, s3), NC_SHORT);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid3, "us3", offsetof(comp3, us3), NC_USHORT);
    check_err(stat, __LINE__, __FILE__);    
    
    stat = nc_insert_compound(ncid, typeid3, "i3", offsetof(comp3, i3), NC_INT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid3, "ui3", offsetof(comp3, ui3), NC_UINT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid3, "ll3", offsetof(comp3, ll3), NC_INT64);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid3, "ull3", offsetof(comp3, ull3), NC_UINT64);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid3, "f3", offsetof(comp3, f3), NC_FLOAT);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid3, "d3", offsetof(comp3, d3), NC_DOUBLE);
    check_err(stat, __LINE__, __FILE__);    

    stat = nc_insert_compound(ncid, typeid3, "t3", offsetof(comp3, t3), NC_STRING);
    check_err(stat, __LINE__, __FILE__);    
    
    /* ------------------------------------ */
    
    stat = nc_def_dim(ncid, "rec", NC_UNLIMITED, & dimids[0]);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_def_dim(ncid, "dim1", dimlens[1], & dimids[1]);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_def_var(ncid, "var3", typeid3, 2, dimids, & varid3);
    check_err(stat, __LINE__, __FILE__);
    
    stat = nc_enddef(ncid);
    check_err(stat, __LINE__, __FILE__);
        
    for(j = 0; j < dimlens[0]; ++j) {
        start[0] = j;
        start[1] = 0;
        count[0] = 1;
        count[1] = 1;
        for(k = 0; k < dimlens[1]; ++k) {
            start[1] = k;
            if (fuzz && j && k) {
                com3.com2.com1.sc1 = fuzz;
                com3.com2.com1.uc1 = fuzz;
                com3.com2.com1.c1 = fuzz + 'a';
                com3.com2.com1.s1 = fuzz;
                com3.com2.com1.us1 = fuzz;
                com3.com2.com1.i1 = fuzz;
                com3.com2.com1.ui1 = fuzz;
                com3.com2.com1.ll1 = fuzz;
                com3.com2.com1.ull1 = fuzz;
                com3.com2.com1.f1 = fuzz;
                com3.com2.com1.d1 = fuzz;
                strings[0] = "cd";
                strcpy(com3.com2.com1.t1[0], strings[0]);
                
                com3.com2.sc2 = fuzz + 1;
                com3.com2.uc2 = fuzz + 1;
                com3.com2.c2 = fuzz + 1 + 'a';
                com3.com2.s2 = fuzz + 1;
                com3.com2.us2 = fuzz + 1;
                com3.com2.i2 = fuzz + 1;
                com3.com2.ui2 = fuzz + 1;
                com3.com2.ll2 = fuzz + 1;
                com3.com2.ull2 = fuzz + 1;
                com3.com2.f2 = fuzz + 1;
                com3.com2.d2 = fuzz + 1;
                strings[0] = "ef";
                strcpy(com3.com2.t2[0], strings[0]);
                
                com3.sc3 = fuzz + 2;
                com3.uc3 = fuzz + 2;
                com3.c3 = fuzz + 2 + 'a';
                com3.s3 = fuzz + 2;
                com3.us3 = fuzz + 2;
                com3.i3 = fuzz + 2;
                com3.ui3 = fuzz + 2;
                com3.ll3 = fuzz + 2;
                com3.ull3 = fuzz + 2;
                com3.f3 = fuzz + 2;
                com3.d3 = fuzz + 2;
                strings[0] = "gh";
                strcpy(com3.t3[0], strings[0]);
            }
        
            /* printf("put j=%d k=%d\n", j, k); */
            stat = nc_put_vara(ncid, varid3, start, count, (void*) & com3);
            check_err(stat, __LINE__, __FILE__);
        }
    }
    
    nc_close(ncid);
    
    return 0;
}   
