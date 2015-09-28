#!/bin/sh
echo "30. Nans should be equal."
CMD="$(./nccmp.sh) -d -s -N test_nans1b.nc test_nans1c.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp still differs NaNs with override -N enabled."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo
    exit 1
fi
