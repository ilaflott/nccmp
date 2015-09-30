#!/bin/bash
export I="61" 
export ARGS="-df test18a.$I.nc test18b.$I.nc"
export DATA=18
export EXPECT=1
export HELP="Netcdf4 user defined compound type enum field nonarray"
export SORT="-d"
./test_nccmp_template.sh