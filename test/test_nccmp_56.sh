#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="56" 
export ARGS="-mdf test14a.$I.nc test14b.$I.nc"
export DATA=14
export EXPECT=1
export HELP="Netcdf4 user defined vlen type diffs"
export SORT="-d"
$srcdir/test_nccmp_template.sh