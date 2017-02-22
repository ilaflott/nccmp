#!/bin/bash

export I="03" 
export ARGS="-mf -x recdata,rec test01a.$I.nc test01b.$I.nc"
export DATA=01
export EXPECT=1
export HELP="nonrecord var metadata"
export SORT="-d"
$srcdir/test_nccmp_template.sh