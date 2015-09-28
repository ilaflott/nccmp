#!/bin/sh
echo "16. Number of records equal to zero."
LOG=stderr16.txt
CMD="$(./nccmp.sh) -dmf test7a.nc test7b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo "nccmp comparison failed excluding variable attributes test."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
