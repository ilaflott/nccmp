#!/bin/sh
echo "38. Format info."

../src/nccmp --version 2>&1 >/dev/null | grep 'header-pad = no'
if test "$?" = "0"; 
then
    # Does not have pad support.
    exit 0
fi

CMD="$(./nccmp.sh) -i padded.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 1. Did not get info."
    exit 1
fi

test $(grep -c 'header_pad_size=2048 bytes' stderr.tmp) -eq "1"

if test "$?" = "0"; then :; else
    echo
    echo "nccmp should have found header pad."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo
    exit 1
fi
