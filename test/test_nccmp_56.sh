#!/bin/sh
i=56
echo "$i. Netcdf4 user defined vlen type diffs."

CMD="$(./nccmp.sh) -mdf test14a.nc test14b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr$i.txt stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected vlen data diffs."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi
