#!/bin/bash

export I="58" 
export ARGS="-mdf test16a.$I.nc test16b.$I.nc"
export DATA=16
export EXPECT=1
export HELP="Compare all asymmetric Netcdf4 atomic type pairings"
export SORT="-d"
$srcdir/test_nccmp_template.sh