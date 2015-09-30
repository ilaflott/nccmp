#!/bin/bash
export I="56" 
export ARGS="-mdf test14a.$I.nc test14b.$I.nc"
export DATA=14
export EXPECT=1
export HELP="Netcdf4 user defined vlen type diffs"
export SORT="-d"
./test_nccmp_template.sh