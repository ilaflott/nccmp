#!/bin/bash
export I="51" 
export ARGS="-mf test11a.$I.nc test11b.$I.nc"
export DATA=11
export EXPECT=1
export HELP="Netcdf4 extended atomic attribute types"
export SORT="-d"
./test_nccmp_template.sh