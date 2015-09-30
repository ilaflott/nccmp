#!/bin/bash
export I="53" 
export ARGS="-ef not_encoded.$I.nc encoded.$I.nc"
export DATA=encoding
export EXPECT=1
export HELP="Netcdf4 advanced variable encodings"
export SORT="-d"
./test_nccmp_template.sh