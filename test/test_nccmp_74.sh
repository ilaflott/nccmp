#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="74"
export ARGS="-dmf --no-subgroups -r group group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=1
export HELP="Only subgroup. No subgroups. Long option, short name"
export SORT="-d"
$srcdir/test_nccmp_template.sh
