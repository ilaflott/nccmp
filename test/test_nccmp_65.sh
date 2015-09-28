#!/bin/sh
i=65
echo "$i. Netcdf4 compound field array atomic."

CMD="$(./nccmp.sh) -df make_compound_array_atomic1.nc make_compound_array_atomic2.nc"
$CMD > stderr$i.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr$i.txt stderr$i.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected data diffs."
    echo "Test that failed: "
    echo "$CMD > stderr$i.tmp 2>&1"
    echo
    exit 1
fi
