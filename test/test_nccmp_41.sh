#!/bin/bash
export I="41" 
export ARGS="-df -c 4 -t 1 test08a.$I.nc test08b.$I.nc"
export DATA=08
export EXPECT=1
export HELP="Print up to limit number of var diff messages with tolerance mode"
export SORT="-d"
./test_nccmp_template.sh