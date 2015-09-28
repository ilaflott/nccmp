#!/bin/sh
i=64
echo "$i. Netcdf4 nested vlen and compound types."

CMD="$(./nccmp.sh) -df make_compound_vlen_nest1.nc make_compound_vlen_nest2.nc"
$CMD > stderr$i.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr$i.txt stderr$i.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected vlen data diffs."
    echo "Test that failed: "
    echo "$CMD > stderr$i.tmp 2>&1"
    echo
    exit 1
fi
