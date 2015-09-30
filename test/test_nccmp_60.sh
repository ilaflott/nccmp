#!/bin/bash
export I="60" 
export ARGS="-df test17a.$I.nc test17b.$I.nc"
export DATA=17
export EXPECT=1
export HELP="Netcdf4 user defined compound type opaq field nonarray"
export SORT="-d"
./test_nccmp_template.sh