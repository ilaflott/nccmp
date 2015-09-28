#!/bin/sh

echo "8. number of records"
LOG=stderr08.txt
CMD="$(./nccmp.sh) -mdf test3a.nc test3b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 2."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo "nccmp data comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
