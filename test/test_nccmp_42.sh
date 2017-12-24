#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="42" 
export ARGS="-dmf group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=1
export HELP="Group complete diff"
export SORT="-d"
$srcdir/test_nccmp_template.sh