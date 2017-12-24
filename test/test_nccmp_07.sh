#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="07" 
export ARGS="-mdf test02a.$I.nc test02b.$I.nc"
export DATA=02
export EXPECT=1
export HELP="record names"
export SORT="-d"
$srcdir/test_nccmp_template.sh