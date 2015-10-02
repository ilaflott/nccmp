#!/bin/bash

export I="04" 
export ARGS="-df -x recdata,rec test01a.$I.nc test01b.$I.nc"
export DATA=01
export EXPECT=1
export HELP="nonrecord var values & dim lengths"
export SORT="-d"
$srcdir/test_nccmp_template.sh