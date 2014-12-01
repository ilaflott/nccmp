/*
Prints a Netcdf file's format, header size and if
compression and chunking are on.

20090915 rsz Created.
20090917 rsz Tested.
*/
#include <stdlib.h>
#include <netcdf.h>
#include <stdio.h>
#include <nc4internal.h>
#include <nc.h>

#define handle_error(status) {                                      \
    if (status != NC_NOERR)                                         \
            {                                                       \
                    fprintf(stderr, "%s\n", nc_strerror(status));   \
                    exit(-1);                                       \
            }                                                       \
    }

void PrintHeaderSize(int ncid)
{
    NC* ncp;
    int status;
    NC_FILE_INFO_T *nc;
    
    nc = nc4_find_nc_file(ncid);
    if (nc->nc4_info)
        /* This isn't a classic v3 netcdf file, so abort. */
        return;
          
    status = NC_check_id(nc->int_ncid, &ncp);
    if (status ) handle_error(status);
   
    /* position of the first (non-record) var, same as ->xsz */
    printf("header_size=%lld bytes\n", (long long int)ncp->begin_var);
}

int main(int argc, char* argv[])
{
    int status, ncid, format, storage, i, j, ndim, natt, nvar, shuffle, deflate, deflate_level;
    size_t chunksizes[NC_MAX_DIMS];
    int dimids[NC_MAX_DIMS];
    char * filenamein;
    char name[NC_MAX_NAME];
    
    if (argc < 2) 
    {
        printf("Error: input file not given.\n");
        exit(-1);
    }
    
    filenamein = argv[1];
    status = nc_open(filenamein, NC_NOWRITE, &ncid);
    if (status != NC_NOERR) handle_error(status);
   
    status = nc_inq_format(ncid, &format);
    if (status != NC_NOERR) handle_error(status);
     
    switch (format) 
    {
        case NC_FORMAT_CLASSIC:
        printf("format=NC_FORMAT_CLASSIC\n");
        PrintHeaderSize(ncid);
        break;

        case NC_FORMAT_64BIT:
        printf("format=NC_FORMAT_64BIT\n");
        PrintHeaderSize(ncid);        
        break;
        
        case NC_FORMAT_NETCDF4:
        printf("format=NC_FORMAT_NETCDF4\n");
        break;

        case NC_FORMAT_NETCDF4_CLASSIC:
        printf("format=NC_FORMAT_NETCDF4_CLASSIC\n");
        break;
        
        default:
        break;
    }

    status = nc_inq_nvars(ncid, &nvar);
    if (status != NC_NOERR) handle_error(status);
    
    for(i=0; i < nvar; ++i)
    {
        status = nc_inq_varname(ncid, i, name);
        if (status != NC_NOERR) handle_error(status);

        status = nc_inq_var_chunking(ncid, i, &storage, chunksizes);
        if (status != NC_NOERR) handle_error(status);
        
        status = nc_inq_var_deflate(ncid, i, &shuffle, &deflate, &deflate_level);
        if (status != NC_NOERR) handle_error(status);

        printf("var=%s, shuffle=%d, deflate=%d, deflate_level=%d, chunked=%d\n",
        name, shuffle, deflate, deflate_level, storage != NC_CONTIGUOUS);
    }
    
    nc_close(ncid);
    
    return 0;
}
