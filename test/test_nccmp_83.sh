#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="83"
export ARGS="-df -t 2 -T 2 -j test25a.$I.nc test25b.$I.nc"
export DATA=25
export EXPECT=1
export HELP="tolerance join option test"
$srcdir/test_nccmp_template.sh