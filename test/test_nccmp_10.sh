#!/bin/sh

echo "10. tolerance percentage option test."
LOG=stderr10.txt
CMD="$(./nccmp.sh) -df -T 10 test4a.nc test4b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" != "0"; then :; else
    echo
    echo "Expected non-zero exit code."
    echo
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo "nccmp data comparison failed tolerance percentage tests."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
