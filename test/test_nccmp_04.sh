#!/bin/sh

echo "4. nonrecord var values & dim lengths"
LOG=stderr04.txt
CMD="$(./nccmp.sh) -df -x recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "nccmp data comparison failed for non-record variables."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
