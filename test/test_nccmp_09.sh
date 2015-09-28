#!/bin/sh
echo "9. tolerance option test."
LOG=stderr09.txt
CMD="$(./nccmp.sh) -df -t 1e-3 test4a.nc test4b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo "nccmp data comparison failed tolerance tests."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
