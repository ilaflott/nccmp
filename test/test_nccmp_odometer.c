#include "test_common.h"
#include <nccmp_odometer.h>

int failures = 0;

int test_odometer()
{
    int result = PASS;
    int status;
    size_t odo[4], limits[4];

    odo[0] = 2;
    odo[1] = 3;
    odo[2] = 4;
    odo[3] = 5;
    limits[0] = 10;
    limits[1] = 20;
    limits[2] = 30;
    limits[3] = 40;

    status = nccmp_odometer(odo, limits, 0, 3);
    if (! status) {
        PRINT("Expected status != 0.");
        result = FAIL;
        goto recover;
    }
    if (odo[3] != 6) {
        PRINT("Expected odo[3] == 6.");
        result = FAIL;
        goto recover;
    }

    odo[3] = 40;
    status = nccmp_odometer(odo, limits, 0, 3);
    if (! status) {
        PRINT("Expected status != 0.");
        result = FAIL;
        goto recover;
    }
    if (odo[3] != 0) {
        PRINT("Expected odo[3] == 0.");
        result = FAIL;
        goto recover;
    }
    if (odo[2] != 5) {
        PRINT("Expected odo[2] == 5.");
        result = FAIL;
        goto recover;
    }

    odo[0] = 10;
    odo[1] = 20;
    odo[2] = 30;
    odo[3] = 40;
    status = nccmp_odometer(odo, limits, 0, 3);
    if (status) {
        PRINT("Expected roll-over status = 0.");
        result = FAIL;
        goto recover;
    }

recover:

    return result;
}

int test_odometer_cardinality()
{
    int result = PASS;
    int cardinality, expected;
    int odo[4], limits[4];

    odo[0] = 2;
    odo[1] = 3;
    odo[2] = 4;
    odo[3] = 5;
    limits[0] = 10;
    limits[1] = 32;
    limits[2] = 500;
    limits[3] = 250;

    cardinality = nccmp_odometer_cardinality_int(odo, limits, 0, 2);
    expected = 33504;
    if (cardinality != expected) {
        PRINT("Expected cardinality == %d, but got %d", expected, cardinality);
        result = FAIL;
        goto recover;
    }

    cardinality = nccmp_odometer_cardinality_int(odo, limits, 1, 2);
    expected = 1504;
    if (cardinality != expected) {
        PRINT("Expected cardinality == %d, but got %d", expected, cardinality);
        result = FAIL;
        goto recover;
    }

recover:

    return result;
}

int main(int argc, char* argv[])
{
    TEST(test_odometer);
    TEST(test_odometer_cardinality);

    return failures;
}
