#!/bin/bash

export I="40" 
export ARGS="-df -c 4 test08a.$I.nc test08b.$I.nc"
export DATA=08
export EXPECT=1
export HELP="Print up to limit number of var diff messages."
export SORT="-d"
$srcdir/test_nccmp_template.sh