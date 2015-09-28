#!/bin/sh
echo "34. Format info."
CMD="$(./nccmp.sh) -i test1a.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 1. Did not get info."
    exit 1
fi

test $(grep -c 'format=NC_FORMAT_CLASSIC' stderr.tmp) -eq "1"

if test "$?" = "0"; then :; else
    echo
    echo "nccmp should have found classic format."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    echo
    exit 1
fi
