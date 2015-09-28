#ifndef NCCMP_THREAD_H
#define NCCMP_THREAD_H

#include <nccmp_state.h>

typedef struct NCCMP_THREAD_ARGS_T {
    nccmp_strlist_t       *var_names;
    int            rec;
    nccmp_state_t *state;
    int            ncid1;
    int            ncid2;
    int            id;
    int            result;
} nccmp_thread_args_t;

#endif
