#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

I=29
echo "$I. Nans differ."
LOG=stderr$I.tmp
CMD="$($srcdir/nccmp.sh) -d -s test_nans1b.$I.nc test_nans1c.$I.nc > $LOG 2>&1"

$srcdir/test_nccmp_setup.sh nan $I
eval $CMD

if test "$?" = "1"; then :; else
    echo "Expected exit code 1."
    echo "$CMD"
    exit 1
fi

CMD2="grep 'nan <> nan' $LOG > /dev/null"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "NaNs should not be equal."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
