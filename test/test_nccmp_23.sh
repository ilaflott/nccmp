#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

I=23
echo "$I. report-identical when files don't match"
LOG=stderr$I.tmp
CMD="$($srcdir/nccmp.sh) -mdf -s test01a.$I.nc test01b.$I.nc  > $LOG 2>&1"

$srcdir/test_nccmp_setup.sh 01 $I
eval $CMD

if test "$?" = "1"; then :; else
    echo "Expected exit code 1."
    echo "$CMD"
    exit 1
fi

CMD2="grep 'are identical' $LOG > /dev/null"
eval $CMD2

if test "$?" = "1"; then :; else
    echo "nccmp -s (report identical) failed on files that don't match."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi

