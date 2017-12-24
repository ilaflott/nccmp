#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="10" 
export ARGS="-df -T 10 test04a.$I.nc test04b.$I.nc"
export DATA=04
export EXPECT=1
export HELP="tolerance percentage option test"
$srcdir/test_nccmp_template.sh