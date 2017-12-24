#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="15" 
export ARGS="-dmf test06a.$I.nc test06b.$I.nc"
export DATA=06
export EXPECT=1
export HELP="Number of records not equal"
export SORT="-d"
$srcdir/test_nccmp_template.sh