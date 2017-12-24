#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="18" 
export ARGS="-mgfh -G author,lab -x data1,data2,data3,data4,recdata,rec test01a.$I.nc test01b.$I.nc"
export DATA=01
export EXPECT=1
export HELP="metadata globals exclude with history"
export SORT="-d"
$srcdir/test_nccmp_template.sh