#include <test_common.h>
#include <nccmp_stats.h>

int failures = 0;

int test_nccmp_compute_var_stats_mean()
{
    int result = PASS;
    double mean;
    nccmp_stats_t data;
    const double expected = 50.0;

    data.count = 10;
    data.sum = 500.0;
    mean = nccmp_compute_var_stats_mean(& data);

    if (! ALMOST_EQUAL(expected, mean, .001)) {
        result = FAIL;
        PRINT("Expected mean = %.15g, but got %.15g\n", expected, mean);
        goto recover;
    }

recover:
    return result;
}

int test_nccmp_compute_var_stats_stdev()
{
    int result = PASS;
    double stdev;
    nccmp_stats_t data;
    const double expected = 3.0276503540974917;

    data.count = 10;
    data.sum = 55.0;
    data.sum_squares = 385.0;
    stdev = nccmp_compute_var_stats_stdev(& data);

    if (! ALMOST_EQUAL(expected, stdev, .001)) {
        result = FAIL;
        PRINT("Expected mean = %.15g, but got %.15g\n", expected, stdev);
        goto recover;
    }

recover:
    return result;
}

nccmp_field_hashmap_t* create_hashmap(int num_fields)
{
    int i;
    char tmp[256];
    nccmp_field_hashmap_t *result = XMALLOC(nccmp_field_hashmap_t, 1);

    result->num_items = num_fields;
    result->items = XCALLOC(nccmp_user_type_t*, num_fields);

    for(i = 0; i < num_fields; ++i) {
        result->items[i] = XCALLOC(nccmp_user_type_t, 1);

        sprintf(tmp, "comp.test.field%d", i);
        result->items[i]->tree_name = XCALLOC(char, strlen(tmp) + 1);
        strcpy(result->items[i]->tree_name, tmp);

        sprintf(tmp, "field%d", i);
        result->items[i]->name = XCALLOC(char, strlen(tmp) + 1);
        strcpy(result->items[i]->name, tmp);
    }

    return result;
}

void destroy_hashmap(nccmp_field_hashmap_t *map)
{
    int i;
    if (map) {
        if (map->items) {
            for(i = 0; i < map->num_items; ++i) {
                if (map->items[i]) {
                    XFREE(map->items[i]->name);
                    XFREE(map->items[i]->tree_name);
                    XFREE(map->items[i]);
                }
            }
            XFREE(map->items);
        }
    }
    XFREE(map);
}

void create_stats_arrays(nccmp_group_stats_t *group_stats, int group_i, int num_vars, int num_stats)
{
    nccmp_stats_t **result = NULL;
    int var_i, stat_i;

    result = XCALLOC(nccmp_stats_t*, num_vars);
    printf("FIXME create_stats_arrays result=%p num_v=%d num_f=%d\n", (void*)result, num_vars, num_stats);
    for(var_i = 0; var_i < num_vars; ++var_i) {
		group_stats->num_stats[group_i][var_i] = num_stats;
        result[var_i] = XCALLOC(nccmp_stats_t, num_stats);
        printf("FIXME result=%p result[%d]=%p sizeof=%zu\n", (void*)result, var_i, (void*)result[var_i], sizeof(nccmp_stats_t));
        for(stat_i = 0; stat_i < num_stats; ++stat_i) {
        	result[var_i][stat_i].name = XCALLOC(char, 256);
        	result[var_i][stat_i].group_name = XCALLOC(char, 256);
        }
        nccmp_clear_var_stats_array(result[var_i], num_stats);
    }

	group_stats->num_vars[group_i] = num_vars;
    group_stats->stats[group_i] = result;
}

void init_group_var_stats(nccmp_group_stats_t *group_stats, int num_groups, int num_vars, int num_stats)
{
    int group_i;
    printf("FIXME init_group_var_stats group_stats=%p num_g=%d num_v=%d num_f=%d\n",
    	   (void*)group_stats, num_groups, num_vars, num_stats);
    if (group_stats) {
    	for(group_i = 0; group_i < num_groups; ++group_i) {
			group_stats->num_stats[group_i] = XCALLOC(int, num_vars);
    		create_stats_arrays(group_stats, group_i, num_vars, num_stats);
    	}
    }
}

nccmp_group_stats_t* create_group_stats(int num_groups, int num_vars, int num_fields)
{
    nccmp_group_stats_t *result = nccmp_create_group_stats_array(num_groups);
    nccmp_var_t *vars = XMALLOC(nccmp_var_t, num_vars);
    char tmp_str[256];
    int i;

    init_group_var_stats(result, num_groups, num_vars, num_fields);

    for(i = 0; i < num_vars; ++i) {
        sprintf(tmp_str, "var%d", i);
        strcpy(vars[i].name, tmp_str);
    }

    for(i = 0; i < num_groups; ++i) {
        result->group_ids[i] = i;
        sprintf(tmp_str, "/group%d", i+1);
        nccmp_init_group_stats(result, i, tmp_str, vars, num_vars, 0);
    }

    XFREE(vars);
    return result;
}

int test_create_group_stats()
{
    int result = PASS;
    int num_groups = 2, num_vars = 2, num_fields = 1;
    nccmp_group_stats_t *group_stats = create_group_stats(num_groups, num_vars, num_fields);
    nccmp_destroy_group_stats(group_stats);

    return result;
}

int test_create_hashmap()
{
    int result = PASS;
    int num_fields = 1;

    nccmp_field_hashmap_t *hashmap = create_hashmap(num_fields);
    destroy_hashmap(hashmap);

    return result;
}

int test_nccmp_init_group_stats()
{
    int result = PASS;
    nccmp_stats_t **found = NULL;
    int num_groups = 2, num_vars = 2, num_fields = 1;
    nccmp_group_stats_t *group_stats = create_group_stats(num_groups, num_vars, num_fields);
    nccmp_field_hashmap_t *hashmap = create_hashmap(num_fields);
    int group_id = 0;

    found = nccmp_find_group_var_stats(group_stats, group_id);
    if (!found) {
        result = FAIL;
        PRINT("Expected to find group %d, but got none.\n", group_id);
        goto recover;
    }

    if (found[0]->count || found[0]->sum || found[0]->sum_squares) {
        result = FAIL;
        PRINT("Expected to find reset stats in group %d.\n", group_id);
        goto recover;
    }

    group_id = 1;
    found = nccmp_find_group_var_stats(group_stats, group_id);
    if (!found) {
        result = FAIL;
        PRINT("Expected to find group %d, but got none.\n", group_id);
        goto recover;
    }

    if (found[0]->count || found[0]->sum || found[0]->sum_squares) {
        result = FAIL;
        PRINT("Expected to find reset stats 0 in group %d.\n", group_id);
        goto recover;
    }

    if (found[1]->count || found[1]->sum || found[1]->sum_squares) {
        result = FAIL;
        PRINT("Expected to find reset stats 1 in group %d.\n", group_id);
        goto recover;
    }

recover:
    destroy_hashmap(hashmap);
    nccmp_destroy_group_stats(group_stats);

    return result;
}

int test_nccmp_update_stats()
{
    int result = PASS;
    int num_groups = 2, num_vars = 2, num_fields = 1;
    nccmp_field_hashmap_t *hashmap = create_hashmap(num_fields);
    nccmp_group_stats_t *group_stats = create_group_stats(num_groups, num_vars, num_fields);
    nccmp_stats_t *stats = NULL;
    int expect;

    nccmp_update_stats(stats, 1);
    nccmp_update_stats(stats, 2);
    expect = 2;
    if (stats->count != expect) {
        result = FAIL;
        PRINT("Expected count=%d but got %d\n", (int)expect, stats->count);
        goto recover;
    }
    expect = 3.0;
    if (stats->sum != expect) {
        result = FAIL;
        PRINT("Expected sum=%g but got %d\n", expect, stats->sum);
        goto recover;
    }
    expect = 1.0;
    if (stats->min != expect) {
        result = FAIL;
        PRINT("Expected min=%g but got %d\n", expect, stats->min);
        goto recover;
    }
    expect = 2.0;
    if (stats->max != expect) {
        result = FAIL;
        PRINT("Expected max=%g but got %d\n", expect, stats->max);
        goto recover;
    }

    stats = & group_stats->stats[0][1][1];
    nccmp_update_stats(stats, 1);
    nccmp_update_stats(stats, 2);
    expect = 2;
    if (stats->count != expect) {
        result = FAIL;
        PRINT("Expected count=%d but got %d\n", (int)expect, stats->count);
        goto recover;
    }
    expect = 3.0;
    if (stats->sum != expect) {
        result = FAIL;
        PRINT("Expected sum=%g but got %d\n", expect, stats->sum);
        goto recover;
    }
    expect = 1.0;
    if (stats->min != expect) {
        result = FAIL;
        PRINT("Expected min=%g but got %d\n", expect, stats->min);
        goto recover;
    }
    expect = 2.0;
    if (stats->max != expect) {
        result = FAIL;
        PRINT("Expected max=%g but got %d\n", expect, stats->max);
        goto recover;
    }

recover:
    destroy_hashmap(hashmap);
    nccmp_destroy_group_stats(group_stats);

    return result;
}

int main(int argc, char* argv[])
{
    TEST(test_nccmp_compute_var_stats_mean);
    TEST(test_nccmp_compute_var_stats_stdev);
    TEST(test_create_hashmap);
    TEST(test_create_group_stats);
    TEST(test_nccmp_init_group_stats);
    //TEST(test_nccmp_update_stats);

    return failures;
}
