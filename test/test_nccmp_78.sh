#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="78" 
export ARGS="-Bd test24a.$I.nc test24b.$I.nc"
export DATA=24
export EXPECT=1
export HELP="Use option to buffer entire var into memory"
export SORT="-d"
$srcdir/test_nccmp_template.sh
