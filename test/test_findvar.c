/* Private development sanity check not intended for 'make check'. */
#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <nc.h>

#define handle_error(status) {                                       \
        if (status != 0) {                                           \
            printf("Status fatal. Aborting.\n");                     \
            exit(1);                                                 \
        }                                                            \
    }

int main(int argc, char* argv[])
{
    int ncid, varid, status;
    NC* ncp;
    NC_var* var;

    status = nc_open("test1a.nc", NC_NOWRITE, &ncid);
    if (status != NC_NOERR)
        handle_error(status);

    status = NC_check_id(ncid, &ncp);
    if (status != NC_NOERR )
        handle_error(status);

    printf("begin_var = %d, begin_rec = %d, recsize = %d, numrecs = %d\n",
            (int)ncp->begin_var, (int)ncp->begin_rec, (int)ncp->recsize, (int)ncp->numrecs);

    status = nc_inq_varid(ncid, "data1", &varid);
    if (status != NC_NOERR )
        handle_error(status);

    /* These are unstable on i686 unless compiled with -D_FILE_OFFSET_BITS=64. */
    var = NC_lookupvar(ncp, varid);
    printf("offset = %d, len = %d\n", (int)var->begin, (int)var->len);

    /*
        varid = NC_findvar(&(ncp->vars), "data1", &var);
        if (varid != -1) {
        printf("offset = %d, len = %d\n", (int)var->begin, (int)var->len);
        return 1;
        }
    */

    return 0;
}
