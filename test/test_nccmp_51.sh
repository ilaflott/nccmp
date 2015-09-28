#!/bin/sh
echo "51. Netcdf4 extended atomic attribute types."

CMD="$(./nccmp.sh) -mf test11a.nc test11b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr51.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected metadata attribute diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi