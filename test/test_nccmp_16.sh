#!/bin/bash

export I="16" 
export ARGS="-dmf test07a.$I.nc test07b.$I.nc"
export DATA=07
export EXPECT=1
export HELP="Number of records equal to zero"
export SORT="-d"
$srcdir/test_nccmp_template.sh