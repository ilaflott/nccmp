#!/bin/sh
echo "11. exclude var att test1." 
LOG=stderr11.txt
CMD="$(./nccmp.sh) -mf -A units,standard_name test5a.nc test5b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    echo
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
