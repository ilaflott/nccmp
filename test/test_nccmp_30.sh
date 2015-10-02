#!/bin/bash

I=30
echo "$I. Nans should be equal."
LOG=stderr$I.tmp
CMD="$($srcdir/nccmp.sh) -d -s -N test_nans1b.$I.nc test_nans1c.$I.nc > $LOG 2>&1"

$srcdir/test_nccmp_setup.sh nan $I
eval $CMD

if test "$?" = "0"; then :; else
    echo "Expected exit code 0."
    echo "$CMD"
    exit 1
fi

CMD2="grep 'are identical' $LOG > /dev/null"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp still differs NaNs with override -N enabled."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
