#!/bin/sh
echo "48. Posix threads."

../src/nccmp --version 2>&1 >/dev/null | grep -e 'pthreads.*yes'
if test "$?" = "1"; 
then
    # Does not support.
    exit 0
fi

CMD="$(./nccmp.sh) -df -c 3 -n 2 test9a.nc test9b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

test `grep -c 'DIFFER : VARIABLE : var : ' stderr.tmp` -eq 3

if test "$?" = "0"; then :; else
    echo
    echo "Expected only 3 diffs printed."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi