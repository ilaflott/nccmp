#!/bin/bash

export I="49" 
export ARGS="-mf test10a.$I.nc test10b.$I.nc"
export DATA=10
export EXPECT=1
export HELP="String array attribute"
export SORT="-d"
$srcdir/test_nccmp_template.sh