#!/bin/sh
echo "27. Ignore different missing_value and _FillValue."
CMD="$(./nccmp.sh) -d -s --missing missing_value1.nc _FillValue1.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to ignore when files have different missing_value and _FillValue."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi

