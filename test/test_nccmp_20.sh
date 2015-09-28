#!/bin/sh
echo "20. unknown warning tag, should print a warning and ignore."
LOG=stderr20.txt
CMD="$(./nccmp.sh) -mdf -w blahblah test1a.nc test1c.nc"
$CMD > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

cat stderr.tmp | sed -e 's/\(.*\)\(Warning.*\)/\2/g' > stderr2.tmp
diff stderr2.tmp $LOG

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to handle an unknown warning tag."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp $LOG"
    echo
    exit 1
fi