#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="82"
export ARGS="-m -N test_nansVarAtt1b.$I.nc test_nansVarAtt1c.$I.nc"
export DATA=nansVarAtt
export EXPECT=0
export HELP="NaNs in var attributes should be equal with override"
export SORT="-d"
$srcdir/test_nccmp_template.sh