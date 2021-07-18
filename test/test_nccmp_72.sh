#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="72"
export ARGS="-dmf -R group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=1
export HELP="Only top group. No subgroups."
export SORT="-d"
$srcdir/test_nccmp_template.sh
