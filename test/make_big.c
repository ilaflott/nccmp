/*
Generate large file for benchmarking and testing.

	gcc make_big.c -I/usr/local/netcdf-4.3.3.1/include/ \
	               -L/usr/local/netcdf-4.3.3.1/lib -lnetcdf \
	               -o make_big
	./make_big out.nc 1
*/

#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <math.h>

#define handle_error(status) {											\
	if (status != NC_NOERR) {											\
		fprintf(stderr, "%s\n", nc_strerror(status));					\
		exit(-1);														\
	}																	\
}

int main(int argc, char *argv[])
{
	int fuzz = 0;
	int ti, zi, yi, xi;
	int ncid, varid, status;
	int dimids[4];
	int dimlens[4];
	float* f;
	const float F = -1.0;
	size_t start[4], count[4];
	char* fn;

	if (argc == 7) {
		dimlens[0] = atoi(argv[1]);
		dimlens[1] = atoi(argv[2]);
		dimlens[2] = atoi(argv[3]);
		dimlens[3] = atoi(argv[4]);
        fuzz = atoi(argv[5]);
        fn = argv[6];
    } else {
        fprintf(stderr, "usage: %s <nt> <nz> <ny> <nx> <fuzz> <out.nc>\n", argv[0]);
        fprintf(stderr, "\nexample: %s 10 20 30 40 1 tmp.nc\n", argv[0]);
        return 1;
    }

	status = nc_create(fn, NC_CLOBBER, & ncid);
	handle_error(status);

	status = nc_def_dim(ncid, "z", dimlens[1], & dimids[1]);
	handle_error(status);

	status = nc_def_dim(ncid, "y", dimlens[2], & dimids[2]);
	handle_error(status);

	status = nc_def_dim(ncid, "x", dimlens[3], & dimids[3]);
	handle_error(status);

	status = nc_def_dim(ncid, "t", NC_UNLIMITED, & dimids[0]);
	handle_error(status);

	status = nc_def_var(ncid, "var", NC_FLOAT, 4, dimids, & varid);
	handle_error(status);

	status = nc_enddef(ncid);

	f = malloc(sizeof(float) * dimlens[3]);
	for(xi = 0; xi < dimlens[3]; ++xi) {
		f[xi] = F;
	}

	count[0] = 1;
	count[1] = 1;
	count[2] = 1;
	count[3] = dimlens[3];
	start[3] = 0;
	for(ti = 0; ti < dimlens[0]; ++ti) {
		start[0] = ti;
		for(zi = 0; zi < dimlens[1]; ++zi) {
			start[1] = zi;
			for(yi = 0; yi < dimlens[2]; ++yi) {
				start[2] = yi;
				if ( fuzz &&
					 (zi == (dimlens[1]-1)) &&
					 (yi == (dimlens[2]-1)) ) {
					/* Set a value in last position of 3D slab. */
					f[dimlens[3]-1] = ti;
				} else {
					f[dimlens[3]-1] = F;
				}
				status = nc_put_vara_float(ncid, varid, start, count, f);
				handle_error(status);
			}
		}
	}

	status = nc_close(ncid);
	handle_error(status);

	free(f);

	return 0;
}
