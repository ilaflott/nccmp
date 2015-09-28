/*
	Generate example group file.

	gcc make_group.c -I/usr/local/netcdf-4.3.3.1/include \
	                 -L/usr/local/netcdf-4.3.3.1/lib -lnetcdf \
	                 -o make_group
	./make_group out.nc

 */

#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <math.h>
#include <string.h>

#define handle_error(status) {												\
		if (status != NC_NOERR)												\
		{													    			\
			fprintf(stderr, "%s\n", nc_strerror(status));					\
			exit(-1);														\
		}																	\
}

#define LEN 4

int main(int argc, char *argv[])
{
	int group = 0;
	int ncid, groupncid, subgroupncid;
	int varid, groupvarid, subgroupvarid, status, i;
	int dimids[4], groupdimids[4], subgroupdimids[4];
	float fp[LEN];
	size_t start[1], count[1];
	char* filename;

	if (argc == 2)
    {
        filename = argv[1];
    }	else
    {
        fprintf(stderr, "usage: %s <out.nc>\n", argv[0]);
        fprintf(stderr, "\nexample: %s tmp.nc\n", argv[0]);
        return 1;
    }
        
	for(i=0; i < LEN; ++i) 
    {
		fp[i] = (float) ((i+1) * group);
    }
        
	status = nc_create(filename, NC_CLOBBER | NC_NETCDF4, & ncid);
	handle_error(status);

	/* Root */
	status = nc_def_dim(ncid, "dim1", LEN, & dimids[0]);
	handle_error(status);

	status = nc_def_var(ncid, "var1", NC_FLOAT, 1, dimids, & varid);
	handle_error(status);

	status = nc_put_att_text(ncid, varid, "desc", 4, "test");
	handle_error(status);

	/* Group */
	status = nc_def_grp(ncid, "group", & groupncid);
	handle_error(status);

	status = nc_def_dim(groupncid, "dim1", LEN, & groupdimids[0]);
	handle_error(status);

	status = nc_def_var(groupncid, "var1", NC_FLOAT, 1, groupdimids, & groupvarid);
	handle_error(status);

	status = nc_put_att_text(groupncid, groupvarid, "desc", 4, "test");
	handle_error(status);

	/* Subgroup */
	status = nc_def_grp(groupncid, "subgroup", & subgroupncid);
	handle_error(status);

	status = nc_def_dim(subgroupncid, "dim1", LEN, & subgroupdimids[0]);
	handle_error(status);

	status = nc_def_var(subgroupncid, "var1", NC_FLOAT, 1, subgroupdimids, & subgroupvarid);
	handle_error(status);

	status = nc_put_att_text(subgroupncid, subgroupvarid, "desc", 4, "test");
	handle_error(status);

	status = nc_enddef(ncid);
	handle_error(status);
	
	start[0] = 0;
	count[0] = LEN;
	status = nc_put_vara_float(ncid, varid, start, count, fp);
	handle_error(status);

	start[0] = 0;
	count[0] = LEN;
	status = nc_put_vara_float(groupncid, groupvarid, start, count, fp);
	handle_error(status);

	start[0] = 0;
	count[0] = LEN;
	status = nc_put_vara_float(subgroupncid, subgroupvarid, start, count, fp);
	handle_error(status);

	status = nc_close(ncid);
	handle_error(status);

	return 0;
}
