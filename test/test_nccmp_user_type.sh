#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi 
LABEL="user_type"
DATA=compound
$srcdir/test_nccmp_setup.sh $DATA $LABEL
