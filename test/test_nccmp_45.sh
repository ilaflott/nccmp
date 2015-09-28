#!/bin/sh

echo "45. Specific groups mixing short and full name and in order specified."
LOG=stderr45.txt
CMD="$(./nccmp.sh) -dmf --group /group/subgroup,group group1.nc group2.nc"
$CMD

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

$CMD 2>&1 | sort > stderr.tmp
diff stderr.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "Expected group diffs."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi