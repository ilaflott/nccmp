#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="86"
export ARGS="-d -v some_data test28a.$I.nc test28b.$I.nc"
export DATA=28
export EXPECT=1
export HELP="Diff when there is empty group"
export SORT="-d"
$srcdir/test_nccmp_template.sh