#!/bin/sh
echo "58. Compare all asymmetric Netcdf4 atomic type pairings."

CMD="$(./nccmp.sh) -mdf test16a.nc test16b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr58.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected opaque type diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi
