#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

I=36
echo "$I. Format info."
LOG=stderr$I.tmp
CMD="$($srcdir/nccmp.sh) -i test01c.$I.nc > $LOG 2>&1"

$srcdir/test_nccmp_setup.sh 01 $I
eval $CMD

if test "$?" = "0"; then :; else
    echo "Expected exit code 1. Did not get info."
    echo "$CMD"
    exit 1
fi

CMD2="test $(grep -c 'format=NC_FORMAT_NETCDF4' $LOG) -eq 1"
eval $CMD2

if test "$?" = "0"; then :; else
    echo "nccmp should have found hdf format."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
