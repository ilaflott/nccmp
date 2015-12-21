/*
Generate file with or without hdf encodings such as shuffle, deflate, checksum.

	gcc make_nc4_encodings.c \
	    -I/usr/local/netcdf-4.3.3.1/include/ \
	    -L/usr/local/netcdf-4.3.3.1/lib -lnetcdf \
	    -o make_nc4_encodings
	./make_nc4_encodings out.nc 1
*/

#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <string.h>

#define handle_error(status) {											\
	if (status != NC_NOERR) {											\
		fprintf(stderr, "%s\n", nc_strerror(status));					\
		exit(-1);														\
	}																	\
}

int main(int argc, char *argv[])
{
	char* fn;
	int enable = 0;
	const int N = 10;
	const int NDIMS = 1;
	size_t dimlens[NDIMS];
	size_t start[N], count[N];
	int dimids[NDIMS], varid, status, ncid, data[N];
	size_t chunksizes[NDIMS];
	
	if (argc == 3) {
		enable = atoi(argv[1]);
		fn = argv[2];
    } else {
        fprintf(stderr, "usage: %s <enable> <out.nc>\n", argv[0]);
        fprintf(stderr, "\nexample: %s 1 tmp.nc\n", argv[0]);
        return 1;
    }

	status = nc_create(fn, NC_CLOBBER | NC_NETCDF4, & ncid);
	handle_error(status);

	dimlens[0] = N;
	status = nc_def_dim(ncid, "dim1", dimlens[0], & dimids[0]);
	handle_error(status);

	status = nc_def_var(ncid, "var1", NC_INT, 1, dimids, & varid);
	handle_error(status);

	if (enable) {
		status = nc_def_var_fletcher32(ncid, varid, NC_FLETCHER32);
		handle_error(status);
		
		chunksizes[0] = 2;
		status = nc_def_var_chunking(ncid, varid, NC_CHUNKED, chunksizes);
		handle_error(status);
		
		status = nc_def_var_deflate(ncid, varid, 
									1 /* shuffle */, 
									1 /* deflate */,
									5 /* deflate_level */);
        handle_error(status);
        
        status = nc_def_var_endian(ncid, varid, NC_ENDIAN_BIG);
        handle_error(status);
	} else {
        status = nc_def_var_endian(ncid, varid, NC_ENDIAN_LITTLE);
        handle_error(status);
	}		

	status = nc_enddef(ncid);

	memset(data,  0, N * sizeof(int));
	memset(start, 0, NDIMS * sizeof(size_t));
	memset(count, 0, NDIMS * sizeof(size_t));
	count[0] = N;
	status = nc_put_vara_int(ncid, varid, start, count, data);
	handle_error(status);
	
	status = nc_close(ncid);
	handle_error(status);

	return 0;
}
