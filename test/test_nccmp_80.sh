#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="80"
export ARGS="-m -g -N test_nansGlobalAtt1b.$I.nc test_nansGlobalAtt1c.$I.nc"
export DATA=nansGlobalAtt
export EXPECT=0
export HELP="NaNs in global attributes should be equal with override"
export SORT="-d"
$srcdir/test_nccmp_template.sh