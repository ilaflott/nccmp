#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

I=28
echo "$I. Nans differ from real values."
LOG=stderr$I.tmp
CMD="$($srcdir/nccmp.sh) -d -s test_nans1a.$I.nc test_nans1b.$I.nc > $LOG 2>&1"

$srcdir/test_nccmp_setup.sh nan $I
eval $CMD 

if test "$?" = "1"; then :; else
    echo "Expected exit code 1."
    echo "$CMD"
    exit 1
fi

CMD2="grep '1 <> nan' $LOG > /dev/null"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp could not differentiate a NaN from a real value."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
