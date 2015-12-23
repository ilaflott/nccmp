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

int test_nccmp_create_group_stats()
{
    int result = PASS;
    nccmp_group_stats_t *group_stats = NULL;
    const char *var_name = "var1";
    nccmp_var_t vars[1];
    nccmp_darray_t *types = nccmp_darray_create(1);
    nccmp_user_type_t *type = nccmp_create_user_type(1);
    const char *group_name = "/group";
    int nvars = 1;
    int group_id = 0;
    
    memset(& vars[0], 0, sizeof(nccmp_var_t));
    strcpy(vars[0].name, var_name);
    
    nccmp_darray_append(types, type);
    
    group_stats = nccmp_create_group_stats(
            group_id,
            group_name,
            vars,
            nvars,
            types);    
    
    if (!group_stats) {
        result = FAIL;
        PRINT("Expected non null result.\n");
        goto recover;        
    }
    
recover:    
    nccmp_destroy_group_stats(group_stats);
    nccmp_destroy_user_type(type);
    nccmp_darray_destroy(types);

    return result;
}

int main(int argc, char* argv[])
{
    TEST(test_nccmp_compute_var_stats_mean);
    TEST(test_nccmp_compute_var_stats_stdev);
    TEST(test_nccmp_create_group_stats);
    
    return failures;
}
