#!/bin/sh
echo "33. Header pads content difference (lengths are equal)."

../src/nccmp --version 2>&1 >/dev/null | grep 'header-pad = no'
if test "$?" = "0"; 
then
    # Does not have pad support.
    exit 0
fi

CMD="$(./nccmp.sh) -md -s -P -f test_pad1a.nc test_pad1b.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1. Did not find header pad content diff."
    exit 1
fi

test $(grep -c 'VALUES : HEADERPAD' stderr.tmp) -eq "3"

if test "$?" = "0"; then :; else
    echo
    echo "nccmp should have found differing header pad contents with -P option."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo
    exit 1
fi
