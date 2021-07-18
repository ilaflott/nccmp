#!/bin/bash
if [ -z "$srcdir" ]; then export srcdir=`dirname $0`; fi

export I="76"
export ARGS="-dmf -r foo group1.$I.nc group2.$I.nc"
export DATA=group
export EXPECT=2
export HELP="Non existing short group name."
export SORT="-d"
$srcdir/test_nccmp_template.sh
