#!/bin/sh
echo "21. warning tag 'all'"
LOG=stderr21.txt
CMD="$(./nccmp.sh) -mdf -w all test1a.nc test1b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to handle an unknown warning tag."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi