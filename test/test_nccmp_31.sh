#!/bin/bash
I=31
echo "$I. Header pads ignored."
LOG=stderr$I.tmp
CMD="$(./nccmp.sh) -md -s unpadded.$I.nc padded.$I.nc > $LOG 2>&1"

./test_nccmp_setup.sh pad $I
eval $CMD

if test "$?" = "0"; then :; else
    echo "Expected exit code 0."
    echo "$CMD"
    exit 1
fi

CMD2="grep 'are identical' $LOG > /dev/null"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp should have ignored differing header pads by default."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
