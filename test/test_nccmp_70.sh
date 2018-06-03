#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="70" 
export ARGS="-df test23a.$I.nc test23b.$I.nc"
export DATA=23
export EXPECT=1
export HELP="Test vars without any dimensions (scalars)."
export SORT="-d"
$srcdir/test_nccmp_template.sh
