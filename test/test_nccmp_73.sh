#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="73"
export ARGS="-dmf --no-subgroups group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=1
export HELP="Only top group. No subgroups. Long option."
export SORT="-d"
$srcdir/test_nccmp_template.sh
