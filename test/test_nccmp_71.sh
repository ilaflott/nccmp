#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="71"
export ARGS="-df subgroups1a.$I.nc subgroups1b.$I.nc"
export DATA=subgroups
export EXPECT=1
export HELP="Test huge number of subgroups."
export SORT="-d"
$srcdir/test_nccmp_template.sh
