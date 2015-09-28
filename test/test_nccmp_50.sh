#!/bin/bash
echo "50. String array data."

CMD="$(./nccmp.sh) -df test10a.nc test10b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

NCGEN_VER=$(ncgen -v 2>&1 | grep -e version | cut -d' ' -f4)
if [ ${NCGEN_VER:0:5} != "4.3.3" ]
then
    diff stderr50.pre433.txt stderr.tmp
else
    diff stderr50.txt stderr.tmp
fi

if test "$?" = "0"; then :; else
    echo
    echo "Expected data diff."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi