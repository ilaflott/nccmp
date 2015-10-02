#!/bin/bash

export I="43" 
export ARGS="-dmf -r subgroup group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=1
export HELP="Specific group short name"
export SORT="-d"
$srcdir/test_nccmp_template.sh