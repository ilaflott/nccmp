#!/bin/bash

export I="08" 
export ARGS="-mdf test03a.$I.nc test03b.$I.nc"
export DATA=03
export EXPECT=1
export HELP="number of records"
export SORT="-d"
$srcdir/test_nccmp_template.sh