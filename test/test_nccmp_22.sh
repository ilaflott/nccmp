#!/bin/sh
echo "22. report-identical when files match"
CMD="$(./nccmp.sh) -mdf -s test1a.nc test1a.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp -s (report identical) failed on files that match."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi
