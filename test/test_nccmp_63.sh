#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="63" 
export ARGS="-df test19a.$I.nc test19b.$I.nc"
export DATA=19
export EXPECT=1
export HELP="Netcdf4 user-type vlens"
export SORT="-d"
$srcdir/test_nccmp_template.sh