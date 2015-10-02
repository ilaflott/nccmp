#!/bin/bash

export I="17" 
export ARGS="-mgf -G author,lab -x data1,data2,data3,data4,recdata,rec test01a.$I.nc test01b.$I.nc"
export DATA=01
export EXPECT=1
export HELP="metadata globals exclude"
export SORT="-d"
$srcdir/test_nccmp_template.sh