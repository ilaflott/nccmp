#!/bin/sh
echo "53. Netcdf4 advanced variable encodings."

CMD="$(./nccmp.sh) -ef not_encoded.nc encoded.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr53.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected encoding diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi