#!/bin/sh
echo "13. test header padded netcdf file vs. unpadded. Should have no diff without pad option."
LOG=stderr13.txt
CMD="$(./nccmp.sh) -dmf padded.nc unpadded.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo "nccmp comparison failed test with padded vs. unpadded header files."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi
