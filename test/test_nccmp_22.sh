#!/bin/bash
export I="22" 
export ARGS="-mdf -s test01a.$I.nc test01a.$I.nc"
export DATA=01
export EXPECT=0
export HELP="report-identical when files match"
./test_nccmp_template.sh