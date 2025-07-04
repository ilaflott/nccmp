#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="79"
export ARGS="-m -g test_nansGlobalAtt1b.$I.nc test_nansGlobalAtt1c.$I.nc"
export DATA=nansGlobalAtt
export EXPECT=1
export HELP="NaNs in global attributes should differ"
export SORT="-d"
$srcdir/test_nccmp_template.sh