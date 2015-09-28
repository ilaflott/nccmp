#!/bin/sh
echo "52. Netcdf4 extended atomic data types."

CMD="$(./nccmp.sh) -df test11a.nc test11b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr52.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected data diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi