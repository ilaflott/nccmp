#include <stdlib.h>
#include <nccmp_constants.h>
#include <nccmp_error.h>
#include <nccmp_format.h>
#include <nccmp_group.h>
#include <nccmp_io.h>
#include <nccmp_metadata.h>
#include <nccmp_opt.h>
#include <nccmp_common.h>

#if HAVE_NC_H
    #include <nc.h>
    #include <nc3internal.h>
#endif

PRINT_SETUP()

int nccmp_cmp_att(int ncid1, int ncid2, int varid1, int varid2,
           const char* name, const char* varname, nccmp_opt_t* opts)
{
    int ncstatus, status;
    nc_type type1, type2;
    size_t lenp1, lenp2;
    static const int MAX_STRLEN = 8192;
    char typestr1[MAX_STRLEN];
    char typestr2[MAX_STRLEN];

    status = EXIT_SUCCESS;
    ncstatus = nc_inq_att(ncid1, varid1, name, &type1, &lenp1);
    if (ncstatus != NC_NOERR) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : VARIABLE \"%s\" IS MISSING ATTRIBUTE WITH NAME \"%s\" IN FILE \"%s\"\n", varname, name, opts->file1);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            status = EXIT_DIFFER;
        }
        return status;
    }
    ncstatus = nc_inq_att(ncid2, varid2, name, &type2, &lenp2);
    if (ncstatus != NC_NOERR) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : VARIABLE \"%s\" IS MISSING ATTRIBUTE WITH NAME \"%s\" IN FILE \"%s\"\n", varname, name, opts->file2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            status = EXIT_DIFFER;
        }
        return status;
    }
    if (type1 != type2) {
        if (!opts->quiet) {
            nccmp_nc_type_name_to_str(type1, typestr1, ncid1, opts->debug);
            nccmp_nc_type_name_to_str(type2, typestr2, ncid2, opts->debug);
            PRINT_DIFF(opts->debug, "DIFFER : TYPES : ATTRIBUTE : %s : VARIABLE : %s : %s <> %s\n", name, varname, typestr1, typestr2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            status = EXIT_DIFFER;
        }
        return status;
    }

    if (lenp1 != lenp2) {
        if (!opts->quiet) {
            nccmp_pretty_print_att(ncid1, varname, varid1, name, typestr1, MAX_STRLEN);
            nccmp_pretty_print_att(ncid2, varname, varid2, name, typestr2, MAX_STRLEN);
            PRINT_DIFF(opts->debug, "DIFFER : LENGTHS : ATTRIBUTE : %s : VARIABLE : %s : %lu <> %lu : VALUES : ", name, varname, (unsigned long)lenp1, (unsigned long)lenp2);
        }
        switch(type1) {
        case NC_CHAR:
            if (!opts->quiet) {
                /* Quote strings. */
                fprintf(stderr, "\"%s\" : \"%s\"\n", typestr1, typestr2);
            }
            if (strcmp(typestr1,typestr2) == 0) {
                /* Same text, but invisible trailing nulls because lengths differ. */
                if (opts->warn[NCCMP_W_EOS] || opts->warn[NCCMP_W_ALL]) {
                    /* Pass */
                } else {
                    status = EXIT_DIFFER;
                    return status;
                }
            }
            break;
        default:
            if (!opts->quiet) {
                /* Unquoted. */
                fprintf(stderr, "%s : %s\n", typestr1, typestr2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status = EXIT_DIFFER;
                return status;
            }
            break;
        }
    }
    if (nccmp_cmp_att_val(ncid1, ncid2, varid1, varid2, name, lenp1, lenp2, type1) != EXIT_SUCCESS) {
        if (!opts->quiet) {
            nccmp_pretty_print_att(ncid1, varname, varid1, name, typestr1, MAX_STRLEN);
            nccmp_pretty_print_att(ncid2, varname, varid2, name, typestr2, MAX_STRLEN);
            PRINT_DIFF(opts->debug, "DIFFER : VARIABLE : %s : ATTRIBUTE : %s : VALUES : ", varname, name);

            switch(type1) {
            case NC_CHAR:
                /* Quote strings. */
                fprintf(stderr, "\"%s\" <> \"%s\"\n", typestr1, typestr2);
                break;
            default:
                /* Unquoted. */
                fprintf(stderr, "%s <> %s\n", typestr1, typestr2);
                break;
            }
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            status = EXIT_DIFFER;
            return status;
        }
    }
    return EXIT_SUCCESS;
}

int nccmp_cmp_att_val(int nc1, int nc2, int varid1, int varid2, const char* name, int len1,
        int len2, nc_type type)
{
    signed char        *sc1 = 0,  *sc2 = 0;
    char               *c1 = 0,   *c2 = 0;
    double             *d1 = 0,   *d2 = 0;
    float              *f1 = 0,   *f2 = 0;
    int                *i1 = 0,   *i2 = 0;
    long long          *l1 = 0,   *l2 = 0;
    short              *s1 = 0,   *s2 = 0;
    char               **st1 = 0, **st2 = 0;
    unsigned char      *uc1 = 0,  *uc2 = 0;
    unsigned int       *ui1 = 0,  *ui2 = 0;
    unsigned long long *ul1 = 0,  *ul2 = 0;
    unsigned short     *us1 = 0,  *us2 = 0;

    int status, last_status = EXIT_SUCCESS;
    int i;
    int minlen = nccmp_min_int(len1, len2);

    if (name == NULL) {
        return NC_EINVAL;
    }

    #define CMP_ATT_VAL(PTR, TYPE, NAME) {                                     \
        PTR##1 = XMALLOC(TYPE, len1);                                          \
        PTR##2 = XMALLOC(TYPE, len2);                                          \
        status = nc_get_att_##NAME(nc1, varid1, name, PTR##1);                 \
        if (status != NC_NOERR) {                                              \
            last_status = EXIT_FAILED;                                         \
            break;                                                             \
        }                                                                      \
        status = nc_get_att_##NAME(nc2, varid2, name, PTR##2);                 \
        if (status != NC_NOERR) {                                              \
            last_status = EXIT_FAILED;                                         \
            break;                                                             \
        }                                                                      \
        for(i = 0; i < minlen; ++i) {                                          \
            if (PTR##1[i] != PTR##2[i])    {                                   \
                last_status = EXIT_DIFFER;                                     \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    }

    switch(type)
    {
    case NC_BYTE:   CMP_ATT_VAL(sc, signed char,        schar);     break;
    case NC_CHAR:   CMP_ATT_VAL(c,  char,               text);      break;
    case NC_DOUBLE: CMP_ATT_VAL(d,  double,             double);    break;
    case NC_FLOAT:  CMP_ATT_VAL(f,  float,              float);     break;
    case NC_INT:    CMP_ATT_VAL(i,  int,                int);       break;
 /* case NC_LONG: Alias of NC_INT. */
    case NC_INT64:  CMP_ATT_VAL(l,  long long,          longlong);  break;
    case NC_SHORT:  CMP_ATT_VAL(s,  short,              short);     break;
    case NC_UBYTE:  CMP_ATT_VAL(uc, unsigned char,      uchar);     break;
    case NC_UINT:   CMP_ATT_VAL(ui, unsigned int,       uint);      break;
    case NC_UINT64: CMP_ATT_VAL(ul, unsigned long long, ulonglong); break;
    case NC_USHORT: CMP_ATT_VAL(us, unsigned short,     ushort);    break;
    case NC_STRING:
        st1 = XMALLOC(char*, len1);
        st2 = XMALLOC(char*, len2);
        for(i = 0; i < minlen; ++i) {
            st1[i] = XMALLOC(char, NC_MAX_NAME);
            st2[i] = XMALLOC(char, NC_MAX_NAME);
            memset(st1[i], 0, NC_MAX_NAME);
            memset(st2[i], 0, NC_MAX_NAME);
        }
        status = nc_get_att_string(nc1, varid1, name, st1);
        if (status != NC_NOERR)    {
            last_status = EXIT_FAILED;
            break;
        }
        status = nc_get_att_string(nc2, varid2, name, st2);
        if (status != NC_NOERR)    {
            last_status = EXIT_FAILED;
            break;
        }
        for(i = 0; i < minlen; ++i) {
            if ( (st1[i] && !st2[i]) ||
                 (!st1[i] && st2[i]) ||
                 strcmp(st1[i], st2[i]) ) {
                last_status = EXIT_DIFFER;
                break;
            }
        }
        break;
    default:
        LOG_ERROR("Cannot compare unsupported attribute type = %d\n", type);
        last_status = EXIT_FAILED;
        break;
    }

    #undef CMP_ATT_VAL

    if (st1) {
		status = nc_free_string(len1, st1);
        HANDLE_NC_ERROR(status);
    }

    if (st2) {
		status = nc_free_string(len2, st2);
        HANDLE_NC_ERROR(status);
    }

    XFREE( c1);  XFREE( c2);
    XFREE( d1);  XFREE( d2);
    XFREE( f1);  XFREE( f2);
    XFREE( i1);  XFREE( i2);
    XFREE( l1);  XFREE( l2);
    XFREE( s1);  XFREE( s2);
    XFREE(sc1);  XFREE(sc2);
    XFREE(st1);  XFREE(st2);
    XFREE(uc1);  XFREE(uc2);
    XFREE(ui1);  XFREE(ui2);
    XFREE(ul1);  XFREE(ul2);
    XFREE(us1);  XFREE(us2);

    return last_status;
}

int nccmp_cmp_metadata(nccmp_state_t *state, nccmp_strlist_t* varlist, int ncid1, int ncid2)
{
    char recname1[NC_MAX_NAME], recname2[NC_MAX_NAME];
    int status, last_status = EXIT_SUCCESS;

    strcpy(recname1, "");
    strcpy(recname2, "");

    if (state->opts.verbose) {
        LOG_INFO("Comparing metadata.\n");
    }

    status = nccmp_cmp_metadata_dim_count(state);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    nccmp_get_rec_names(& state->opts, ncid1, ncid2, state->recid1, state->recid2, recname1, recname2);
    status = nccmp_cmp_metadata_dim_lens(state, ncid1, ncid2);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    nccmp_destroy_user_types(state->types1);
    state->types1 = nccmp_load_group_user_types(ncid1, state->opts.debug);
    nccmp_destroy_user_types(state->types2);
    state->types2 = nccmp_load_group_user_types(ncid2, state->opts.debug);

    status = nccmp_cmp_metadata_user_types(& state->opts, ncid1, ncid2);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    status = nccmp_cmp_metadata_var_types(state, varlist, ncid1, ncid2);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    status = nccmp_cmp_metadata_var_dim_names(state, varlist, recname1, recname2,
                ncid1, ncid2);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    status = nccmp_cmp_metadata_var_atts(state, varlist, ncid1, ncid2);
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    if ( state->opts.header_pad ) {
        status = nccmp_header_pad(& state->opts, ncid1, ncid2);
        last_status = status ? status : last_status;
    }

recover:
    return last_status;
}

int nccmp_cmp_metadata_dim_count(nccmp_state_t *state)
{
    int last_status = EXIT_SUCCESS;

    if (state->opts.verbose) {
        LOG_INFO("Comparing number of dimensions.\n");
    }

    if (state->ndims1 != state->ndims2) {
        if (!state->opts.quiet) {
            PRINT_DIFF(state->opts.debug,
                       "DIFFER : NUMBER OF DIMENSIONS IN FILES : %d <> %d\n",
                       state->ndims1, state->ndims2);
        }
        if (!state->opts.warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
    }

    return last_status;
}

int nccmp_cmp_metadata_dim_lens(nccmp_state_t *state, int ncid1, int ncid2)
{
    int status, last_status = EXIT_SUCCESS;
    int i,  dimid1, dimid2;
    char name1[NC_MAX_NAME], name2[NC_MAX_NAME];
    size_t len1, len2;

    if (state->opts.verbose) {
        LOG_INFO("Comparing dimension lengths.\n");
    }

    for(i = 0; i < state->ndims1; ++i) {
        dimid1 = state->dims1[i].dimid;
        status = nc_inq_dim(ncid1, dimid1, name1, & len1);
        if (status != NC_NOERR) {
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            fprintf(stderr, "Failed to query dimension id %d in file %s.\n",
                    dimid1, state->opts.file1);
            if(state->opts.force) continue; else break;
        }

        status = nc_inq_dimid(ncid2, name1, & dimid2);
        if (status != NC_NOERR) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : DIMENSION : %s : DOES NOT EXIST IN \"%s\"\n",
                           name1, state->opts.file2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(state->opts.force) continue; else break;
        }

        status = nc_inq_dim(ncid2, dimid2, name2, & len2);
        if (status != NC_NOERR) {
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            fprintf(stderr, "Failed to query dimension \"%s\" in file \"%s\".\n",
                    name1, state->opts.file2);
            if(state->opts.force) continue; else break;
        }

        if (len1 != len2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : LENGTHS : DIMENSION : %s : %lu <> %lu\n", name1,
                           (unsigned long)len1, (unsigned long)len2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(state->opts.force) continue; else break;
        }
    }

    return last_status;
}

int nccmp_cmp_metadata_user_type_enum_values(nccmp_opt_t *opts, int ncid1, int ncid2,
        nc_type typeid1, nc_type typeid2, int num_members1, int num_members2,
        const char* typename, const char* file1, const char* file2,
        char left_vs_right)
{
    int status, status2, last_status = EXIT_SUCCESS;
    int memberi;
    char name1[NC_MAX_NAME], name2[NC_MAX_NAME];
    nc_type base_nc_type1;

    signed char schar_value;
    char text_value;
    int int_value;
    long long longlong_value;
    short short_value;
    unsigned char uchar_value;
    unsigned int uint_value;
    unsigned long long ulonglong_value;
    unsigned short ushort_value;

    #define GET_ENUM_VAL(V) {                                                  \
        status = nc_inq_enum_member(ncid1, typeid1, memberi, name1, & V);      \
        HANDLE_NC_ERROR(status);                                               \
        status2 = nc_inq_enum_ident(ncid2, typeid2, V, name2);                 \
        if (NC_EINVAL == status2) {                                            \
            if (!opts->quiet) {                                                \
                PRINT_DIFF(opts->debug, "DIFFER : %s ENUM VALUE %lld NOT FOUND in \"%s\"\n", \
                           typename, (long long int)V, file2);                               \
            }                                                                  \
            if (!opts->warn[NCCMP_W_ALL]) {                                    \
                last_status = EXIT_DIFFER;                                     \
            }                                                                  \
            if(!opts->force) goto recover;                                     \
        } else {                                                               \
            HANDLE_NC_ERROR(status2);                                          \
        }                                                                      \
        if (strcmp(name1, name2)) {                                            \
            if (!opts->quiet) {                                                \
                PRINT_DIFF(opts->debug, "DIFFER : %s ENUM VALUE %lld IDENTIFIERS : \"%s\" <> \"%s\"\n",\
                           typename, (long long int)V,                         \
                           left_vs_right ? name1 : name2,                      \
                           left_vs_right ? name2 : name1);                     \
            }                                                                  \
            if (!opts->warn[NCCMP_W_ALL]) {                                    \
                last_status = EXIT_DIFFER;                                     \
            }                                                                  \
            if(!opts->force) goto recover;                                     \
        }                                                                      \
    }

    if (!typeid1 || !typeid2) {
        goto recover;
    }

    for(memberi = 0; memberi < num_members1; ++memberi) {
        status = nc_inq_enum(ncid1,
                    typeid1,
                    0 /* name */,
                    & base_nc_type1,
                    0 /* base_size */,
                    0 /* num_members */);
        HANDLE_NC_ERROR(status);
        strcpy(name1, "");
        strcpy(name2, "");

        switch(base_nc_type1) {
        case NC_BYTE:   GET_ENUM_VAL(schar_value);     break;
        case NC_CHAR:   GET_ENUM_VAL(text_value);      break;
        case NC_INT:    GET_ENUM_VAL(int_value);       break;
        case NC_INT64:  GET_ENUM_VAL(longlong_value);  break;
        case NC_SHORT:  GET_ENUM_VAL(short_value);     break;
        case NC_UBYTE:  GET_ENUM_VAL(uchar_value);     break;
        case NC_UINT:   GET_ENUM_VAL(uint_value);      break;
        case NC_UINT64: GET_ENUM_VAL(ulonglong_value); break;
        case NC_USHORT: GET_ENUM_VAL(ushort_value);    break;
        default:
            /* Do not support float, double, string. */
            LOG_ERROR("Unsupported enum base type = %d\n", base_nc_type1);
            last_status = EXIT_FAILED;
            break;
        }
    }

#undef GET_ENUM_VAL
recover:

    return last_status;
}

int nccmp_cmp_metadata_user_type_enum(nccmp_opt_t *opts, int ncid1, int ncid2,
        int ntypes1, int ntypes2, nc_type* typeids1, nc_type* typeids2)
{
    int status, last_status = EXIT_SUCCESS;
    int i;
    size_t base_size1, base_size2, num_members1, num_members2;
    nc_type typeid1, typeid2, base_nc_type1, base_nc_type2;
    char tmpstr1[NC_MAX_NAME], tmpstr2[NC_MAX_NAME];
    nccmp_strlist_t* enum_type_names;
    char *name;

    enum_type_names = nccmp_get_user_type_names(ncid1, ncid2,
                        ntypes1, ntypes2, typeids1, typeids2, NC_ENUM);

    for(i = 0; i < enum_type_names->size; ++i) {
        name = enum_type_names->items[i];
        /* Find id by name. */
        status = nc_inq_typeid(ncid1, name, & typeid1);
        if (status) {
            if (NC_EBADTYPE == status) {
                if (!opts->quiet) {
                    PRINT_DIFF(opts->debug, "DIFFER : ENUM TYPE %s DOES NOT EXIST IN \"%s\"\n",
                               name, opts->file1);
                }
                if (!opts->warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(!opts->force) goto recover;
            } else {
                HANDLE_NC_ERROR(status);
            }
            typeid1 = base_nc_type1 = base_size1 = num_members1 = 0;
        } else {
            status = nc_inq_enum(ncid1,
                        typeid1,
                        0 /* name */,
                        & base_nc_type1,
                        & base_size1,
                        & num_members1);
            HANDLE_NC_ERROR(status);
        }

        status = nc_inq_typeid(ncid2, name, & typeid2);
        if (status) {
            if (NC_EBADTYPE == status) {
                if (!opts->quiet) {
                    PRINT_DIFF(opts->debug, "DIFFER : ENUM TYPE %s DOES NOT EXIST IN \"%s\"\n",
                               name, opts->file2);
                }
                if (!opts->warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(!opts->force) goto recover;
            } else {
                HANDLE_NC_ERROR(status);
            }
            typeid2 = base_nc_type2 = base_size2 = num_members2 = 0;
        } else {
            status = nc_inq_enum(ncid2,
                        typeid2,
                        0 /* name */,
                        & base_nc_type2,
                        & base_size2,
                        & num_members2);
            HANDLE_NC_ERROR(status);
        }

        if (base_nc_type1 != base_nc_type2) {
            if (!opts->quiet) {
                nccmp_nc_type_name_to_str(base_nc_type1, tmpstr1, ncid1, opts->debug);
                nccmp_nc_type_name_to_str(base_nc_type2, tmpstr2, ncid2, opts->debug);
                PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : ENUM BASETYPE : %s <> %s\n",
                                        name, tmpstr1, tmpstr2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!opts->force) goto recover;
        }

        if (base_size1 != base_size2) {
            if (!opts->quiet) {
                PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : SIZE : %zu <> %zu\n",
                           name, base_size1, base_size2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!opts->force) goto recover;
        }

        if (num_members1 != num_members2) {
            if (!opts->quiet) {
                PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : NUMBER OF ENUM MEMBERS : %zu <> %zu\n",
                           name, num_members1, num_members2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!opts->force) goto recover;
        }

        /* Compare enum strings and values bi-directionally to handle mapping diffs. */
        status = nccmp_cmp_metadata_user_type_enum_values(opts,
                    ncid1, ncid2,
                    typeid1, typeid2,
                    num_members1, num_members2, name,
                    opts->file1, opts->file2, 1);
        last_status = status ? status : last_status;
        if(last_status && !opts->force) goto recover;

        status = nccmp_cmp_metadata_user_type_enum_values(opts,
                    ncid2, ncid1,
                    typeid2, typeid1,
                    num_members2, num_members1, name,
                    opts->file2, opts->file1, 0);
        last_status = status ? status : last_status;
        if(last_status && !opts->force) goto recover;
    }

recover:
    nccmp_free_strlist(& enum_type_names);

    return last_status;
}

int nccmp_cmp_metadata_user_type_opaque(nccmp_opt_t *opts, int ncid1, int ncid2,
        int ntypes1, int ntypes2, nc_type* typeids1, nc_type* typeids2)
{
    int status, last_status = EXIT_SUCCESS;
    int i;
    size_t base_size1, base_size2;
    nc_type typeid1, typeid2;
    nccmp_strlist_t* type_names;
    char *name;

    type_names = nccmp_get_user_type_names(ncid1, ncid2,
                        ntypes1, ntypes2, typeids1, typeids2, NC_OPAQUE);

    for(i = 0; i < type_names->size; ++i) {
        name = type_names->items[i];
        /* Find id by name. */
        status = nc_inq_typeid(ncid1, name, & typeid1);
        if (status) {
            if (NC_EBADTYPE == status) {
                if (!opts->quiet) {
                    PRINT_DIFF(opts->debug, "DIFFER : OPAQUE TYPE %s DOES NOT EXIST IN \"%s\"\n",
                               name, opts->file1);
                }
                if (!opts->warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(!opts->force) goto recover;
            } else {
                HANDLE_NC_ERROR(status);
            }
            typeid1 = base_size1 = 0;
        } else {
            status = nc_inq_opaque(ncid1,
                        typeid1,
                        0 /* name */,
                        & base_size1);
            HANDLE_NC_ERROR(status);
        }

        status = nc_inq_typeid(ncid2, name, & typeid2);
        if (status) {
            if (NC_EBADTYPE == status) {
                if (!opts->quiet) {
                    PRINT_DIFF(opts->debug, "DIFFER : OPAQUE TYPE %s DOES NOT EXIST IN \"%s\"\n",
                               name, opts->file2);
                }
                if (!opts->warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(!opts->force) goto recover;
            } else {
                HANDLE_NC_ERROR(status);
            }
            typeid2 = base_size2 = 0;
        } else {
            status = nc_inq_opaque(ncid2,
                        typeid2,
                        0 /* name */,
                        & base_size2);
            HANDLE_NC_ERROR(status);
        }

        if (base_size1 != base_size2) {
            if (!opts->quiet) {
                PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : SIZE : %zu <> %zu\n",
                           name, base_size1, base_size2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!opts->force) goto recover;
        }
    }

recover:
    nccmp_free_strlist(& type_names);

    return last_status;
}

int nccmp_cmp_metadata_user_type_vlen(nccmp_opt_t *opts, int ncid1, int ncid2,
        int ntypes1, int ntypes2, nc_type* typeids1, nc_type* typeids2)
{
    int status, last_status = EXIT_SUCCESS;
    int i;
    nc_type base_type1, base_type2, typeid1, typeid2;
    char tmpstr1[NC_MAX_NAME], tmpstr2[NC_MAX_NAME];
    nccmp_strlist_t* type_names;
    char *name;

    type_names = nccmp_get_user_type_names(ncid1, ncid2,
                        ntypes1, ntypes2, typeids1, typeids2, NC_VLEN);

    for(i = 0; i < type_names->size; ++i) {
        name = type_names->items[i];
        /* Find id by name. */
        status = nc_inq_typeid(ncid1, name, & typeid1);
        if (status) {
            if (NC_EBADTYPE == status) {
                if (!opts->quiet) {
                    PRINT_DIFF(opts->debug, "DIFFER : VLEN TYPE %s DOES NOT EXIST IN \"%s\"\n",
                               name, opts->file1);
                }
                if (!opts->warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(!opts->force) goto recover;
            } else {
                HANDLE_NC_ERROR(status);
            }
            typeid1 = base_type1 = 0;
        } else {
            status = nc_inq_vlen(ncid1,
                        typeid1,
                        0 /* name */,
                        0 /* base_size */,
                        & base_type1);
            HANDLE_NC_ERROR(status);
        }

        status = nc_inq_typeid(ncid2, name, & typeid2);
        if (status) {
            if (NC_EBADTYPE == status) {
                if (!opts->quiet) {
                    PRINT_DIFF(opts->debug, "DIFFER : VLEN TYPE %s DOES NOT EXIST IN \"%s\"\n",
                               name, opts->file2);
                }
                if (!opts->warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(!opts->force) goto recover;
            } else {
                HANDLE_NC_ERROR(status);
            }
            typeid1 = base_type2 = 0;
        } else {
            status = nc_inq_vlen(ncid2,
                        typeid2,
                        0 /* name */,
                        0 /* base_size */,
                        & base_type2);
            HANDLE_NC_ERROR(status);
        }

        if (base_type1 != base_type2) {
            if (!opts->quiet) {
                nccmp_nc_type_name_to_str(base_type1, tmpstr1, ncid1, opts->debug);
                nccmp_nc_type_name_to_str(base_type2, tmpstr2, ncid2, opts->debug);
                PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : BASE TYPE : %s <> %s\n",
                           name, tmpstr1, tmpstr2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!opts->force) goto recover;
        }
    }

recover:
    nccmp_free_strlist(& type_names);

    return last_status;
}

int nccmp_cmp_metadata_user_type_compound_field(nccmp_opt_t *opts, int ncid1, int ncid2,
        const char* name, nc_type typeid1, nc_type typeid2, const char* fieldname)
{
    int status, last_status = EXIT_SUCCESS;
    int fieldid1 = -1, fieldid2 = -1;
    size_t offset1 = 9999, offset2 = 8888;
    nc_type type1 = 0, type2 = 0;
    char str1[NC_MAX_NAME], str2[NC_MAX_NAME];

    if (!typeid1 || !typeid2) {
        return last_status;
    }

    status = nc_inq_compound_fieldindex(ncid1, typeid1, fieldname, & fieldid1);
    if (NC_EBADFIELD == status) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : FIELD : %s : DOES NOT EXIST IN \"%s\"\n",
                       name, fieldname, opts->file1);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        if(!opts->force) goto recover;
    } else {
        HANDLE_NC_ERROR(status);
    }

    status = nc_inq_compound_fieldindex(ncid2, typeid2, fieldname, & fieldid2);
    if (NC_EBADFIELD == status) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : FIELD : %s : DOES NOT EXIST IN \"%s\"\n",
                       name, fieldname, opts->file2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        if(!opts->force) goto recover;
    } else {
        HANDLE_NC_ERROR(status);
    }

    if (-1 < fieldid1) {
        status = nc_inq_compound_fieldoffset(ncid1, typeid1, fieldid1, & offset1);
        HANDLE_NC_ERROR(status);
    }

    if (-1 < fieldid2) {
        status = nc_inq_compound_fieldoffset(ncid2, typeid2, fieldid2, & offset2);
        HANDLE_NC_ERROR(status);
    }

    if (offset1 != offset2 && (-1 < fieldid1) && (-1 < fieldid2)) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : FIELD : %s : OFFSET : %zu <> %zu\n",
                       name, fieldname, offset1, offset2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        if(!opts->force) goto recover;
    }

    if (-1 < fieldid1) {
        status = nc_inq_compound_fieldtype(ncid1, typeid1, fieldid1, & type1);
        HANDLE_NC_ERROR(status);
    }

    if (-1 < fieldid2) {
        status = nc_inq_compound_fieldtype(ncid2, typeid2, fieldid2, & type2);
        HANDLE_NC_ERROR(status);
    }

    if (type1 != type2 && (-1 < fieldid1) && (-1 < fieldid2)) {
        if (!opts->quiet) {
            nccmp_nc_type_name_to_str(type1, str1, ncid1, opts->debug);
            nccmp_nc_type_name_to_str(type2, str2, ncid2, opts->debug);
            PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : FIELD : %s : TYPE : %s <> %s\n",
                       name, fieldname, str1, str2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        if(!opts->force) goto recover;
    }

recover:

    return last_status;
}

int nccmp_cmp_metadata_user_type_compound_fields(nccmp_opt_t *opts, int ncid1, int ncid2,
        const char* name, const char* file1, const char* file2)
{
    int status, last_status = EXIT_SUCCESS;
    nc_type typeid1 = 0, typeid2 = 0;
    size_t base_size1, base_size2, num_fields1, num_fields2;
    nccmp_strlist_t *field_names = 0;
    int i;

    status = nc_inq_typeid(ncid1, name, & typeid1);
    if (status) {
        if (NC_EBADTYPE == status) {
            if (!opts->quiet) {
                PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : DOES NOT EXIST IN \"%s\"\n",
                           name, opts->file1);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!opts->force) goto recover;
        } else {
            HANDLE_NC_ERROR(status);
        }
        typeid1 = num_fields1 = base_size1 = 0;
    } else {
        status = nc_inq_compound(ncid1,
                    typeid1,
                    0 /* name */,
                    & base_size1,
                    & num_fields1);
        HANDLE_NC_ERROR(status);
    }

    status = nc_inq_typeid(ncid2, name, & typeid2);
    if (status) {
        if (NC_EBADTYPE == status) {
            if (!opts->quiet) {
                PRINT_DIFF(opts->debug, "DIFFER : : TYPE : %s : DOES NOT EXIST IN \"%s\"\n",
                           name, opts->file2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!opts->force) goto recover;
        } else {
            HANDLE_NC_ERROR(status);
        }
        typeid2 = num_fields2 = base_size2 = 0;
    } else {
        status = nc_inq_compound(ncid2,
                    typeid2,
                    0 /* name */,
                    & base_size2,
                    & num_fields2);
        HANDLE_NC_ERROR(status);
    }

    if (base_size1 != base_size2 && typeid1 && typeid2) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : SIZE : %zu <> %zu\n",
                       name, base_size1, base_size2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        if(!opts->force) goto recover;
    }

    if (num_fields1 != num_fields2 && typeid1 && typeid2) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : TYPE : %s : NUMBER OF FIELDS : %zu <> %zu\n",
                       name, num_fields1, num_fields2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        if(!opts->force) goto recover;
    }

    field_names = nccmp_get_user_type_compound_field_names(ncid1, ncid2, name, opts->debug);

    for(i = 0; i < field_names->size; ++i) {
        status = nccmp_cmp_metadata_user_type_compound_field(opts, ncid1, ncid2,
                    name, typeid1, typeid2, field_names->items[i]);
        last_status = status ? status : last_status;
        if(last_status && !opts->force) goto recover;
    }

recover:
    nccmp_free_strlist(& field_names);

    return last_status;
}

int nccmp_cmp_metadata_user_type_compound(nccmp_opt_t *opts, int ncid1, int ncid2,
        int ntypes1, int ntypes2, nc_type* typeids1, nc_type* typeids2)
{
    int status, last_status = EXIT_SUCCESS;
    nccmp_strlist_t* type_names;
    int i;
    char *name;

    type_names = nccmp_get_user_type_names(ncid1, ncid2,
                        ntypes1, ntypes2, typeids1, typeids2, NC_COMPOUND);

    for(i = 0; i < type_names->size; ++i) {
        name = type_names->items[i];
        status = nccmp_cmp_metadata_user_type_compound_fields(opts,
                    ncid1, ncid2, name, opts->file1, opts->file2);
        last_status = status ? status : last_status;
        if(last_status && !opts->force) goto recover;
    }

recover:
    nccmp_free_strlist(& type_names);

    return last_status;
}

int nccmp_cmp_metadata_user_types(nccmp_opt_t *opts, int ncid1, int ncid2)
{
    int status, last_status = EXIT_SUCCESS;
    int ntypes1, ntypes2;
    nc_type typeids1[NC_MAX_NAME], typeids2[NC_MAX_NAME];

    if (opts->verbose) {
        LOG_INFO("Comparing user defined types.\n");
    }

    status = nc_inq_typeids(ncid1, & ntypes1, typeids1);
    HANDLE_NC_ERROR(status);
    status = nc_inq_typeids(ncid2, & ntypes2, typeids2);
    HANDLE_NC_ERROR(status);

    if (ntypes1 != ntypes2) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : NUMBER OF USER DEFINED TYPES : %d <> %d\n",
                       ntypes1, ntypes2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            last_status = EXIT_DIFFER;
        }
        if(!opts->force) goto recover;
    }

    status = nccmp_cmp_metadata_user_type_enum(opts, ncid1, ncid2, ntypes1, ntypes2,
                typeids1, typeids2);
    last_status = status ? status : last_status;
    if (last_status && !opts->force) {
        goto recover;
    }

    status = nccmp_cmp_metadata_user_type_opaque(opts, ncid1, ncid2, ntypes1, ntypes2,
                typeids1, typeids2);
    last_status = status ? status : last_status;
    if (last_status && !opts->force) {
        goto recover;
    }

    status = nccmp_cmp_metadata_user_type_vlen(opts, ncid1, ncid2, ntypes1, ntypes2,
                typeids1, typeids2);
    last_status = status ? status : last_status;
    if (last_status && !opts->force) {
        goto recover;
    }

    status = nccmp_cmp_metadata_user_type_compound(opts, ncid1, ncid2, ntypes1, ntypes2,
                typeids1, typeids2);
    last_status = status ? status : last_status;
    if (last_status && !opts->force) {
        goto recover;
    }

recover:
    return last_status;
}

int nccmp_cmp_metadata_var_types(nccmp_state_t *state, nccmp_strlist_t* varlist, int ncid1, int ncid2)
{
    int last_status = EXIT_SUCCESS;
    int i;
    char typestr1[NC_MAX_NAME], typestr2[NC_MAX_NAME];
    nccmp_user_type_t *type1 = 0, *type2 = 0;
    const nccmp_var_t *var1, *var2;
    char is_diff_types;
    
    if (state->opts.verbose) {
        LOG_INFO("Comparing variable datatypes and rank.\n");
    }

    for(i=0; i < varlist->size; ++i) {
        is_diff_types = 0;
        var1 = nccmp_find_var_by_name(state->vars1, state->nvars1, varlist->items[i]);
        if (!var1) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : DOES NOT EXIST IN \"%s\"\n",
                           varlist->items[i], state->opts.file1);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(state->opts.force) continue; else break;
        }

        var2 = nccmp_find_var_by_name(state->vars2, state->nvars2, varlist->items[i]);
        if (!var2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : DOES NOT EXIST IN \"%s\"\n",
                           varlist->items[i], state->opts.file2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(state->opts.force) continue; else break;
        }

        if (!var1 || !var2) {
            continue;
        }
        
        /* Compare numeric id, but also check names if user-types. */
        if(var1->type != var2->type) {
            typestr1[0] = 0;
            typestr2[0] = 0;
            if (var1->type < NC_FIRSTUSERTYPEID) {
                nccmp_nc_type_name_to_str(var1->type, typestr1, ncid1, state->opts.debug);
                nccmp_nc_type_name_to_str(var2->type, typestr2, ncid2, state->opts.debug);    
                is_diff_types = 1;          
            } else {
                type1 = nccmp_get_user_type_by_id(state->types1->items, state->types1->num_items, var1->type);
                type2 = nccmp_get_user_type_by_id(state->types2->items, state->types2->num_items, var2->type);
                if (type1) {
                    strcpy(typestr1, type1->name);
                }
                if (type2) {
                    strcpy(typestr2, type2->name);
                }
                is_diff_types = (strcmp(typestr1, typestr2) != 0);
            }
            
            if (is_diff_types) {
                if (!state->opts.quiet) {
                    PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : TYPE : %s <> %s\n",
                               varlist->items[i], typestr1, typestr2);
                }
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(state->opts.force) continue; else break;            
            }
        }

        if(var1->ndims != var2->ndims) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : NUMBER : DIMENSIONS : VARIABLE : %s : %d <> %d\n",
                           varlist->items[i], var1->ndims, var2->ndims);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(state->opts.force) continue; else break;
        }
    }

    return last_status;
}

int nccmp_cmp_metadata_var_dim_names(const nccmp_state_t *state, nccmp_strlist_t* varlist,
        const char* recname1, const char* recname2, int ncid1, int ncid2)
{
    int last_status = EXIT_SUCCESS;
    int i, dim, dimid1, dimid2, cmp1, cmp2;
    nccmp_dim_t *dim1, *dim2;
    const nccmp_var_t *var1, *var2;
    
    if (state->opts.verbose) {
        LOG_INFO("Comparing variable dimension names.\n");
    }
    
    for(i=0; i < varlist->size; ++i) {
        var1 = nccmp_find_var_by_name(state->vars1, state->nvars1, varlist->items[i]);
        var2 = nccmp_find_var_by_name(state->vars2, state->nvars2, varlist->items[i]);
        if (!var1 || !var2) {
            continue;
        }
        
        /* dimensions */
        for(dim = 0; dim < var1->ndims; ++dim) {
            dimid1 = nccmp_get_var_dimid(var1, dim);
            dimid2 = nccmp_get_var_dimid(var2, dim);
            if ((-1 == dimid1) || (-1 == dimid2)) {
                break;
            }

            dim1 = nccmp_get_dim_by_id(state->dims1, state->ndims1, dimid1);
            dim2 = nccmp_get_dim_by_id(state->dims2, state->ndims2, dimid2);
            if (!dim1 || !dim2) {
                continue;
            }

            if (strcmp(dim1->name, dim2->name) != 0) {
                if (!state->opts.quiet) {
                    PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : DIMENSION : %s <> %s\n",
                               varlist->items[i], dim1->name, dim2->name);
                }
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(state->opts.force) continue; else goto recover;
            }

            cmp1 = strcmp(dim1->name, recname1);
            cmp2 = strcmp(dim2->name, recname2);

            if (!cmp1 && cmp2) {
                if (!state->opts.quiet) {
                    PRINT_DIFF(state->opts.debug,
                               "DIFFER : VARIABLE : %s : DIMENSION %s IS RECORD IN FILE \"%s\" BUT NOT IN \"%s\"\n",
                               var1->name, dim1->name, state->opts.file1, state->opts.file2);
                }
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(state->opts.force) continue; else goto recover;
            } else if (cmp1 && !cmp2) {
                if (!state->opts.quiet) {
                    PRINT_DIFF(state->opts.debug,
                               "DIFFER : VARIABLE : %s : DIMENSION %s IS RECORD IN FILE \"%s\" BUT NOT IN \"%s\"\n",
                               var1->name, dim2->name, state->opts.file2, state->opts.file1);
                }
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(state->opts.force) continue; else goto recover;
            }
        }
    }

recover:
    return last_status;
}

int nccmp_cmp_metadata_var_atts(nccmp_state_t *state, nccmp_strlist_t* varlist, int ncid1, int ncid2)
{
    int status, last_status = EXIT_SUCCESS;
    nccmp_strlist_t* processed_atts = NULL;
    int natts1, natts2, i, attid1, attid2;
    char name1[NC_MAX_NAME], name2[NC_MAX_NAME];
    const nccmp_var_t *var1, *var2;

    if (state->opts.verbose) {
        LOG_INFO("Comparing variables attributes.\n");
    }

    processed_atts = nccmp_new_strlist(NC_MAX_VARS);
    if (!processed_atts) {
        LOG_ERROR("Failed to allocate string list for comparing attributes.\n");
        return EXIT_FATAL;
    }

    for(i=0; i < varlist->size; ++i) {
        var1 = nccmp_find_var_by_name(state->vars1, state->nvars1, varlist->items[i]);
        var2 = nccmp_find_var_by_name(state->vars2, state->nvars2, varlist->items[i]);
        if (!var1 || !var2) {
            continue;
        }        
        
        natts1 = natts2 = 0;
        nccmp_clear_strlist(processed_atts);

        /* Attributes */
        for(attid1=0; attid1 < var1->natts; ++attid1) {
            status = nc_inq_attname(ncid1, var1->varid, attid1, name1);
            if (status != NC_NOERR) {
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(state->opts.force) continue; else goto recover;
            }
            if (nccmp_exists_in_strlist(state->opts.excludeattlist, name1) ||
                    nccmp_exists_in_strlist(processed_atts, name1)) {
                continue;
            }
            /* Log that this att was processed. */
            nccmp_add_to_strlist(processed_atts, name1);
            ++natts1;
            status = nccmp_cmp_att(ncid1, ncid2, var1->varid,
                        var2->varid, name1,
                        var1->name, & state->opts);
            if (status == EXIT_DIFFER) {
                last_status = status;
                if(state->opts.force) continue; else goto recover;
            }
        }

        for(attid2=0; attid2 < var2->natts; ++attid2) {
            status = nc_inq_attname(ncid2, var2->varid, attid2, name2);
            if (status != NC_NOERR) {
                fprintf(stderr, "Failed to query variable %s attribute in file \"%s\"\n",
                        var2->name, state->opts.file2);

                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(state->opts.force) continue; else goto recover;
            }

            if (nccmp_exists_in_strlist(state->opts.excludeattlist, name2)) {
                continue;
            }
            /* Count non-excluded attribute. */
            ++natts2;
            if (nccmp_exists_in_strlist(processed_atts, name2)) {
                continue;
            }
            /* Log that this att was processed. */
            nccmp_add_to_strlist(processed_atts, name2);

            /* Do comparison. */
            status = nccmp_cmp_att(ncid1, ncid2, var1->varid,
                    var2->varid, name2, var2->name,
                    & state->opts);
            if (status == EXIT_DIFFER) {
                last_status = status;
                if(state->opts.force) continue; else goto recover;
            }
        }

        if(natts1 != natts2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug,
                           "DIFFER : NUMBER OF ATTRIBUTES : VARIABLE : %s : %d <> %d\n",
                           varlist->items[i], natts1, natts2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(state->opts.force) continue; else goto recover;
        }
    }

recover:
    nccmp_free_strlist(& processed_atts);

    return last_status;
}

int nccmp_dump_info_path(nccmp_state_t* state, int ncid)
{
    int status = EXIT_SUCCESS;
    char resolved_path[NC_MAX_VARS];
    char *pathptr = NULL;

    #if HAVE_NC_INQ_PATH
        size_t pathlenp = 0;
        char path[NC_MAX_VARS];
        path[0] = 0;
        status = nc_inq_path(ncid, & pathlenp, path);
        HANDLE_NC_ERROR(status);
        pathptr = path;
    #endif

    if (!pathptr) {
        if (state->nthread_groups1 &&
            state->thread_groups1 &&
            state->thread_groups1[0].id == ncid) {
            pathptr = state->opts.file1;
        } else if (state->nthread_groups2 &&
                state->thread_groups2 &&
                state->thread_groups2[0].id == ncid) {
            pathptr = state->opts.file2;
        } else {
            status = EXIT_FAILURE;
        }
    }

    printf("path=");
    if (pathptr)
    {
        resolved_path[0] = 0;
        pathptr = realpath(pathptr, resolved_path);
        if (pathptr) {
            printf("%s", resolved_path);
        }
    }
    printf("\n");

    return status;
}

int nccmp_dump_info_format(int ncid)
{
    int status, format;

    status = nc_inq_format(ncid, & format);
    HANDLE_NC_ERROR(status);

    printf("format=");

    switch (format)
    {
    case NC_FORMAT_CLASSIC:
        printf("NC_FORMAT_CLASSIC");
        break;
    case NC_FORMAT_64BIT:
        printf("NC_FORMAT_64BIT");
        break;
    case NC_FORMAT_NETCDF4:
        printf("NC_FORMAT_NETCDF4");
        break;
    case NC_FORMAT_NETCDF4_CLASSIC:
        printf("NC_FORMAT_NETCDF4_CLASSIC");
        break;
    default:
        printf("UNKNOWN");
        break;
    }
    printf("\n");

    return status;
}

int nccmp_dump_info_headerpad(int ncid)
{
    int status = EXIT_SUCCESS;

#if HAVE_NC_H
    NC* ncp;
    size_t pad_size = 0;
    size_t header_size = 0;

    status = NC_check_id(ncid, & ncp);
    HANDLE_NC_ERROR(status);

    if (NC3_DATA(ncp))
    {
        header_size = NC3_DATA(ncp)->xsz;
        pad_size = NC3_DATA(ncp)->begin_var - NC3_DATA(ncp)->xsz;
    }

    printf("header_size=%lld bytes\n", (long long int) header_size);
    printf("header_pad_size=%lld bytes\n", (long long int) pad_size);
#endif

    return status;

}

int nccmp_dump_info_vars(int ncid)
{
    char name[NC_MAX_NAME];
    int status, format, storage, i, nvars, ndims, shuffle, deflate, deflate_level, endian, checksum;
    int options_mask, pixels_per_block;
    size_t chunksizes[NC_MAX_DIMS];
    char chunksizes_str[NC_MAX_NAME];
    char checksum_str[NC_MAX_NAME];
    char storage_str[NC_MAX_NAME];
    char endian_str[NC_MAX_NAME];
    int is_nc4;
    const int UNKNOWN = -999;

    status = nc_inq_format(ncid, & format);
    HANDLE_NC_ERROR(status);

    is_nc4 = (NC_FORMAT_NETCDF4 == format);

    status = nc_inq_nvars(ncid, &nvars);
    HANDLE_NC_ERROR(status);

    for(i=0; i < nvars; ++i)
    {
        status = nc_inq_varname(ncid, i, name);
        HANDLE_NC_ERROR(status);

        status = nc_inq_varndims(ncid, i, & ndims);
        HANDLE_NC_ERROR(status);

        status = nc_inq_var_chunking(ncid, i, & storage, chunksizes);
        HANDLE_NC_ERROR(status);

        status = nc_inq_var_deflate(ncid, i, & shuffle, & deflate, & deflate_level);
        HANDLE_NC_ERROR(status);

        if (is_nc4) {
            status = nc_inq_var_endian(ncid, i, & endian);
            HANDLE_NC_ERROR(status);
        } else {
            endian = UNKNOWN;
        }

        status = nc_inq_var_fletcher32(ncid, i, & checksum);
        HANDLE_NC_ERROR(status);

        if (is_nc4) {
            status = nc_inq_var_szip(ncid, i, & options_mask, & pixels_per_block);
            HANDLE_NC_ERROR(status);
        } else {
            options_mask = 0;
            pixels_per_block = 0;
        }

        nccmp_size_t_array_to_str(chunksizes, ndims, chunksizes_str, ",");
        nccmp_checksum_to_str(checksum, checksum_str);
        nccmp_chunk_storage_to_str(storage, storage_str);
        nccmp_endian_to_str(endian, endian_str);
        printf("var=%s checksum=%s chunk=%s chunksizes=[%s] deflate=%d deflate_level=%d endian=%s shuffle=%d szip_mask=%d szip_pixels_per_block=%d\n",
                name,
                checksum_str,
                storage_str,
                chunksizes_str,
                deflate,
                deflate_level,
                endian_str,
                shuffle,
                options_mask,
                pixels_per_block
                );
    }

    return status;
}

int nccmp_dump_info_all(nccmp_state_t* state, int ncid)
{
    int status = EXIT_SUCCESS;

    if (ncid)
    {
        status = nccmp_dump_info_path(state, ncid);
        if (status) return status;

        status = nccmp_dump_info_format(ncid);
        if (status) return status;

        status = nccmp_dump_info_headerpad(ncid);
        if (status) return status;

        status = nccmp_dump_info_vars(ncid);
        if (status) return status;
    }

    return status;
}

int nccmp_dump_info(nccmp_state_t* state, int ncid)
{
    int status = EXIT_SUCCESS;

    status = nccmp_dump_info_all(state, ncid);

    return status;
}

void nccmp_get_rec_names(nccmp_opt_t *opts, int ncid1, int ncid2,
        int recid1, int recid2, char* recname1, char* recname2)
{
    int status;

    if (opts->verbose) {
        LOG_INFO("Getting record dimension names, if they exist.\n");
    }

    if (recid1 != -1) {
        status = nc_inq_dimname(ncid1, recid1, recname1);
        HANDLE_NC_ERROR(status);
    } else {
        strcpy(recname1, "");
    }

    if (recid2 != -1) {
        status = nc_inq_dimname(ncid2, recid2, recname2);
        HANDLE_NC_ERROR(status);
    } else {
        strcpy(recname1, "");
    }
}

int nccmp_file_formats(nccmp_opt_t* opts, int ncid1, int ncid2)
{
    int status, fmt1, fmt2;

    status = nc_inq_format(ncid1, & fmt1);
    HANDLE_NC_ERROR(status);

    status = nc_inq_format(ncid2, & fmt2);
    HANDLE_NC_ERROR(status);

    if (fmt1 != fmt2) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : FILE FORMATS : %s <> %s\n", NCFORMATSTR(fmt1), NCFORMATSTR(fmt2));
        }
        if (!opts->warn[NCCMP_W_ALL] && !opts->warn[NCCMP_W_FORMAT]) {
            return EXIT_DIFFER;
        }
    }

    return EXIT_SUCCESS;
}

