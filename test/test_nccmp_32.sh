#!/bin/bash

I=32
echo "$I. Header pads length difference (ignore contents)."
LOG=stderr$I.tmp

../src/nccmp --version 2>&1 >/dev/null | grep 'header-pad = no'
if test "$?" = "0"; 
then
    # Does not have pad support.
    exit 0
fi

$srcdir/test_nccmp_setup.sh pad $I
CMD="$($srcdir/nccmp.sh) -md -s -P unpadded.$I.nc padded.$I.nc > $LOG 2>&1"
eval $CMD

if test "$?" = "1"; then :; else
    echo "Expected exit code 1. Did not find header pad length diff."
    echo "$CMD"
    exit 1
fi

CMD2="grep 'LENGTHS : HEADERPAD' $LOG > /dev/null"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp should have found differing header pad lengths with -P option."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
