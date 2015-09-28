#!/bin/sh
i=62
echo "$i. Netcdf4 user defined compound type nested fields."

CMD="$(./nccmp.sh) -df make_compound_nest_atomic1.nc make_compound_nest_atomic2.nc"
$CMD > stderr$i.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr$i.txt stderr$i.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected compound type diffs."
    echo "Test that failed: "
    echo "$CMD > stderr$i.tmp 2>&1"
    echo
    exit 1
fi
