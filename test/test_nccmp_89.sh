#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

RUN="$($srcdir/nccmp.sh)"
TMP=stderr89.tmp

echo "89. Unrecognized option exits with code 2 (fatal)"

# Long unrecognized option
eval "$RUN --bogus > $TMP 2>&1"
if test "$?" -ne 2; then
    echo "FAIL: --bogus should exit code 2, got $?"
    rm -f $TMP
    exit 1
fi

# Short unrecognized option
eval "$RUN -Z > $TMP 2>&1"
if test "$?" -ne 2; then
    echo "FAIL: -Z should exit code 2, got $?"
    rm -f $TMP
    exit 1
fi

# Missing argument for option that requires one
eval "$RUN -v > $TMP 2>&1"
if test "$?" -ne 2; then
    echo "FAIL: -v without arg should exit code 2, got $?"
    rm -f $TMP
    exit 1
fi

rm -f $TMP
echo "PASS"
exit 0
