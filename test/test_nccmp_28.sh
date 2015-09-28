#!/bin/sh
echo "28. Nans differ from real values."
CMD="$(./nccmp.sh) -d -s test_nans1a.nc test_nans1b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep '1 <> nan' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp could not differentiate a NaN from a real value."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep '1 <> nan' stderr.tmp"
    echo
    exit 1
fi
