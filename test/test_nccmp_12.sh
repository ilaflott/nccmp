#!/bin/sh
export I="12"
export ARGS="-mf -A standard_name test05a.$I.nc test05b.$I.nc"
export DATA=05
export EXPECT=1
export HELP="exclude var att test"
export SORT="-d"
./test_nccmp_template.sh