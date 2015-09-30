#!/bin/bash
I=48
echo "$I. Posix threads."
LOG=stderr$I.tmp

../src/nccmp --version 2>&1 >/dev/null | grep -e 'pthreads.*yes'
if test "$?" = "1"; 
then
    # Does not support.
    exit 0
fi

./test_nccmp_setup.sh 09 $I
CMD="$(./nccmp.sh) -df -c 3 -n 2 test09a.$I.nc test09b.$I.nc > $LOG 2>&1"
eval $CMD 

if test "$?" = "1"; then :; else
    echo "Expected exit code 1."
    echo "$CMD"
    exit 1
fi

CMD2="test `grep -c 'DIFFER : VARIABLE : var : ' $LOG` -eq 3"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "Expected only 3 diffs printed."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi