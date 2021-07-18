#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="45" 
export ARGS="-dmf --group group group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=1
export HELP="Specific parent short group name"
export SORT="-d"
$srcdir/test_nccmp_template.sh
