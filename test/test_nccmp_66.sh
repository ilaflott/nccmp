#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="66" 
export ARGS="-df make_compound_array_user_type1.$I.nc make_compound_array_user_type2.$I.nc"
export DATA=compound
export EXPECT=1
export HELP="Netcdf4 compound field array user types"
export SORT="-d"
$srcdir/test_nccmp_template.sh