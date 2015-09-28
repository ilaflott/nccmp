#!/bin/sh

echo "5. record var metadata"
LOG=stderr05.txt
CMD="$(./nccmp.sh) -mf -v recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "nccmp metadata comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
