#!/bin/sh
echo "15. Number of records not equal."
LOG=stderr15.txt
CMD="$(./nccmp.sh) -dmf test6a.nc test6b.nc"
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
