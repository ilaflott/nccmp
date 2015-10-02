#!/bin/bash

export I="21" 
export ARGS="-mdf -w all test01a.$I.nc test01b.$I.nc"
export DATA=01
export EXPECT=0
export HELP="warning tag 'all'"
export SORT="-d"
$srcdir/test_nccmp_template.sh