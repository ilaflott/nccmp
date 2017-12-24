#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

I=35
echo "$I. Format info."
LOG=stderr$I.tmp
CMD="$($srcdir/nccmp.sh) -i $srcdir/eos1.nc > $LOG 2>&1"
eval $CMD

if test "$?" = "0"; then :; else
    echo "Expected exit code 1. Did not get info."
    echo "$CMD"
    exit 1
fi

CMD2="test $(grep -c 'format=NC_FORMAT_64BIT' $LOG) -eq 1"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp should have found 64bit format."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
