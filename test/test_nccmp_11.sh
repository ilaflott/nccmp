#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="11" 
export ARGS="-mf -A units,standard_name test05a.$I.nc test05b.$I.nc"
export DATA=05
export EXPECT=0
export HELP="exclude var att test1"
export SORT="-d"
$srcdir/test_nccmp_template.sh