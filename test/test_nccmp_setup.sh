#!/bin/bash 
#   Copyright (C) 2004-2007 Remik Ziemlinski @ noaa gov
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; see the file COPYING.
#   If not, write to the Free Software Foundation,
#   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

# Exit this script if any command fails.
set -e -v

# find 'ncgen' command
export NCGEN=`which ncgen 2> /dev/null`

if test "$?" = "0"; then :; else 
    echo "ERROR: ncgen command not found. Aborting tests..."
    exit 1
fi

NC_FORMATS=$(./ncgen_formats.sh)
NC3=$(echo $NC_FORMATS | cut -f1 -d' ')
NC4=$(echo $NC_FORMATS | cut -f2 -d' ')

# check for nccmp executable in source tree
export NCCMP=`which ../src/nccmp 2> /dev/null`

if test "$?" = "0"; then :; else
    echo "ERROR: nccmp not built or missing from ../src/"
    exit 1
fi

NCGEN_VER=$(ncgen -v 2>&1 | grep -e version | cut -d' ' -f4)
echo $NCGEN_VER

######################################################################
# create netcdf test files
NCDL="19"
FMT=(0 $NC3 $NC3 $NC3 $NC3 $NC3 $NC3 $NC3 $NC3 $NC3 $NC4 $NC4 $NC4 $NC4 $NC4 $NC4 $NC4 $NC4 $NC4 $NC4)
ctr=1

while test "$ctr" -le "$NCDL" 
do
    for SUFFIX in a b
    do
        fin=test${ctr}${SUFFIX}
        # Check for backwards compatible test files. 
        if [ ${NCGEN_VER:0:5} != "4.3.3" ]; then
            if [ -e ${fin}.pre433.cdl ]; then
                fin=${fin}.pre433
            elif [ -e ${fin}.pre413.cdl ]; then
                fin=${fin}.pre413
            fi
        fi
        fin=${fin}.cdl
        fout=test${ctr}${SUFFIX}.nc
        echo Creating $fout
        CMD="ncgen -k ${FMT[${ctr}]} -o $fout $fin"
        echo "    $CMD"
        $CMD || exit 1
    done
    
    ctr=`expr $ctr + 1`
done

echo Create file in nc4 format to test warning option.
`ncgen -k $NC4 -o test1c.nc test1a.cdl` 
if test "$?" = "0"; then :;	else
	exit 1
fi

echo Create two files with different missing_values.
$NCGEN -o missing_value1.nc missing_value1.cdl 
$NCGEN -o missing_value2.nc missing_value2.cdl 
$NCGEN -o _FillValue1.nc _FillValue1.cdl 

echo Create the head pad test files for comparison of different lengths.
./padheader unpadded.nc 0 
./padheader padded.nc 2048 

echo Files for NaN features.
./make_nans test_nans1a.nc 0 # Without nans.
./make_nans test_nans1b.nc 1 # With nans.
./make_nans test_nans1c.nc 1 # With nans. Just separate file from version 1b.

echo For group tests.
$NCGEN -k $NC4 -o group1.nc -x group1.cdl
$NCGEN -k $NC4 -o group2.nc -x group2.cdl
 
echo To compare encoding features in hdf format.
./make_nc4_encodings 0 not_encoded.nc
./make_nc4_encodings 1 encoded.nc

echo For compound type with atomic arrays.
./make_compound_array_atomic 0 make_compound_array_atomic1.nc
./make_compound_array_atomic 1 make_compound_array_atomic2.nc

echo For compound type with user-type arrays.
./make_compound_array_user_type 1 make_compound_array_user_type1.nc 
./make_compound_array_user_type 2 make_compound_array_user_type2.nc

echo For compound type nested with atomics.
./make_compound_nest_atomic 0 make_compound_nest_atomic1.nc
./make_compound_nest_atomic 1 make_compound_nest_atomic2.nc

echo For compound/vlen nested types. ncgen cannot create these.
./make_compound_vlen_nest 0 make_compound_vlen_nest1.nc
./make_compound_vlen_nest 100 make_compound_vlen_nest2.nc
