/*
   Copyright (C) 2004, 2009 Remik Ziemlinski <first d0t surname att n0aa d0t g0v>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#include <nccmp_ncinfo.h>

int nccmp_non_rec_vars(int ncid, nccmp_strlist_t* list)
{
    int status;
    int recid;
    int varid;
    int dimids[NC_MAX_VAR_DIMS];
    int ndims;
    int nvars;
    int natts;
    int i;
    nc_type type;
    char name[NC_MAX_NAME];

    /* Query the number of variables */
    status = nc_inq_nvars (ncid, &nvars);
    if (status != NC_NOERR) return EXIT_FATAL;

    /* record dimension id */
    status = nc_inq_unlimdim(ncid, &recid);
    if (status == -1)
    {
        /* no record dimension */
        recid = -1;
    }

    for(i=0; i < nvars; ++i)
    {
        /* Query the variable */
        varid = i;
        status = nc_inq_var(ncid, varid, name, &type, &ndims, dimids, &natts);
        if (status != NC_NOERR) return EXIT_FATAL;

        if (nccmp_has_rec_dim(dimids, ndims, recid) == 0)
        {
            nccmp_add_to_strlist(list, name);
        }
    }

    return EXIT_SUCCESS;
}

int nccmp_rec_vars(int ncid, nccmp_strlist_t* list)
{
    int status;
    int recid = -1;
    int varid;
    int dimids[NC_MAX_VAR_DIMS];
    int ndims;
    int nvars;
    int natts;
    int i;
    nc_type type;
    char name[NC_MAX_NAME];

    /* Query the number of variables */
    status = nc_inq_nvars (ncid, &nvars);
    if (status != NC_NOERR) return EXIT_FATAL;

    /* record dimension id */
    nc_inq_unlimdim(ncid, & recid);

    for(i=0; i < nvars; ++i)
    {
        /* Query the variable */
        varid = i;
        status = nc_inq_var(ncid, varid, name, &type, &ndims, dimids, &natts);
        if (status != NC_NOERR) return EXIT_FATAL;

        if (nccmp_has_rec_dim(dimids, ndims, recid)) {
            nccmp_add_to_strlist(list, name);
        }
    }

    return EXIT_SUCCESS;
}

int nccmp_all_var_names(int ncid, nccmp_strlist_t* list)
{
    nccmp_strlist_t *nonrecvars, *recvars;
    int status;
  
    status = EXIT_SUCCESS;
    nonrecvars = nccmp_new_strlist(NC_MAX_VARS);
    recvars = nccmp_new_strlist(NC_MAX_VARS);

    if (!nonrecvars) {
        status = EXIT_FATAL;
        goto recover;
    }

    if (!recvars) {
        status = EXIT_FATAL;
        goto recover;
    }

    if(nccmp_non_rec_vars(ncid, nonrecvars) != EXIT_SUCCESS)
    {
        status = EXIT_FATAL;
        goto recover;
    }

    if(nccmp_rec_vars(ncid, recvars) != EXIT_SUCCESS)
    {
        status = EXIT_FATAL;
        goto recover;
    }

    if(nccmp_union_strlist(nonrecvars, recvars, list) != EXIT_SUCCESS)
    {
        status = EXIT_FATAL;
        goto recover;
    }

 recover:
    nccmp_free_strlist(& recvars);
    nccmp_free_strlist(& nonrecvars);

    return status;
}

int nccmp_rec_info(int ncid, int* recid, char* name, size_t* size)
{
    int status;

    /* get ID of unlimited dimension */
    status = nc_inq_unlimdim(ncid, recid);
    if (status != NC_NOERR) return EXIT_FATAL;

    if (*recid < 0)
    {
        /* no record */
        strcpy(name,"");
        *size = 0;
        return EXIT_SUCCESS;
    }

    /* Query the dimension */
    status = nc_inq_dim (ncid, *recid, name, size);
    if (status != NC_NOERR) return EXIT_FATAL;

    return EXIT_SUCCESS;
}
int nccmp_has_rec_dim(int* dimids, int ndims, const int recid)
{
    int i;

    if ( (ndims < 1) || (recid < 0) ) {
        return 0;
    }

    for(i = 0; i < ndims; ++i) {
        if (recid == dimids[i]) {
            return 1;
        }
    }

    return 0;
}
int nccmp_nc_type_size(nc_type type)
{
    switch(type) {
        case NC_BYTE:
        case NC_CHAR:
        case NC_UBYTE:
            return 1;
            break;

        case NC_SHORT:
        case NC_USHORT:
            return 2;
            break;

        case NC_INT:
        case NC_FLOAT:
        case NC_UINT:
            return 4;
            break;

        case NC_DOUBLE:
        case NC_INT64:
        case NC_UINT64:
            return 8;
            break;
    }
    return 0;
}
