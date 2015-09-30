#!/bin/bash
export I="59" 
export ARGS="-df -t 5 test15a.$I.nc test15b.$I.nc"
export DATA=15
export EXPECT=1
export HELP="Netcdf4 user defined compound type tolerance diffs"
export SORT="-d"
./test_nccmp_template.sh