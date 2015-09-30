#!/bin/bash
I=27
echo "$I. Ignore different missing_value and _FillValue."
LOG=stderr$I.tmp
CMD="$(./nccmp.sh) -d -s --missing missing_value1.$I.nc _FillValue1.$I.nc > $LOG 2>&1"

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
    echo "nccmp failed to ignore when files have different missing_value and _FillValue."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi

