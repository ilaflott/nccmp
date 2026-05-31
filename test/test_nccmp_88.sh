#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="88"
export ARGS="-d -v BOGUS test29a.$I.nc test29b.$I.nc"
export DATA=29
export EXPECT=1
export HELP="Variable name missing in both files"
export SORT="-d"
$srcdir/test_nccmp_template.sh