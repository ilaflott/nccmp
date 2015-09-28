#!/bin/sh

echo "3. nonrecord var metadata"
LOG=stderr03.txt
CMD="$(./nccmp.sh) -mf -x recdata,rec test1a.nc test1b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "nccmp attribute comparison failed for non-record variables."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
