#!/bin/sh
echo "14. test failure of trying header-pad option when not available"

$(./nccmp.sh) --version 2>&1 >/dev/null | grep 'header-pad = yes'
if test "$?" = "0"; 
then
    # Was built with feature, so skip test for error message.
    exit 0
fi

CMD="$(./nccmp.sh) -qdmf --header-pad padded.nc unpadded.nc"
$CMD > stderr.tmp 2>&1

grep 'pad feature unavailable' stderr.tmp

if test "$?" = "0"; then :; else
    echo
    echo "Expected no header pad support."
    echo "Test that failed: "
    echo "$CMD > stderr.tmp 2>&1"
    exit 1
fi
