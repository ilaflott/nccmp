#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="69" 
export ARGS="-d test22a.$I.nc test22b.$I.nc"
export DATA=22
export EXPECT=0
export HELP="Test vars without any dimensions (scalars) with missing values."
export SORT="-d"
$srcdir/test_nccmp_template.sh
