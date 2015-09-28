#!/bin/sh

echo "47. Print up to limit number of diff messages with tolerance mode."
LOG=stderr47.txt
CMD="$(./nccmp.sh) -df -C 4 -t 1 test8a.nc test8b.nc"
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