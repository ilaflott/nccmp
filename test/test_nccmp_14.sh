#!/bin/bash

I=14
echo "$I. test failure of trying header-pad option when not available"

$($srcdir/nccmp.sh) --version 2>&1 >/dev/null | grep 'header-pad = yes'
if test "$?" = "0"; 
then
    # Was built with feature, so skip test for error message.
    exit 0
fi

$srcdir/test_nccmp_setup.sh pad $I
CMD="$($srcdir/nccmp.sh) -dmf --header-pad padded.$I.nc unpadded.$I.nc > stderr$I.tmp 2>&1"
eval $CMD 

grep 'pad feature unavailable' stderr$I.tmp

if test "$?" = "0"; then :; else
    echo "Expected no header pad support."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi
