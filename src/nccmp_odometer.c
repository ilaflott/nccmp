#include <nccmp_odometer.h>

int nccmp_odometer(size_t* odo, size_t* limits, int first, int last)
{
  int i = last;
  while (i >= first) {
    odo[i] += 1;
    if (odo[i] > limits[i])
      odo[i] = 0;
    else
      break;

    --i;
  }

  /* Test for roll over. */
  for(i=first; i <= last; ++i) {
    if (odo[i] != 0)
      return 1;
  }

  /* If we get here then rolled over. */
  return 0;
}

#define CARDINALITY(TYPE)                                                            \
TYPE nccmp_odometer_cardinality_##TYPE(TYPE *odo, TYPE *limits, int first, int last) \
{                                       \
    int i;                              \
    TYPE result = 0;                    \
    TYPE prod = 1;                      \
    for(i = last;  i >= first; --i) {   \
        result += prod * odo[i];        \
        prod *= limits[i];              \
    }                                   \
    return result;                      \
}
CARDINALITY(int)
CARDINALITY(size_t)
#undef CARDINALITY
