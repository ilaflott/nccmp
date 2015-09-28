#!/bin/sh
echo "32. Header pads length difference (ignore contents)."

../src/nccmp --version 2>&1 >/dev/null | grep 'header-pad = no'
if test "$?" = "0"; 
then
    # Does not have pad support.
    exit 0
fi

CMD="$(./nccmp.sh) -md -s -P unpadded.nc padded.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1. Did not find header pad length diff."
    exit 1
fi

grep 'LENGTHS : HEADERPAD' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp should have found differing header pad lengths with -P option."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo
    exit 1
fi
