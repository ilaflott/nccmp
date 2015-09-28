#ifndef NCCMP_ODOMETER_H
#define NCCMP_ODOMETER_H

#include <stdlib.h>

/*
  Mimics incrementing odometer.
  Returns 0 if rolled-over.

  @param odo: the counters that are updated.
  @param limits: the maximum values allowed per counter.
  @param first: index of first counter to update.
  @param last: index of last counter to update.
*/
int nccmp_odometer(size_t* odo, size_t* limits, int first, int last);

/* *********************************************************** */
/* Return cardinal value of current values in an odometer array
   from 'first' (left) index to 'last' (right) index inclusive.
   Examples:
       odometer_cardinality({2,3,4,0}, {10,32,500,250}, 0, 2) = 33504
       odometer_cardinality({2,3,4,0}, {10,32,500,250}, 1, 2) = 1504
*/
size_t nccmp_odometer_cardinality_size_t(size_t *odo, size_t *limits, int first, int last);
int nccmp_odometer_cardinality_int(int *odo, int *limits, int first, int last);

#endif
