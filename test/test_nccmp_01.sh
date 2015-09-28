#!/bin/sh 
echo 1. metadata globals
CMD="$(./nccmp.sh) -mgf -x data1,data2,data3,data4,recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    echo "$CMD"
    exit 1
fi

diff stderr.tmp stderr01.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp metadata comparison failed on global attributes (excluding history attribute)."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr01.txt"
    echo
    exit 1
fi