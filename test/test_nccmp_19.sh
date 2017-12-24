#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="19" 
export ARGS="-mdf -w format test01a.$I.nc test01c.$I.nc"
export DATA=01
export EXPECT=0
export HELP="warning on file format difference"
export SORT="-d"
$srcdir/test_nccmp_template.sh