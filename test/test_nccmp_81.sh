#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="81"
export ARGS="-m test_nansVarAtt1b.$I.nc test_nansVarAtt1c.$I.nc"
export DATA=nansVarAtt
export EXPECT=1
export HELP="NaNs in var attributes should differ"
export SORT="-d"
$srcdir/test_nccmp_template.sh