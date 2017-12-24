#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="47" 
export ARGS="-df -C 4 -t 1 test08a.$I.nc test08b.$I.nc"
export DATA=08
export EXPECT=1
export HELP="Print up to limit number of diff messages with tolerance mode"
export SORT="-d"
$srcdir/test_nccmp_template.sh