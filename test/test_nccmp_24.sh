#!/bin/bash
I=24
echo "$I. Fail on extra eos in var attribute."
LOG=stderr$I.tmp
CMD="$(./nccmp.sh) -m -s eos1.nc eos2.nc > $LOG 2>&1"

eval $CMD

if test "$?" = "1"; then :; else
    echo "Expected exit code 1."
    echo "$CMD"
    exit 1
fi

CMD2="grep 'are identical' $LOG > /dev/null"
eval $CMD2

if test "$?" = "1"; then :; else
    echo "nccmp failed on files that have attribute strings with extra trailing end-of-string nulls."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    echo
    exit 1
fi
