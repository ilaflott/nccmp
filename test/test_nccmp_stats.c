#include <test_common.h>
#include <nccmp_stats.h>

int failures = 0;

int test_nccmp_compute_var_stats_mean()
{
    int result = PASS;
    double mean;
    nccmp_var_stats_t data;
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
    nccmp_var_stats_t data;
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

int test_nccmp_init_group_stats()
{
    int result = PASS;
    nccmp_var_stats_t *found = NULL;
    nccmp_group_stats_t *group_stats = nccmp_create_group_stats_array(2);
    group_stats->group_ids[0] = 1;
    group_stats->group_ids[1] = 2;
    nccmp_init_group_stats(group_stats, 1, 1);
    nccmp_init_group_stats(group_stats, 2, 2);

    found = nccmp_find_var_stats_array(group_stats, 1);
    if (!found) {
        result = FAIL;
        PRINT("Expected to find group 1, but got none.\n");
        goto recover;
    }

    if (found[0].count || found[0].sum || found[0].sum_squares) {
        result = FAIL;
        PRINT("Expected to find reset stats in group 1.\n");
        goto recover;
    }


    found = nccmp_find_var_stats_array(group_stats, 2);
    if (!found) {
        result = FAIL;
        PRINT("Expected to find group 2, but got none.\n");
        goto recover;
    }

    if (found[0].count || found[0].sum || found[0].sum_squares) {
        result = FAIL;
        PRINT("Expected to find reset stats 0 in group 2.\n");
        goto recover;
    }

    if (found[1].count || found[1].sum || found[1].sum_squares) {
        result = FAIL;
        PRINT("Expected to find reset stats 1 in group 2.\n");
        goto recover;
    }

recover:
    nccmp_destroy_group_stats(group_stats);

    return result;
}

int test_nccmp_update_stats()
{
    int result = PASS;
    nccmp_var_stats_t *stats = NULL;
    nccmp_group_stats_t *group_stats = nccmp_create_group_stats_array(2);
    group_stats->group_ids[0] = 1;
    group_stats->group_ids[1] = 2;
    nccmp_init_group_stats(group_stats, 1, 1);
    nccmp_init_group_stats(group_stats, 2, 2);
    int expect;

    stats = & group_stats->groups[0][0];
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

    stats = & group_stats->groups[1][1];
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
    nccmp_destroy_group_stats(group_stats);

    return result;
}

int main(int argc, char* argv[])
{
    TEST(test_nccmp_compute_var_stats_mean);
    TEST(test_nccmp_compute_var_stats_stdev);
    TEST(test_nccmp_init_group_stats);
    TEST(test_nccmp_update_stats);

    return failures;
}
