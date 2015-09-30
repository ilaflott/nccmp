#!/bin/sh
export I="05" 
export ARGS="-mf -v recdata,rec test01a.$I.nc test01b.$I.nc"
export DATA=01
export EXPECT=1
export HELP="record var metadata"
./test_nccmp_template.sh