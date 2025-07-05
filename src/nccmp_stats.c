#include <float.h>
#include <log.h>
#include <math.h>
#include <nccmp_common.h>
#include <nccmp_darray.h>
#include <nccmp_stats.h>
#include <nccmp_utils.h>

double nccmp_compute_var_stats_mean(nccmp_stats_t *data)
{
    if (data && (data->count > 0) ) {
        return data->sum_diff / data->count;
    } else {
        return 0.0;
    }
}

double nccmp_compute_var_stats_stdev(nccmp_stats_t *data) 
{
    if (data && (data->count > 1) ) {
        return sqrt( (data->sum_diff_squares - (data->sum_diff * data->sum_diff)/data->count) /
                     (data->count - 1) );
    } else {
        return 0.0;
    }
}

double nccmp_compute_var_stats_l1_relative(nccmp_stats_t *data)
{
	if (data && (data->count > 0) ) {
		return data->abs_sum_diff_over_truth / data->count;
	} else {
		return 0.0;
	}
}

double nccmp_compute_var_stats_l2(nccmp_stats_t *data)
{
	if (data && (data->count > 0) )	{
		return sqrt( data->sum_diff_squares );
	} else {
		return 0.0;
	}
}

double nccmp_compute_var_stats_l2_relative(nccmp_stats_t *data)
{
	if (data && (data->count > 0) )	{
		return sqrt( data->sum_diff_squares / data->sum_truth_squares );
	} else {
		return 0.0;
	}
}

nccmp_group_stats_t* nccmp_create_group_stats(
		int group_id,
		const char *group_name,
		nccmp_var_t *vars, 
		int num_vars,
		nccmp_darray_t *types)
{
    nccmp_group_stats_t *result = XMALLOC(nccmp_group_stats_t, 1);
    nccmp_var_stats_t *var_stats;
    nccmp_var_t *var;
    int i = 0;

	result->group_id = group_id;
	result->var_stats = nccmp_darray_create(num_vars);

	for(i = 0; i < num_vars; ++i) {
		var = & vars[i];
		var_stats = nccmp_create_var_stats(var, types, group_name);
		nccmp_darray_append(result->var_stats, var_stats);
    }

	return result;
}

nccmp_stats_t* nccmp_create_stats(const char *name, const char *group_name)
{
	nccmp_stats_t *result = XMALLOC(nccmp_stats_t, 1);
	result->count = 0;
    result->min_diff = INFINITY;
	result->max_diff = -INFINITY;
	result->sum_diff = 0.0;
	result->abs_sum_diff = 0.0;
	result->abs_sum_diff_over_truth = 0.0;
	result->sum_diff_squares = 0.0;
	result->sum_truth_squares = 0.0;

	if (name) {
		result->name = XMALLOC(char, strlen(name) + 1);
		strcpy(result->name, name);
	} else {
		result->name = NULL;
	}

	if (group_name) {
		result->group_name = XMALLOC(char, strlen(group_name) + 1);
		strcpy(result->group_name, group_name);
	} else {
		result->group_name = NULL;
	}

    #if HAVE_PTHREAD
        pthread_mutex_init(& result->lock, NULL);
    #endif

	return result;
}

nccmp_var_stats_t* nccmp_create_var_stats_many(		
		const char *var_name,
		const char *group_name,
		nccmp_darray_t *types,
        nccmp_darray_t *leafs)
{
    nccmp_var_stats_t *result = XMALLOC(nccmp_var_stats_t, 1);    
	nccmp_user_type_t *type = NULL;
    char *field_name = NULL;
    int i;
    
    result->stats = nccmp_darray_create(types->num_items);
    for(i = 0; i < types->num_items; ++i) {
        type = nccmp_find_user_type_by_id(leafs, i);
        if (type) {
            /* Copy names only for usable leaf nodes, ignore others. */
            nccmp_replace_delimited_prefix(
                    type->tree_name,
                    var_name,
                    '.',
                    & field_name);

            nccmp_darray_append(result->stats,
                                nccmp_create_stats(field_name, group_name));
            if (field_name) {
                XFREE(field_name);
            }
        } else {
            /* Add empty stat anyway to make array perfectly hashable by index. */
            nccmp_darray_append(result->stats, NULL);
        }
    }
    
    return result;    
}

nccmp_var_stats_t* nccmp_create_var_stats_single(
        const char *var_name,
        const char *group_name)
{
    nccmp_var_stats_t *result = XMALLOC(nccmp_var_stats_t, 1);        

    result->stats = nccmp_darray_create(1);
    nccmp_darray_append(result->stats,
					    nccmp_create_stats(var_name, group_name));
            
    return result;
}

nccmp_var_stats_t* nccmp_create_var_stats(
		nccmp_var_t *var,
		nccmp_darray_t *types,
		const char *group_name)
{
	nccmp_var_stats_t *result = NULL;
	nccmp_user_type_t *type = NULL;
	nccmp_darray_t *leafs = NULL;

	if (var) {
		if (types) {
			type = nccmp_find_user_type_by_type_id(types, var->type);
			leafs = nccmp_find_user_type_leafs(types, type);
		}

		if (nccmp_darray_empty(leafs)) {
            result = nccmp_create_var_stats_single(var->name, group_name);
		} else {
			result = nccmp_create_var_stats_many(var->name, group_name,
                        types, leafs);            
		}
		result->var_id = var->varid;
	}

	if (leafs) {
		nccmp_darray_destroy(leafs);
	}

	return result;
}

void nccmp_destroy_group_stats(nccmp_group_stats_t *item)
{
    if (item) {
		nccmp_destroy_var_stats_array(item->var_stats);
        XFREE(item);
    }
}

void nccmp_destroy_group_stats_array(nccmp_darray_t *array)
{
	int i;

	if (array) {
		for(i = 0; i < array->num_items; ++i) {
			nccmp_destroy_group_stats( (nccmp_group_stats_t*) array->items[i] );
		}
		nccmp_darray_destroy(array);
	}
}

void nccmp_destroy_stats(nccmp_stats_t *stats)
{
	if (stats) {
		XFREE(stats->name);
		XFREE(stats->group_name);

		#if HAVE_PTHREAD
			pthread_mutex_destroy(& stats->lock);
		#endif

		XFREE(stats);
	}
}

void nccmp_destroy_stats_array(nccmp_darray_t *array)
{
	int i;

	if (array) {
		for(i = 0; i < array->num_items; ++i) {
			nccmp_destroy_stats( (nccmp_stats_t*) array->items[i] );
		}
        nccmp_darray_destroy(array);
	}
}

void nccmp_destroy_var_stats(nccmp_var_stats_t *item)
{
    if (item) {
        nccmp_destroy_stats_array(item->stats);
        XFREE(item);
    }
}

void nccmp_destroy_var_stats_array(nccmp_darray_t *array)
{
    int i;
    
	if (array) {
        for(i = 0; i < array->num_items; ++i) {
            nccmp_destroy_var_stats( (nccmp_var_stats_t*) array->items[i] );
        }
		nccmp_darray_destroy(array);
	}
}

void nccmp_update_stats(nccmp_stats_t *stat, double truth, double prediction)
{
	const double diff      = truth - prediction;
	const double abs_diff  = diff < 0 ? -diff : diff;
	const double abs_truth = truth < 0 ? -truth : truth;

    ++stat->count;

    if (diff > stat->max_diff) {
    	stat->max_diff = diff;
    }

    if (diff < stat->min_diff) {
    	stat->min_diff = diff;
    }

    stat->sum_diff += diff;
    stat->abs_sum_diff += abs_diff;
    stat->sum_diff_squares += diff * diff;

	if( 0 == truth )
	{
    	//Avoid division and bias of zero. The diff is relatively maximal.
		stat->abs_sum_diff_over_truth += 1.0;
		stat->sum_truth_squares += prediction * prediction;
	}
	else
	{
		stat->abs_sum_diff_over_truth += abs_diff / abs_truth;
		stat->sum_truth_squares += truth * truth;
	}
}
