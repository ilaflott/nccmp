#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="65" 
export ARGS="-df make_compound_array_atomic1.$I.nc make_compound_array_atomic2.$I.nc"
export DATA=compound
export EXPECT=1
export HELP="Netcdf4 compound field array atomic"
export SORT="-d"
$srcdir/test_nccmp_template.sh