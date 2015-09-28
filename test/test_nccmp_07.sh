#!/bin/sh

echo "7. record names"
LOG=stderr07.txt
CMD="$(./nccmp.sh) -mdf test2a.nc test2b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "nccmp data comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
