#!/bin/bash
I=20
echo "$I. unknown warning tag, should print a warning and ignore."
LOG=stderr$I.tmp
LOG2=stderr${I}b.tmp
TRUTH=stderr$I.txt
CMD="$(./nccmp.sh) -mdf -w blahblah test01a.$I.nc test01c.$I.nc > $LOG2 2>&1"

./test_nccmp_setup.sh 01 $I
eval $CMD

if test "$?" = "1"; then :; else
    echo "Expected exit code 1."
    echo "Test that failed: "
    echo "$CMD"
    exit 1
fi

cat $LOG2 | sed -e 's/\(.*\)\(Warning.*\)/\2/g' > $LOG
DIFF="diff $TRUTH $LOG"
eval $DIFF

if test "$?" = "0"; then :; else
    echo "nccmp failed to handle an unknown warning tag."
    echo "Test that failed: "
    echo "$CMD"
    echo "$DIFF"
    exit 1
fi

