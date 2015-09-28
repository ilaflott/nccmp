#!/bin/sh
echo "23. report-identical when files don't match"
CMD="$(./nccmp.sh) -mdf -s test1a.nc test1b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "1"; then :; else
    echo
    echo "nccmp -s (report identical) failed on files that don't match."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi
