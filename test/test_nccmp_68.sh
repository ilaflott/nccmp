#!/bin/bash

export I="68" 
export ARGS="-dfqS test20a.$I.nc test20b.$I.nc"
export DATA=20
export EXPECT=1
export HELP="Test statistics printing."
export SORT="-d"
$srcdir/test_nccmp_template.sh