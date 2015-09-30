#!/bin/bash
export I="63" 
export ARGS="-df test19a.$I.nc test19b.$I.nc"
export DATA=19
export EXPECT=1
export HELP="Netcdf4 user-type vlens"
export SORT="-d"
./test_nccmp_template.sh