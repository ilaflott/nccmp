#!/bin/sh
echo "24. Fail on extra eos in var attribute."
CMD="$(./nccmp.sh) -m -s eos1.nc eos2.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "1"; then :; else
    echo
    echo "nccmp failed on files that have attribute strings with extra trailing end-of-string nulls."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi
