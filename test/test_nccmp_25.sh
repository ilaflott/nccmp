#!/bin/sh
echo "25. Fail on extra eos in var attribute."
CMD="$(./nccmp.sh) -m -s -w eos eos1.nc eos2.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to ignore when files that have attribute strings with extra trailing end-of-string nulls."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi