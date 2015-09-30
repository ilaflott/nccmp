#!/bin/bash
I=25
echo "$I. Fail on extra eos in var attribute."
LOG=stderr$I.tmp
CMD="$(./nccmp.sh) -m -s -w eos eos1.nc eos2.nc > $LOG 2>&1"
eval $CMD

if test "$?" = "0"; then :; else
    echo "Expected exit code 0."
    echo "$CMD"
    exit 1
fi

CMD2="grep 'are identical' $LOG > /dev/null"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp failed to ignore when files that have attribute strings with extra trailing end-of-string nulls."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    echo
    exit 1
fi