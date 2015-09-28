#!/bin/sh
echo "29. Nans differ."
CMD="$(./nccmp.sh) -d -s test_nans1b.nc test_nans1c.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep 'nan <> nan' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "NaNs should not be equal."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep '1 <> nan' stderr.tmp"
    echo
    exit 1
fi
