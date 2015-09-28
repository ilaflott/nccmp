#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>
#include <string.h>

#define LOG() printf("%s:%d\n", __FILE__, __LINE__);

void
check_err(const int stat, const int line, const char *file) {
    if (stat != NC_NOERR) {
        (void)fprintf(stderr,"line %d of %s: %s\n", line, file, nc_strerror(stat));
        fflush(stderr);
        exit(1);
    }
}

typedef nc_vlen_t vlen1;
typedef nc_vlen_t vlen2;

typedef struct COMP1 {
    double d1;
    vlen1 v1;
} comp1;

typedef struct COMP2 {
    int i2;
    vlen2 v2;
} comp2;

int main(int argc, char** argv) {
    const char *filename;
    int ncid, stat, varid, dimids[4], typeid_vlen1, typeid_vlen2;
    int typeid_comp1, typeid_comp2;
    int fuzz;
    
    if (argc < 3 ) {
        fprintf(stderr, "usage: %s <fuzz> <in.nc>\n", argv[0]);
        exit(2);
    }
    
    fuzz = atoi(argv[1]);
    filename = argv[2];
  
    stat = nc_create(filename, NC_CLOBBER | NC_NETCDF4, & ncid);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_def_vlen(ncid, "vlen1", NC_INT, & typeid_vlen1);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_def_compound(ncid, sizeof(comp1), "comp1", & typeid_comp1);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_insert_compound(ncid, typeid_comp1, "d1", offsetof(comp1, d1), NC_DOUBLE);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_insert_compound(ncid, typeid_comp1, "v1", offsetof(comp1, v1), typeid_vlen1);
    check_err(stat,__LINE__,__FILE__);
    
      stat = nc_def_vlen(ncid, "vlen2", typeid_comp1, & typeid_vlen2);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_def_compound(ncid, sizeof(comp2), "comp2", & typeid_comp2);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_insert_compound(ncid, typeid_comp2, "i2", offsetof(comp2, i2), NC_INT);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_insert_compound(ncid, typeid_comp2, "v2", offsetof(comp2, v2), typeid_vlen2);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_def_dim(ncid, "dim1", 1, & dimids[0]);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_def_dim(ncid, "dim2", 2, & dimids[1]);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_def_var(ncid, "var1", typeid_comp2, 2, dimids, & varid);
    check_err(stat,__LINE__,__FILE__);

    stat = nc_enddef(ncid);
    check_err(stat,__LINE__,__FILE__); 
 
    comp1 *c1p = malloc(sizeof(comp1) * 2);
    c1p[0].d1 = 2.0 + fuzz;
        int ip1[] = {3 + fuzz, 4 + fuzz};
        c1p[0].v1.len = 2;
        c1p[0].v1.p = ip1;
    c1p[1].d1 = 5.0 + fuzz;
        int ip2[] = {6 + fuzz, 7 + fuzz};
        c1p[1].v1.len = 2;
        c1p[1].v1.p = ip2;
    comp2 *c2p = malloc(sizeof(comp2) * 2);
    c2p[0].i2 = 1 + fuzz;
    c2p[0].v2.len = 2;
    c2p[0].v2.p = c1p;

    comp1 *c1p2 = malloc(sizeof(comp1) * 2);
    c1p2[0].d1 = 13.0 + fuzz;
        int ip3[] = {14 + fuzz, 15 + fuzz};
        c1p2[0].v1.len = 2;
        c1p2[0].v1.p = ip3;
    c1p2[1].d1 = 16.0 + fuzz;
        int ip4[] = {17 + fuzz, 18 + fuzz};
        c1p2[1].v1.len = 2;
        c1p2[1].v1.p = ip4;
    c2p[1].i2 = 12 + fuzz;
    c2p[1].v2.len = 2;
    c2p[1].v2.p = c1p2;
 
    stat = nc_put_var(ncid, varid, (void*) c2p);
    check_err(stat,__LINE__,__FILE__);
    
    stat = nc_close(ncid);
    check_err(stat,__LINE__,__FILE__);

    return 0;
}
