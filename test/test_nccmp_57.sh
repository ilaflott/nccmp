#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="57" 
export ARGS="-mdf test15a.$I.nc test15b.$I.nc"
export DATA=15
export EXPECT=1
export HELP="Netcdf4 user defined compound type diffs"
export SORT="-d"
$srcdir/test_nccmp_template.sh