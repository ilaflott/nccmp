#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="55" 
export ARGS="-mdf test13a.$I.nc test13b.$I.nc"
export DATA=13
export EXPECT=1
export HELP="Netcdf4 user defined opaque type diffs"
export SORT="-d"
$srcdir/test_nccmp_template.sh