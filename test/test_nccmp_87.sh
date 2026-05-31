#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="87"
export ARGS="-d -v some_data test29a.$I.nc test29b.$I.nc"
export DATA=29
export EXPECT=0
export HELP="No diff when there is empty group"
export SORT="-d"
$srcdir/test_nccmp_template.sh