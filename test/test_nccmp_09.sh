#!/bin/sh
export I="09" 
export ARGS="-df -t 1e-3 test04a.$I.nc test04b.$I.nc"
export DATA=04
export EXPECT=1
export HELP="tolerance option test"
export SORT="-d"
./test_nccmp_template.sh