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

int main(int argc, char* argv[])
{
    TEST(test_nccmp_compute_var_stats_mean);
    TEST(test_nccmp_compute_var_stats_stdev);

    return failures;
}
