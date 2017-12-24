#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="67" 
export ARGS="-mdfq test20a.$I.nc test20b.$I.nc"
export DATA=20
export EXPECT=1
export HELP="Test -quiet option to silence diff printing."
export SORT="-d"
$srcdir/test_nccmp_template.sh