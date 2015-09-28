#!/bin/sh
echo "39. Format info."

../src/nccmp --version 2>&1 >/dev/null | grep 'header-pad = no'
if test "$?" = "0"; 
then
    # Does not have pad support.
    exit 0
fi

CMD="$(./nccmp.sh) -i padded.nc unpadded.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 1. Did not get info."
    exit 1
fi

test $(grep -c 'header_pad_size' stderr.tmp) -eq "2"

if test "$?" = "0"; then :; else
    echo
    echo "nccmp should have checked info for both files."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo
    exit 1
fi
