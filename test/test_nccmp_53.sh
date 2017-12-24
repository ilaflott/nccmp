#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="53" 
export ARGS="-ef not_encoded.$I.nc encoded.$I.nc"
export DATA=encoding
export EXPECT=1
export HELP="Netcdf4 advanced variable encodings"
export SORT="-d"
$srcdir/test_nccmp_template.sh