#!/bin/sh
i=66
echo "$i. Netcdf4 compound field array user types."

CMD="$(./nccmp.sh) -df make_compound_array_user_type1.nc make_compound_array_user_type2.nc"
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