int nccmp_global_atts(nccmp_opt_t* opts, int ncid1, int ncid2)
{
    int ngatts1, ngatts2, nattsex1, nattsex2, i, status, status2, attid1, attid2;
    nc_type type1, type2;
    size_t len1, len2;
    char name1[NC_MAX_NAME], name2[NC_MAX_NAME];
    nccmp_strlist_t* processedatts = NULL;
    static const int MAX_STRLEN = 8192;
    char typestr1[MAX_STRLEN], typestr2[MAX_STRLEN];

    status = status2 = EXIT_SUCCESS;
    if (!opts->global) {
        return status;
    }

    if (opts->verbose) {
        LOG_INFO("Comparing global attributes.\n");
    }

    if (opts->history == 0) {
        nccmp_add_to_strlist(opts->globalexclude, "history");
    }

    /* Number of global atts to compare with exclusion taken into account. */
    nattsex1 = 0;
    nattsex2 = 0;

    status = nc_inq_natts(ncid1, &ngatts1);
    HANDLE_NC_ERROR(status);

    status = nc_inq_natts(ncid2, &ngatts2);
    HANDLE_NC_ERROR(status);

    for(i = 0; i < ngatts1; ++i) {
        attid1 = i;
        status = nc_inq_attname(ncid1, NC_GLOBAL, attid1, name1);
        HANDLE_NC_ERROR(status);

        if (!nccmp_exists_in_strlist(opts->globalexclude, name1)) {
            ++nattsex1;
        }
    }

    for(i = 0; i < ngatts2; ++i) {
        attid2 = i;
        status = nc_inq_attname(ncid2, NC_GLOBAL, attid2, name2);
        HANDLE_NC_ERROR(status);

        if (!nccmp_exists_in_strlist(opts->globalexclude, name2)) {
            ++nattsex2;
        }
    }

    if(nattsex1 != nattsex2) {
        if (!opts->quiet) {
            PRINT_DIFF(opts->debug, "DIFFER : NUMBER OF GLOBAL ATTRIBUTES : %d <> %d\n", nattsex1, nattsex2);
        }
        if (!opts->warn[NCCMP_W_ALL]) {
            status2 = EXIT_DIFFER;
        }

        if(!opts->force) return status2;
    }

    processedatts = nccmp_new_strlist(NC_MAX_VARS);
    if(!processedatts) {
        LOG_ERROR("Failed to allocated string list for comparing  global attributes.\n");
        status2 = EXIT_FATAL;
        goto recover;
    }

    for(i = 0; i < ngatts1; ++i) {
        attid1 = i;
        status = nc_inq_attname(ncid1, NC_GLOBAL, attid1, name1);
        HANDLE_NC_ERROR(status);

        nccmp_add_to_strlist(processedatts, name1);

        if (nccmp_exists_in_strlist(opts->globalexclude, name1))
            continue;

        status = nc_inq_att(ncid1, NC_GLOBAL, name1, &type1, &len1);
        if (status != NC_NOERR) {
            LOG_ERROR("Query failed on global attribute in %s\n", opts->file1);
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;;
        }

        status = nc_inq_att(ncid2, NC_GLOBAL, name1, &type2, &len2);
        if (status != NC_NOERR) {
            if (!opts->quiet) {
                PRINT_DIFF(opts->debug, "DIFFER : NAME OF GLOBAL ATTRIBUTE : %s : GLOBAL ATTRIBUTE DOESN'T EXIST IN \"%s\"\n", name1, opts->file2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;;
        }

        if (type1 != type2)  {
            if (!opts->quiet) {
                nccmp_nc_type_name_to_str(type1, typestr1, ncid1, opts->debug);
                nccmp_nc_type_name_to_str(type2, typestr2, ncid2, opts->debug);
                PRINT_DIFF(opts->debug, "DIFFER : GLOBAL ATTRIBUTE TYPES : %s : %s <> %s\n", name1, typestr1, typestr2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;
        }

        if (len1 != len2)   {
            if (!opts->quiet) {
                nccmp_pretty_print_att(ncid1, NULL, NC_GLOBAL, name1, typestr1, MAX_STRLEN);
                nccmp_pretty_print_att(ncid2, NULL, NC_GLOBAL, name1, typestr2, MAX_STRLEN);
                PRINT_DIFF(opts->debug, "DIFFER : LENGTHS OF GLOBAL ATTRIBUTE : %s : %lu <> %lu : VALUES : %s <> %s\n", name1,
                           (unsigned long)len1, (unsigned long)len2, typestr1, typestr2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;
        }

        if (nccmp_cmp_att_val(ncid1,ncid2,NC_GLOBAL,NC_GLOBAL,name1,len1,len2,type1) != NC_NOERR) {
            if (!opts->quiet) {
                /* Pretty print values. */
                nccmp_pretty_print_att(ncid1, NULL, NC_GLOBAL, name1, typestr1, MAX_STRLEN);
                nccmp_pretty_print_att(ncid2, NULL, NC_GLOBAL, name1, typestr2, MAX_STRLEN);
                PRINT_DIFF(opts->debug, "DIFFER : VALUES OF GLOBAL ATTRIBUTE : %s : %s <> %s\n", name1, typestr1, typestr2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;
        }
    }

    for(i = 0; i < ngatts2; ++i) {
        attid2 = i;
        status = nc_inq_attname(ncid2, NC_GLOBAL, attid2, name2);
        if (status != NC_NOERR) {
            LOG_ERROR("Query failed for global attribute name in %s\n", opts->file2);
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;
        }

        /* Skip if already processed (or excluded). */
        if (nccmp_exists_in_strlist(processedatts, name2)) {
            continue;
        }
        /* Log that this att was processed. */
        nccmp_add_to_strlist(processedatts, name2);

        status = nc_inq_att(ncid2, NC_GLOBAL, name2, &type2, &len2);
        if (status != NC_NOERR) {
            LOG_ERROR("Query failed on global attribute in %s\n", opts->file2);
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;
        }

        status = nc_inq_att(ncid1, NC_GLOBAL, name2, &type1, &len1);
        if (status != NC_NOERR) {
            if (!opts->quiet) {
                PRINT_DIFF(opts->debug, "DIFFER : NAME OF GLOBAL ATTRIBUTE : %s : GLOBAL ATTRIBUTE DOESN'T EXIST IN %s\n", name2, opts->file1);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;
        }

        if (type1 != type2) {
            if (!opts->quiet) {
                nccmp_nc_type_name_to_str(type1, typestr1, ncid1, opts->debug);
                nccmp_nc_type_name_to_str(type2, typestr2, ncid2, opts->debug);
                PRINT_DIFF(opts->debug, "DIFFER : GLOBAL ATTRIBUTE TYPE : %s : %s <> %s\n", name1, typestr1, typestr2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;
        }

        if (len1 != len2)  {
            if (!opts->quiet) {
                nccmp_pretty_print_att(ncid1, NULL, NC_GLOBAL, name1, typestr1, MAX_STRLEN);
                nccmp_pretty_print_att(ncid2, NULL, NC_GLOBAL, name1, typestr2, MAX_STRLEN);
                PRINT_DIFF(opts->debug, "DIFFER : LENGTHS OF GLOBAL ATTRIBUTE : %s : %lu <> %lu : VALUES : ", name1, (unsigned long)len1, (unsigned long)len2);
            }
            switch(type1) {
            case NC_CHAR:
                if (!opts->quiet) {
                    /* Quote strings. */
                    fprintf(stderr, "\"%s\" : \"%s\"\n", typestr1, typestr2);
                }
                if (strcmp(typestr1,typestr2) == 0) {
                    /* Same text, but invisible trailing nulls because lengths differ. */
                    if (opts->warn[NCCMP_W_EOS] || opts->warn[NCCMP_W_ALL]) {
                        /* Pass */
                    } else {
                        status2 = EXIT_DIFFER;
                        if(opts->force) continue; else goto recover;
                    }
                }
                break;
            default:
                if (!opts->quiet) {
                    /* No quotes. */
                    fprintf(stderr, "%s : %s\n", typestr1, typestr2);
                }
                if (!opts->warn[NCCMP_W_ALL]) {
                    status2 = EXIT_DIFFER;
                    if(opts->force) continue; else goto recover;
                }
                break;
            }
        }

        if (nccmp_cmp_att_val(ncid1,ncid2,NC_GLOBAL,NC_GLOBAL,name1,len1,len2,type1) != NC_NOERR) {
            if (!opts->quiet) {
                /* Pretty print values. */
                nccmp_pretty_print_att(ncid1, NULL, NC_GLOBAL, name1, typestr1, MAX_STRLEN);
                nccmp_pretty_print_att(ncid2, NULL, NC_GLOBAL, name1, typestr2, MAX_STRLEN);
                PRINT_DIFF(opts->debug, "DIFFER : VALUES OF GLOBAL ATTRIBUTE : %s : %s <> %s\n", name1, typestr1, typestr2);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status2 = EXIT_DIFFER;
            }
            if(opts->force) continue; else goto recover;
        }
    }

recover:
    nccmp_free_strlist(& processedatts);

    return status2;
}

int nccmp_header_pad(nccmp_opt_t* opts, int ncid1, int ncid2)
{
    int status = EXIT_SUCCESS;

#if HAVE_NC_H
    char hex1[5], hex2[5]; /* Printable bytes as hex strings like 0xAB. */
    size_t padsize1, padsize2;
    unsigned char * bytes1 = 0;
    unsigned char * bytes2 = 0;
    size_t numRead;
    FILE * pFile;
    int i = 0;
    NC * ncp1;
    NC * ncp2;

    if (opts->verbose) {
        LOG_INFO("Comparing header padding.\n");
    }

    status = NC_check_id( ncid1, & ncp1 );
    HANDLE_NC_ERROR(status);
    if (status != NC_NOERR) {
        return EXIT_FATAL;
    }

    status = NC_check_id( ncid2, & ncp2 );
    HANDLE_NC_ERROR(status);
    if (status != NC_NOERR) {
        return EXIT_FATAL;
    }

    if ( (NULL == NC3_DATA(ncp1)) && (NULL == NC3_DATA(ncp2)) ) {
        if (opts->verbose) {
            LOG_INFO("No dispatchdata for both files.\n");
        }
        return EXIT_SUCCESS;
    }

    padsize1 = NC3_DATA(ncp1) ? NC3_DATA(ncp1)->begin_var - NC3_DATA(ncp1)->xsz : 0;
    padsize2 = NC3_DATA(ncp2) ? NC3_DATA(ncp2)->begin_var - NC3_DATA(ncp2)->xsz : 0;

    /* Empty pad? */
    if ( (0 == padsize1) && (0 == padsize2) ) {
        return EXIT_SUCCESS;
    }

    if ( padsize1 != padsize2 ) {
        PRINT_DIFF(opts->debug, "DIFFER : LENGTHS : HEADERPAD : %lu <> %lu\n",
                (unsigned long) padsize1, (unsigned long) padsize2);
        if ( !opts->warn[NCCMP_W_ALL] ) {
            status = EXIT_DIFFER;
        }
        if( ! opts->force ) goto recover;
    }

    /* Compare bytes. */
    bytes1 = (unsigned char *) malloc( padsize1 );
    bytes2 = (unsigned char *) malloc( padsize2 );

    pFile = fopen ( opts->file1, "r" );
    if ( NULL == pFile ) {
        fprintf(stderr, "Fatal error initializing header pad for file %s\n", opts->file1);
        goto recover;
    }

    fseek ( pFile , NC3_DATA(ncp1)->xsz , SEEK_SET );
    numRead = fread( bytes1, 1, padsize1, pFile );
    fclose ( pFile );

    if ( numRead != padsize1 ) {
        fprintf(stderr, "Fatal error reading header pad from file %s\n", opts->file1);
        goto recover;
    }

    pFile = fopen ( opts->file2, "r" );
    if ( NULL == pFile ) {
        fprintf(stderr, "Fatal error initializing header pad for file %s\n", opts->file2);
        goto recover;
    }

    fseek ( pFile , NC3_DATA(ncp2)->xsz , SEEK_SET );
    numRead = fread( bytes2, 1, padsize2, pFile );
    fclose ( pFile );

    if ( numRead != padsize2 ) {
        fprintf(stderr, "Fatal error reading header pad from file %s\n", opts->file2);
        goto recover;
    }

    for( ; i < padsize1; ++i ) {
        if ( bytes1[i] != bytes2[i] ) {
            nccmp_uchar_to_hex( bytes1[i], hex1 );
            nccmp_uchar_to_hex( bytes2[i], hex2 );
            PRINT_DIFF(opts->debug, "DIFFER : VALUES : HEADERPAD : %s <> %s : OFFSETS : %lu, %lu\n",
                    hex1, hex2,
                    (unsigned long) NC3_DATA(ncp1)->xsz + i,
                    (unsigned long) NC3_DATA(ncp2)->xsz + i );

            if ( !opts->warn[NCCMP_W_ALL] ) {
                status = EXIT_DIFFER;
            }

            if ( !opts->force ) {
                goto recover;
            }
        }
    }

recover:

    if ( bytes1 ) {
        free( bytes1 );
    }

    if ( bytes2 ) {
        free( bytes2 );
    }
#endif

    return status;
}

int nccmp_cmp_rec_info(nccmp_state_t* state, nccmp_strlist_t* varnames, int ncid1, int ncid2)
{
    char name1[NC_MAX_NAME], name2[NC_MAX_NAME];
    int status = EXIT_SUCCESS;

    strcpy(name1, "");
    strcpy(name2, "");

    if (state->opts.verbose) {
        LOG_INFO("Comparing record information.\n");
    }

    status = nc_inq_unlimdim(ncid1, & state->recid1);
    HANDLE_NC_ERROR(status);

    if (state->recid1 != -1) {
        status = nc_inq_dim(ncid1, state->recid1, name1, & state->nrecs1);
        HANDLE_NC_ERROR(status);
    } else {
        state->nrecs1 = 0;
    }

    status = nc_inq_unlimdim(ncid2, & state->recid2);
    HANDLE_NC_ERROR(status);

    if (state->recid2 != -1) {
        status = nc_inq_dim(ncid2, state->recid2, name2, & state->nrecs2);
        HANDLE_NC_ERROR(status);
    } else {
        state->nrecs2 = 0;
    }

    if (!nccmp_exists_in_strlist(varnames, name1) && !nccmp_exists_in_strlist(varnames, name2)) {
        return EXIT_SUCCESS;
    }

    if (strcmp(name1, name2)) {
        if (!state->opts.quiet) {
            PRINT_DIFF(state->opts.debug, "DIFFER : NAMES OF RECORDS : %s <> %s\n", name1, name2);
        }
        if (!state->opts.warn[NCCMP_W_ALL])
            status = EXIT_DIFFER;

        if(!state->opts.force) return status;
    }

    if (state->nrecs1 != state->nrecs2) {
        if (!state->opts.quiet) {
            PRINT_DIFF(state->opts.debug, "DIFFER : LENGTHS OF RECORDS : %s (%d) <> %s (%d)\n",
                       name1, (int)state->nrecs1, name2, (int)state->nrecs2);
        }
        if (!state->opts.warn[NCCMP_W_ALL])
            status = EXIT_DIFFER;

        if(!state->opts.force) return status;
    }

    return status;
}

int nccmp_var_encoding(nccmp_state_t *state, nccmp_strlist_t* varlist, int ncid1, int ncid2)
{
    int i, dimi, idx1, idx2, status, last_status = EXIT_SUCCESS;
    int varid1, varid2;
    int checksum1, checksum2;
    int chunk_storage1, chunk_storage2;
    size_t chunk_sizes1[NC_MAX_DIMS], chunk_sizes2[NC_MAX_DIMS];
    const int MAX_LEN = 256;
    char tmpstr1[MAX_LEN], tmpstr2[MAX_LEN];
    int shuffle1, shuffle2, deflate1, deflate2, deflate_level1, deflate_level2;
    int endian1, endian2;
    int szip_mask1, szip_mask2, szip_pixels_per_block1, szip_pixels_per_block2;

    memset(chunk_sizes1, 0, sizeof(chunk_sizes1));
    memset(chunk_sizes2, 0, sizeof(chunk_sizes2));

    if (state->opts.verbose) {
        LOG_INFO("Comparing variable encodings.\n");
    }

    for(i=0; i < varlist->size; ++i) {
        idx1 = find_var(varlist->items[i], state->vars1, state->nvars1);
        idx2 = find_var(varlist->items[i], state->vars2, state->nvars2);

        if ((idx1 == -1) || (idx2 == -1)) {
            continue;
        }

        varid1 = state->vars1[idx1].varid;
        varid2 = state->vars2[idx2].varid;

        status = nc_inq_var_fletcher32(ncid1, varid1, & checksum1);
        HANDLE_NC_ERROR(status);
        status = nc_inq_var_fletcher32(ncid2, varid2, & checksum2);
        HANDLE_NC_ERROR(status);

        if (checksum1 != checksum2) {
            if (!state->opts.quiet) {
                nccmp_checksum_to_str(checksum1, tmpstr1);
                nccmp_checksum_to_str(checksum2, tmpstr2);
                PRINT_DIFF(state->opts.debug,
                        "DIFFER : VARIABLE : %s : CHECKSUM FILTER : %s <> %s\n",
                        state->vars1[idx1].name,
                        tmpstr1,
                        tmpstr2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!state->opts.force) {
                break;
            }
        }

        status = nc_inq_var_chunking(ncid1, varid1, & chunk_storage1, chunk_sizes1);
        HANDLE_NC_ERROR(status);
        status = nc_inq_var_chunking(ncid2, varid2, & chunk_storage2, chunk_sizes2);
        HANDLE_NC_ERROR(status);

        if (chunk_storage1 != chunk_storage2) {
            if (!state->opts.quiet) {
                nccmp_chunk_storage_to_str(chunk_storage1, tmpstr1);
                nccmp_chunk_storage_to_str(chunk_storage2, tmpstr2);
                PRINT_DIFF(state->opts.debug,
                        "DIFFER : VARIABLE : %s : CHUNK STORAGE : %s <> %s\n",
                        state->vars1[idx1].name,
                        tmpstr1,
                        tmpstr2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!state->opts.force) {
                break;
            }
        }

        for(dimi = 0; dimi < state->vars1[idx1].ndims; ++dimi) {
            if (chunk_sizes1[dimi] != chunk_sizes2[dimi]) {
                if (!state->opts.quiet) {
                    nccmp_size_t_array_to_str(chunk_sizes1, state->vars1[idx1].ndims, tmpstr1, " ");
                    nccmp_size_t_array_to_str(chunk_sizes2, state->vars1[idx1].ndims, tmpstr2, " ");
                    PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : CHUNK SIZES : %s <> %s\n",
                               state->vars1[idx1].name, tmpstr1, tmpstr2);
                }
                if (!state->opts.warn[NCCMP_W_ALL]) {
                    last_status = EXIT_DIFFER;
                }
                if(!state->opts.force) {
                    break;
                }
            }
        }

        status = nc_inq_var_deflate(ncid1, varid1, & shuffle1, & deflate1, & deflate_level1);
        HANDLE_NC_ERROR(status);
        status = nc_inq_var_deflate(ncid2, varid2, & shuffle2, & deflate2, & deflate_level2);
        HANDLE_NC_ERROR(status);

        if (shuffle1 != shuffle2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : SHUFFLE : %d <> %d\n",
                           state->vars1[idx1].name, shuffle1, shuffle2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!state->opts.force) {
                break;
            }
        }

        if (deflate1 != deflate2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : DEFLATE : %d <> %d\n",
                           state->vars1[idx1].name, deflate1, deflate2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!state->opts.force) {
                break;
            }
        }

        if (deflate_level1 != deflate_level2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : DEFLATE LEVEL : %d <> %d\n",
                           state->vars1[idx1].name, deflate_level1, deflate_level2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!state->opts.force) {
                break;
            }
        }

        status = nc_inq_var_endian(ncid1, varid1, & endian1);
        HANDLE_NC_ERROR(status);
        status = nc_inq_var_endian(ncid2, varid2, & endian2);
        HANDLE_NC_ERROR(status);

        if (endian1 != endian2) {
            if (!state->opts.quiet) {
                nccmp_endian_to_str(endian1, tmpstr1);
                nccmp_endian_to_str(endian2, tmpstr2);
                PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : ENDIAN : %s <> %s\n",
                           state->vars1[idx1].name, tmpstr1, tmpstr2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!state->opts.force) {
                break;
            }
        }

        status = nc_inq_var_szip(ncid1, varid1, & szip_mask1, & szip_pixels_per_block1);
        HANDLE_NC_ERROR(status);
        status = nc_inq_var_szip(ncid2, varid2, & szip_mask2, & szip_pixels_per_block2);
        HANDLE_NC_ERROR(status);

        if (szip_mask1 != szip_mask2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : SZIP MASK : %d <> %d\n",
                           state->vars1[idx1].name, szip_mask1, szip_mask2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!state->opts.force) {
                break;
            }
        }

        if (szip_pixels_per_block1 != szip_pixels_per_block2) {
            if (!state->opts.quiet) {
                PRINT_DIFF(state->opts.debug, "DIFFER : VARIABLE : %s : SZIP PIXELS PER BLOCK : %d <> %d\n",
                           state->vars1[idx1].name, szip_pixels_per_block1, szip_pixels_per_block2);
            }
            if (!state->opts.warn[NCCMP_W_ALL]) {
                last_status = EXIT_DIFFER;
            }
            if(!state->opts.force) {
                break;
            }
        }
    }

    return last_status;
}
