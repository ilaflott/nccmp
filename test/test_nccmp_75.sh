#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="75"
export ARGS="-dmf -R -r /group group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=1
export HELP="Only parent subgroup. No subgroups. Short option, full name"
export SORT="-d"
$srcdir/test_nccmp_template.sh
