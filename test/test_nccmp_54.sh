#!/bin/sh
echo "54. Netcdf4 user defined enum type diffs."

CMD="$(./nccmp.sh) -mdf test12a.nc test12b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr54.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected enum diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi