#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="06" 
export ARGS="-df -v recdata,rec test01a.$I.nc test01b.$I.nc"
export DATA=01
export EXPECT=1
export HELP="record var values"
export SORT="-d"
$srcdir/test_nccmp_template.sh