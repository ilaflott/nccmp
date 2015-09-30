#!/bin/bash
export I="54" 
export ARGS="-mdf test12a.$I.nc test12b.$I.nc"
export DATA=12
export EXPECT=1
export HELP="Netcdf4 user defined enum type diffs"
export SORT="-d"
./test_nccmp_template.sh