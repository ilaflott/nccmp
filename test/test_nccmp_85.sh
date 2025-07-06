#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="85"
export ARGS="-d test27a.$I.nc test27b.$I.nc"
export DATA=27
export EXPECT=1
export HELP="Fail due to shape change"
export SORT="-d"
$srcdir/test_nccmp_template.sh