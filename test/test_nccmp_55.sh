#!/bin/sh
echo "55. Netcdf4 user defined opaque type diffs."

CMD="$(./nccmp.sh) -mdf test13a.nc test13b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr55.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected opaque type diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi