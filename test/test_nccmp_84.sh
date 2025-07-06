#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="84"
export ARGS="-d test26a.$I.nc test26b.$I.nc"
export DATA=26
export EXPECT=1
export HELP="Fail due to shape change"
export SORT="-d"
$srcdir/test_nccmp_template.sh