#!/bin/sh

echo "40. Print up to limit number of var diff messages."
LOG=stderr40.txt
CMD="$(./nccmp.sh) -df -c 4 test8a.nc test8b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "Expected only 4 diffs printed out of 6."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi