/*
	For testing nccmp with netcdf file with padded header.
	Pass the argument 1 from the command-line to create a padded file.

	gcc padheader.c -L/net2/rsz/linux/netcdf/netcdf-3.6.1/lib -lnetcdf -o padheader
	./padheader unpadded.nc 0       
	./padheader padded.nc 1 

	20070116 rsz Created.
 */

#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

#define handle_error(status) {												\
		if (status != NC_NOERR)														\
			{																								\
																											\
				fprintf(stderr, "%s\n", nc_strerror(status));	\
				exit(-1);																			\
																											\
			}																								\
																											\
	}

int main(int argc, char *argv[])
{
	int pad = 0;
	int cdfid, varid1, varid2, status, recdimid, i;
	int dimids[4];
	float fp[32];
	size_t start[2], count[2];
	char* filename;

	if (argc == 3)
	{
		filename = argv[1];
		pad = atoi(argv[2]);
	} else
	{
		fprintf(stderr, "usage: %s <out.nc> <headerPadNumBytes>\n\n", argv[0]);
		return 1;
	}

	for(i=0; i < 32; ++i)
		fp[i] = (float)i;

	status = nc_create(filename, NC_CLOBBER, &cdfid);
	handle_error(status);

	status = nc_def_dim(cdfid, "test", 32, &dimids[1]); 
	handle_error(status);

	status = nc_def_dim(cdfid, "rec", NC_UNLIMITED, &recdimid); 
	handle_error(status);

	dimids[0] = recdimid;

	status = nc_def_var(cdfid, "test1", NC_FLOAT, 1, dimids+1, &varid1);
	handle_error(status);

	status = nc_def_var(cdfid, "test2", NC_FLOAT, 2, dimids, &varid2);
	handle_error(status);

	if ( pad > 0 ) {
		status = nc__enddef(cdfid, pad, 4, 0, 4);
	} else {
		status = nc_enddef(cdfid);
	}

	handle_error(status);
	
	status = nc_put_var_float(cdfid, varid1, fp);
	handle_error(status);

	start[0] = 0;
	start[1] = 0;
	count[0] = 1;
	count[1] = 32;
	status = nc_put_vara_float(cdfid, varid2, start, count, fp);
	handle_error(status);

	start[0] = 1;
	status = nc_put_vara_float(cdfid, varid2, start, count, fp);
	handle_error(status);

	status = nc_close(cdfid);
	handle_error(status);

	return 0;
}
