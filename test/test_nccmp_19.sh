#!/bin/sh
echo "19. warning on file format difference"
LOG=stderr19.txt
CMD="$(./nccmp.sh) -mdf -w format test1a.nc test1c.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to ignore file format difference."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi