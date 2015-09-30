#!/bin/bash
I=38
echo "$I. Format info."
LOG=stderr$I.tmp

../src/nccmp --version 2>&1 >/dev/null | grep 'header-pad = no'
if test "$?" = "0"; 
then
    # Does not have pad support.
    exit 0
fi

./test_nccmp_setup.sh pad $I
CMD="$(./nccmp.sh) -i padded.$I.nc > $LOG 2>&1"
eval $CMD

if test "$?" = "0"; then :; else
    echo "Expected exit code 1. Did not get info."
    echo "$CMD"
    exit 1
fi

CMD2="test $(grep -c 'header_pad_size=2048 bytes' $LOG) -eq 1"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp should have found header pad."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
