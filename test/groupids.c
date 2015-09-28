/*
	Shows that absolute group names cannot be used with root ncid to
	get group id. You must query by short name using immediate parent ncid.

	gcc groupids.c -I/usr/local/netcdf-4.3.3.1/include -L/usr/local/netcdf-4.3.3.1/lib -lnetcdf -o groupids
	./groupids

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
	int ncid, ncid1, groupncid, ncid2, ncid3, subgroupncid;
	int varid, groupvarid, subgroupvarid, status, i;
	int dimids[4], groupdimids[4], subgroupdimids[4];
	float fp[LEN];
	size_t start[1], count[1];
	const char* f;
	const char* g;
       
    printf("NC_ENOGRP = %d\n", NC_ENOGRP);
       
    f = "group1.nc";
	nc_open(f, NC_NOWRITE, & ncid1);
	printf("f = %s, ncid = %d\n", f, ncid1);
	g = "group";
	nc_inq_ncid(ncid1, g, & groupncid);
	printf("    g = %s, ncid = %d\n", g, groupncid);
	g = "subgroup";
	status = nc_inq_ncid(ncid1, g, & subgroupncid);
	printf("    g = %s, ncid = %d, status = %d\n", g, subgroupncid, status);
	g = "/group/subgroup";
	status = nc_inq_ncid(ncid1, g, & subgroupncid);
	printf("    g = %s, ncid = %d, status = %d\n", g, subgroupncid, status);	
	g = "subgroup";
	status = nc_inq_ncid(groupncid, g, & subgroupncid);
	printf("    g = %s, ncid = %d, status = %d\n", g, subgroupncid, status);
	g = "group3";
	nc_inq_ncid(ncid1, g, & groupncid);
	printf("    g = %s, ncid = %d\n", g, groupncid);

	/* Repeat */
    f = "group1.nc";
	nc_open(f, NC_NOWRITE, & ncid2);
	printf("f = %s, ncid = %d\n", f, ncid2);
	g = "group";
	status = nc_inq_ncid(ncid2, g, & groupncid);
	printf("    g = %s, ncid = %d, status = %d\n", g, groupncid, status);
	g = "subgroup";
	status = nc_inq_ncid(groupncid, g, & subgroupncid);
	printf("        g = %s, ncid = %d, status = %d\n", g, subgroupncid, status);
	g = "subgroup";
	status = nc_inq_ncid(ncid2, g, & groupncid);
	printf("        g = %s, ncid = %d, status = %d\n", g, groupncid, status);
	g = "group3";
	status = nc_inq_ncid(ncid2, g, & groupncid);
	printf("    g = %s, ncid = %d, status = %d\n", g, groupncid, status);
	
    f = "group2.nc";
	nc_open(f, NC_NOWRITE, & ncid3);
	printf("f = %s, ncid = %d\n", f, ncid3);
	g = "group";
	status = nc_inq_ncid(ncid3, g, & groupncid);
	printf("    g = %s, ncid = %d, status = %d\n", g, groupncid, status);
	g = "subgroup";
	status = nc_inq_ncid(ncid3, g, & groupncid);
	printf("    g = %s, ncid = %d, status = %d\n", g, groupncid, status);
	g = "subgroup2";
	status = nc_inq_ncid(ncid3, g, & groupncid);
	printf("        g = %s, ncid = %d, status = %d\n", g, groupncid, status);
	g = "/group/subgroup2";
	status = nc_inq_ncid(ncid3, g, & groupncid);
	printf("        g = %s, ncid = %d, status = %d\n", g, groupncid, status);
	g = "group2";
	status = nc_inq_ncid(ncid3, g, & groupncid);
	printf("g = %s, ncid = %d, status = %d\n", g, groupncid, status);

/*
	if ((res = nc_inq_grps(root_ncid, & ngroups, NULL)))
        return res;
           
    ncids = malloc(sizeof(int) * numgrps);
    if ((res = nc_inq_grps(root_ncid, NULL, ncids)))
        return res;
*/         
	return 0;
}
