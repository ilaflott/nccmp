#!/bin/bash

I=33
echo "$I. Header pads content difference (lengths are equal)."
LOG=stderr$I.tmp

../src/nccmp --version 2>&1 >/dev/null | grep 'header-pad = no'
if test "$?" = "0"; 
then
    # Does not have pad support.
    exit 0
fi

$srcdir/test_nccmp_setup.sh pad $I
CMD="$($srcdir/nccmp.sh) -md -s -P -f $srcdir/pad1a.nc $srcdir/pad1b.nc > $LOG 2>&1"
eval $CMD 

if test "$?" = "1"; then :; else
    echo "Expected exit code 1. Did not find header pad content diff."
    echo "$CMD"
    exit 1
fi

CMD2="test $(grep -c 'VALUES : HEADERPAD' $LOG) -eq 3"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp should have found differing header pad contents with -P option."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
