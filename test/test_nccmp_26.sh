#!/bin/bash
I=26
echo "$I. Ignore different missing_value."
LOG=stderr$I.tmp
CMD="$(./nccmp.sh) -d -s -M missing_value1.$I.nc missing_value2.$I.nc > $LOG 2>&1"

./test_nccmp_setup.sh missing $I
eval $CMD

if test "$?" = "0"; then :; else
    echo "Expected exit code 0."
    echo "$CMD"
    exit 1
fi

CMD2="grep 'are identical' $LOG > /dev/null"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp failed to ignore when files have different missing_values."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
