#!/bin/bash

export I="64" 
export ARGS="-df make_compound_vlen_nest1.$I.nc make_compound_vlen_nest2.$I.nc"
export DATA=compound
export EXPECT=1
export HELP="Netcdf4 nested vlen and compound types"
export SORT="-d"
$srcdir/test_nccmp_template.sh