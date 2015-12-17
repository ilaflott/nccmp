#!/bin/bash

export I="68" 
export ARGS="-dfqS test21a.$I.nc test21b.$I.nc"
export DATA=21
export EXPECT=1
export HELP="Test statistics printing."
export SORT="-d"
$srcdir/test_nccmp_template.sh