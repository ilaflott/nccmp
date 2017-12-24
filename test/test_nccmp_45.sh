#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="45" 
export ARGS="-dmf --group /group/subgroup,group group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=1
export HELP="Specific groups mixing short and full name and in order specified"
export SORT="-d"
$srcdir/test_nccmp_template.sh