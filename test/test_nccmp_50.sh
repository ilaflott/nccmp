#!/bin/bash
export I="50" 
export ARGS="-df test10a.$I.nc test10b.$I.nc"
export DATA=10
export EXPECT=1
export HELP="String array data"
export SORT="-d"
./test_nccmp_template.sh