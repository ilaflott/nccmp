#include <log.h>
#include <nccmp_error.h>
#include <nccmp_io.h>
#include <nccmp_ncinfo.h>
#include <nccmp_var.h>
#include "nccmp_common.h"

int all_varnames(nccmp_opt_t* opts, nccmp_strlist_t* list, int ncid1, int ncid2)
{
    nccmp_strlist_t* tmplist = nccmp_new_strlist(NC_MAX_VARS);

    nccmp_clear_strlist(list);

    if( nccmp_all_var_names(ncid1, list) != EXIT_SUCCESS ) {
        if (opts->debug) {
            LOG_ERROR("Failed to get all var names for ncid = %d\n", ncid1);
        }
        return EXIT_FAILED;
    }

    if( nccmp_all_var_names(ncid2, tmplist) != EXIT_SUCCESS ) {
        if (opts->debug) {
            LOG_ERROR("Failed to get all var names for ncid = %d\n", ncid2);
        }
        return EXIT_FAILED;
    }

    if( nccmp_union_strlist(tmplist, list, list) != EXIT_SUCCESS ) {
        if (opts->debug) {
            LOG_ERROR("Failed to union list of var names.\n");
        }
        return EXIT_FAILED;
    }

    nccmp_free_strlist(& tmplist);

    return EXIT_SUCCESS;
}

void broadcast_missing(nc_type var_type, nc_type att_type, nccmp_nc_type_t *values) {
  #define BROADCAST_MISSING(T) {                                               \
    switch(att_type) {                                                         \
      case NC_BYTE:   values->T = values->m_schar;    break;                   \
      case NC_CHAR:   values->T = values->m_text;     break;                   \
      case NC_DOUBLE: values->T = values->m_double;   break;                   \
      case NC_FLOAT:  values->T = values->m_float;    break;                   \
      case NC_INT:    values->T = values->m_int;      break;                   \
      case NC_INT64:  values->T = values->m_longlong; break;                   \
      case NC_SHORT:  values->T = values->m_short;    break;                   \
      case NC_UBYTE:  values->T = values->m_uchar;    break;                   \
      case NC_UINT:   values->T = values->m_uint;     break;                   \
      case NC_UINT64: values->T = values->m_ulonglong;break;                   \
      case NC_USHORT: values->T = values->m_ushort;   break;                   \
      default: break;                                                          \
    }                                                                          \
  }

  switch(var_type) {
    case NC_BYTE:   BROADCAST_MISSING(m_schar);     break;
    case NC_CHAR:   BROADCAST_MISSING(m_text);      break;
    case NC_DOUBLE: BROADCAST_MISSING(m_double);    break;
    case NC_FLOAT:  BROADCAST_MISSING(m_float);     break;
    case NC_INT:    BROADCAST_MISSING(m_int);       break;
    case NC_INT64:  BROADCAST_MISSING(m_longlong);  break;
    case NC_SHORT:  BROADCAST_MISSING(m_short);     break;
    case NC_UBYTE:  BROADCAST_MISSING(m_uchar);     break;
    case NC_UINT:   BROADCAST_MISSING(m_uint);      break;
    case NC_UINT64: BROADCAST_MISSING(m_ulonglong); break;
    case NC_USHORT: BROADCAST_MISSING(m_ushort);    break;
    default: break;
  }

  #undef BROADCAST_MISSING
}

int find_var(const char * name, nccmp_var_t *vars, int nvars)
{
    int i = 0;
    for(; i < nvars; ++i) {
        if (strcmp(name, vars[i].name) == 0) {
            return i;
        }
    }

    return -1;
}

char get_missing(int ncid, nccmp_var_t * var, const char* attname) {
    nc_type att_type;
    int status;

    status = nc_inq_atttype(ncid, var->varid, attname, &att_type);
    if (status != NC_NOERR) return 0;

    var->missing.type = att_type;

    #define GET(T) {                                                           \
        status = nc_get_att_##T(ncid, var->varid, attname, & var->missing.m_##T);        \
        if (status != NC_NOERR) {                                              \
            return 0;                                                          \
        }                                                                      \
    }

    switch(att_type) {
    case NC_BYTE:   GET(schar);     break;
    case NC_CHAR:   GET(text);      break;
    case NC_DOUBLE: GET(double);    break;
    case NC_FLOAT:  GET(float);     break;
    case NC_INT:    GET(int);       break;
    case NC_INT64:  GET(longlong);  break;
    case NC_SHORT:  GET(short);     break;
    case NC_UBYTE:  GET(uchar);     break;
    case NC_UINT:   GET(uint);      break;
    case NC_UINT64: GET(ulonglong); break;
    case NC_USHORT: GET(ushort);    break;
    default:        return 0;
    }

    #undef GET

    var->has_missing = 1;
    broadcast_missing(var->type, att_type, &var->missing);

    return 1;
}

int exclude_vars(nccmp_opt_t* opts, int ncid1, int ncid2, nccmp_strlist_t* result,
        nccmp_strlist_t* exclude)
{
    nccmp_strlist_t* all = NULL;
    int status = EXIT_SUCCESS;

    if (exclude->size == 0) {
        return EXIT_SUCCESS;
    }

    all = nccmp_new_strlist(NC_MAX_VARS);
    if(!all) {
        status = EXIT_FATAL;
    }

    if(all_varnames(opts, all, ncid1, ncid2)) {
        status = EXIT_FATAL;
    }

    if(nccmp_diff_strlist(all, exclude, result) != EXIT_SUCCESS) {
        status = EXIT_FATAL;
    }

    nccmp_free_strlist(& all);

    return status;
}

nccmp_strlist_t* make_cmp_var_name_list(nccmp_opt_t* opts, int ncid1, int ncid2)
{
    int status, i;

    nccmp_strlist_t* result = nccmp_new_strlist(NC_MAX_VARS);

    if(opts->variable) {
        if (opts->verbose) {
            LOG_INFO_CHOOSE(opts->color, "Using variables provided in list.\n");
        }
        status = nccmp_deep_copy_strlist(opts->variablelist, result);
    } else if (opts->exclude) {
        if (opts->verbose) {
            LOG_INFO_CHOOSE(opts->color, "Excluding variables in provided list.\n");
        }
        status = exclude_vars(opts, ncid1, ncid2, result, opts->excludelist);
    } else {
        if (opts->verbose) {
            LOG_INFO_CHOOSE(opts->color, "Using all variables.\n");
        }
        status = all_varnames(opts, result, ncid1, ncid2);
        if (opts->debug) {
            LOG_DEBUG_CHOOSE(opts->color, "status = %d, ncid1 = %d, ncid2 = %d\n", status, ncid1, ncid2);
        }
    }

    if (opts->verbose) {
        LOG_INFO_CHOOSE(opts->color, "%d variables to compare:", result->size);
        for(i=0; i < result->size; ++i) {
            printf(" %s", result->items[i]);
        }
        printf("\n");
    }

    if (EXIT_SUCCESS != status) {
        nccmp_free_strlist(& result);
    }

    return result;
}

