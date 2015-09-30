#!/bin/bash
I=34
echo "$I. Format info."
LOG=stderr$I.tmp
CMD="$(./nccmp.sh) -i test01a.$I.nc > $LOG 2>&1"

./test_nccmp_setup.sh 01 $I
eval $CMD

if test "$?" = "0"; then :; else
    echo "Expected exit code 1. Did not get info."
    echo "$CMD"
    exit 1
fi

CMD2="test $(grep -c 'format=NC_FORMAT_CLASSIC' $LOG) -eq 1"
eval "$CMD2"

if test "$?" = "0"; then :; else
    echo "nccmp should have found classic format."
    echo "Test that failed: "
    echo "$CMD"
    echo "$CMD2"
    exit 1
fi
