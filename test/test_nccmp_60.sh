#!/bin/sh
echo "60. Netcdf4 user defined compound type opaq field nonarray."

CMD="$(./nccmp.sh) -df test17a.nc test17b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr60.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected compound type diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi
