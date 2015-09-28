#include <log.h>
#include <nccmp_nest.h>
#include <nccmp_odometer.h>

void nccmp_build_nest_index_str(nccmp_nest_t *nest, int fortran, char *result)
{
    char tmp[32];
    int i = 0;

    if (!nest || !result) {
        return;
    }

    strcat(result, "[");

    if (fortran) {
        // Reverse order for fortran and make 1-based.
        for(i = nest->num_dims - 1; i >= 0; --i) {
            sprintf(tmp, "%d", nest->index[i] + 1);
            strcat(result, tmp);
            if (i) {
                strcat(result, ",");
            }
        }
    } else {
        for(; i < nest->num_dims; ++i) {
            if (i) {
                strcat(result, ",");
            }
            sprintf(tmp, "%d", nest->index[i]);
            strcat(result, tmp);
        }
    }

    strcat(result, "]");
}

void nccmp_build_nest_str(nccmp_nest_t *nest, int fortran, char *result)
{
    if (!nest || !result) {
        return;
    }

    if (nest->prev) {
        nccmp_build_nest_str(nest->prev, fortran, result);
    }

    if (nccmp_is_user_type_field(nest->type1)) {
        if (strlen(result)) {
            strcat(result, ".");
        }
        strcat(result, nest->type1->name);
    }

    if (nest->num_dims > 0) {
        nccmp_build_nest_index_str(nest, fortran, result);
    }
}

int nccmp_nest_cardinal_index(nccmp_nest_t *nest)
{
    if (!nest) {
        return 0;
    }

    return nccmp_odometer_cardinality_int(nest->index, nest->type1->dim_sizes, 0, nest->num_dims-1);
}

void nccmp_destroy_nest(nccmp_nest_t* item)
{
    XFREE(item);
}

nccmp_nest_t* nccmp_create_nest()
{
    nccmp_nest_t *result = XMALLOC(nccmp_nest_t, 1);
    if (!result) {
        LOG_ERROR("Failed to allocated nested_compare_t.\n");
        exit(-1);
    }
    memset(result, 0, sizeof(nccmp_nest_t));
    return result;
}