int validate_var_names(nccmp_opt_t* opts, nccmp_strlist_t* names, nccmp_strlist_t** valid,
        int ncid1, int ncid2, nccmp_var_t *vars1, nccmp_var_t *vars2,
        int nvars1, int nvars2)
{
    int i, idx1, idx2, status = EXIT_SUCCESS;
    nccmp_strlist_t* processed = 0;

    *valid = 0;

    if (! names) {
        LOG_ERROR("Cannot validate empty var names list.\n");
        goto recover;
    }

    *valid = nccmp_new_strlist(names->size);
    if(! *valid) {
        LOG_ERROR("Failed to allocate string list for var validation.\n");
        goto recover;
    }

    processed = nccmp_new_strlist(names->size);
    if(! processed) {
        LOG_ERROR("Failed to allocate string list for var names.\n");
        goto recover;
    }

    for(i = 0; i < names->size; ++i) {
        if (nccmp_exists_in_strlist(processed, names->items[i])) {
            /* Skip duplicate names already processed. */
            continue;
        }

        if (opts->verbose) {
            LOG_INFO_CHOOSE(opts->color, "Validating variable %s.\n", names->items[i]);
        }

        nccmp_add_to_strlist(processed, names->items[i]);

        idx1 = find_var(names->items[i], vars1, nvars1);
        idx2 = find_var(names->items[i], vars2, nvars2);

        if (idx1 < 0) {
            if (! opts->metadata) { /* This gets reported in cmpmeta. */
                if (!opts->quiet) {
                    PRINT_DIFF(opts->color, opts->debug, " : Failed to find variable %s in file \"%s\".\n",
                               names->items[i], opts->file1);
                }
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status = EXIT_DIFFER;
            }
            if (opts->force) {
                continue;
            } else {
                break;
            }
        }

        if (idx2 < 0) {
            if (! opts->metadata) { /* This gets reported in cmpmeta. */
                if (!opts->quiet) {
                    PRINT_DIFF(opts->color, opts->debug, " : Failed to find variable %s in file \"%s\".\n",
                               names->items[i], opts->file2);
                }
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status = EXIT_DIFFER;
            }
            if (opts->force) {
                continue;
            } else {
                break;
            }
        }

        if (vars1[idx1].len != vars2[idx2].len) {
            if (!opts->quiet) {
                PRINT_DIFF(opts->color, opts->debug, " : VARIABLE : %s : SIZE : %d <> %d\n",
                           names->items[i], (int)vars1[idx1].len,
                           (int)vars2[idx2].len);
            }
            if (!opts->warn[NCCMP_W_ALL]) {
                status = EXIT_DIFFER;
            }
            if (opts->force) {
                continue;
            } else {
                break;
            }
        }

        nccmp_add_to_strlist(*valid, names->items[i]);
    }

recover:
    nccmp_free_strlist(& processed);

    return status;
}

void var_info_destroy(nccmp_var_t** vars, int* nvars)
{
    *nvars = 0;

    if (! *vars) {
        return;
    }

    XFREE(*vars);
}

void nccmp_print_vars(nccmp_var_t *vars, int nvars, int color)
{
    int i = 0;
    for(; i < nvars; ++i) {
        LOG_DEBUG_CHOOSE(color, 
                "i=%d name=%s ndims=%d natts=%d type=%d varid=%d\n",
                i, vars[i].name, vars[i].ndims, vars[i].natts, vars[i].type, 
                vars[i].varid);
    }
}

void var_info_get(int ncid, nccmp_var_t* vars, int nvars, int verbose, int color)
{
    int status, i, j, recid;

    status = nc_inq_unlimdim(ncid, & recid);
    HANDLE_NC_ERROR(status);

    for(i=0; i < nvars; ++i) {
        vars[i].varid = i;

        status = nc_inq_var(ncid, i, vars[i].name, & vars[i].type,
                           & vars[i].ndims, vars[i].dimids, & vars[i].natts);
        HANDLE_NC_ERROR(status);

        vars[i].len = 1;
        for(j=0; j < vars[i].ndims; ++j) {
            status = nc_inq_dimlen(ncid, vars[i].dimids[j], &vars[i].dimlens[j]);
            HANDLE_NC_ERROR(status);
            vars[i].len *= vars[i].dimlens[j];
        }

        vars[i].has_rec_dim = (vars[i].dimids[0] == recid);

        /* Get missing_value or _FillValue. */
        if (get_missing(ncid, &vars[i], "missing_value") == 0) {
            get_missing(ncid, &vars[i], "_FillValue");
        }
        if (verbose) {
            if (vars[i].has_missing) {
                switch(vars[i].missing.type) {
                case NC_BYTE:   LOG_INFO_CHOOSE(color, "\"%s\" missing value: %d (byte)\n",           vars[i].name, vars[i].missing.m_schar);           break;
                case NC_CHAR:   LOG_INFO_CHOOSE(color, "\"%s\" missing value: %d (char)\n",           vars[i].name, vars[i].missing.m_text);            break;
                case NC_DOUBLE: LOG_INFO_CHOOSE(color, "\"%s\" missing value: %g (double)\n",         vars[i].name, vars[i].missing.m_double);          break;
                case NC_FLOAT:  LOG_INFO_CHOOSE(color, "\"%s\" missing value: %g (float)\n",          vars[i].name, vars[i].missing.m_float);           break;
                case NC_INT:    LOG_INFO_CHOOSE(color, "\"%s\" missing value: %d (int)\n",            vars[i].name, vars[i].missing.m_int);             break;
                case NC_INT64:  LOG_INFO_CHOOSE(color, "\"%s\" missing value: %ll (long long)\n",     vars[i].name, vars[i].missing.m_longlong);        break;
                case NC_SHORT:  LOG_INFO_CHOOSE(color, "\"%s\" missing value: %d (short)\n",          vars[i].name, vars[i].missing.m_short);           break;
                case NC_UBYTE:  LOG_INFO_CHOOSE(color, "\"%s\" missing value: %d (unsigned byte)\n",  vars[i].name, vars[i].missing.m_uchar);           break;
                case NC_UINT:   LOG_INFO_CHOOSE(color, "\"%s\" missing value: %d (unsigned int)\n",   vars[i].name, vars[i].missing.m_uint);            break;
                case NC_UINT64: LOG_INFO_CHOOSE(color, "\"%s\" missing value: %ull (unsigned long long)\n", vars[i].name, vars[i].missing.m_ulonglong); break;
                case NC_USHORT: LOG_INFO_CHOOSE(color, "\"%s\" missing value: %d (unsigned short)\n",       vars[i].name, vars[i].missing.m_ushort);    break;
                }
            }
        }
    }
}

void var_info_init(int ncid, nccmp_var_t** vars, int* nvars)
{
    int status;

    var_info_destroy(vars, nvars);

    status = nc_inq_nvars(ncid, nvars);
    HANDLE_NC_ERROR(status);

    *vars = XMALLOC(nccmp_var_t, *nvars);
    memset(*vars, 0, sizeof(nccmp_var_t) * *nvars);
}

const nccmp_var_t* nccmp_find_var_by_id(const nccmp_var_t *vars, const int nvars, const int id)
{
	int i = 0;
	
	if (vars && (id >= 0)) {
		for(; i < nvars; ++i) {
			if (id == vars[i].varid) {
				return & vars[i];
			}
		}
	}

	return NULL;
}

const nccmp_var_t* nccmp_find_var_by_name(const nccmp_var_t *vars, const int nvars, const char * name)
{
	int i = 0;
	
	if (name) {
		for(; i < nvars; ++i) {
			if (strcmp(name, vars[i].name) == 0) {
				return & vars[i];
			}
		}
	}

	return NULL;
}

int nccmp_get_var_dimid(const nccmp_var_t *var, const int index)
{
	if (var && (index < var->ndims)) {
		return var->dimids[index];	
	}
	
	return -1;
}

size_t nccmp_get_var_last_dim_num_items(const nccmp_var_t *var)
{
    if (!var) {
        return 0;
    }
    
    if ((1 == var->ndims) && var->has_rec_dim) {
        return 1;
    } else {
        return var->dimlens[var->ndims-1];
    }
}

int nccmp_get_var_num_atts_by_id(const nccmp_var_t *vars, const int nvars, const int varid)
{
    if (vars && (nvars > 0)) {
        return vars[varid].natts;
    } 
    
    return 0;    
}

int nccmp_get_var_num_dims(const nccmp_var_t *vars, const int nvars, const int varid)
{
    if (vars && (nvars > 0)) {
        return vars[varid].ndims;
    } 
    
    return 0;    
}

int nccmp_get_var_dimid_by_id(const nccmp_var_t *vars, const int nvars, 
    const int varid, const int dim_index)
{
    if (vars && (nvars > 0) && (dim_index < vars[varid].ndims)) {
		return vars[varid].dimids[dim_index];
    } 
      
    return -1;
}

int nccmp_get_var_type(const nccmp_var_t *var)
{
	if (var) {
		return var->type;	
	}
	
	return 0;
}
