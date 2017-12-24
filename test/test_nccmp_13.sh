#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="13"
export ARGS="-dmf padded.$I.nc unpadded.$I.nc"
export DATA=pad
export EXPECT=0
export HELP="Header padded netcdf file vs. unpadded. Should have no diff without pad option"
export SORT="-d"
$srcdir/test_nccmp_template.sh