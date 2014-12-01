#!/bin/sh 
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

# find 'ncgen' command
NCGEN=`which ncgen 2> /dev/null`

if test "$?" = "0"; then :; else 
    echo "ERROR: ncgen command not found. Aborting tests..."
fi

# check for nccmp executable in source tree
NCCMP=`which ../src/nccmp 2> /dev/null`

if test "$?" = "0"; then :; else
    echo "ERROR: nccmp not built or missing from ../src/"
fi

######################################################################
# create netcdf test files
NCDL="7"
ctr=1

while test "$ctr" -le "$NCDL" 
do
  `ncgen -o test${ctr}a.nc test${ctr}a.cdl` 
  if test "$?" = "0"; then :; else
   	exit 1
  fi

  `ncgen -o test${ctr}b.nc test${ctr}b.cdl` 
  if test "$?" = "0"; then :; else
	  exit 1
  fi
    
  ctr=`expr $ctr + 1`
done

# Create file in other format to test warning option.
`ncgen -k 3 -o test1c.nc test1a.cdl` 
if test "$?" = "0"; 
	then :; 
	else
	exit 1
fi

# Create two files with different missing_values.
ncgen -o missing_value1.nc missing_value1.cdl
ncgen -o missing_value2.nc missing_value2.cdl
ncgen -o _FillValue1.nc _FillValue1.cdl

# Create the test files for comparison.
./padheader unpadded.nc 0
./padheader padded.nc 2048

# Files for NaN features.
./makenans test_nans1a.nc 0 # Without nans.
./makenans test_nans1b.nc 1 # With nans.
./makenans test_nans1c.nc 1 # With nans. Just separate file from version 1b.

######################################################################
# 1 metadata globals
echo "1."
CMD="$NCCMP -mgqf -x data1,data2,data3,data4,recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    echo "$CMD"
    exit 1
fi

diff stderr.tmp stderr1.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp metadata comparison failed on global attributes (excluding history attribute)."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr1.txt"
    echo
    exit 1
fi

# 2 metadata globals + history
echo "2."
CMD="$NCCMP -mghqf -x data1,data2,data3,data4,recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr2.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp metadata comparison failed on global history attribute values."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr2.txt"
    echo
    exit 1
fi

# 3 nonrecord var metadata
echo "3."
CMD="$NCCMP -mqf -x recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr3.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp attribute comparison failed for non-record variables."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr3.txt"
    echo
    exit 1
fi

# 4 nonrecord var values & dim lengths
echo "4."
CMD="$NCCMP -dqf -x recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr4.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp data comparison failed for non-record variables."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr4.txt"
    echo
    exit 1
fi

# 5 record var metadata
echo "5."
CMD="$NCCMP -mqf -v recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr5.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp metadata comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr5.txt"
    echo
    exit 1
fi

# 6 record var values
echo "6."
CMD="$NCCMP -dqf -v recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr6.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp data comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr6.txt"
    echo
    exit 1
fi

# 7 diff. record names
echo "7."
CMD="$NCCMP -mdqf test2a.nc test2b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr7.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp data comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr7.txt"
    echo
    exit 1
fi

# 8 diff. number of records
echo "8."
CMD="$NCCMP -mdqf test3a.nc test3b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 2."
    exit 1
fi

diff stderr.tmp stderr8.txt

if test "$?" = "0"; then :; else
    echo "nccmp data comparison failed for record variables."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr8.txt"
    echo
    exit 1
fi

# 9 tolerance option test.
echo "9."
CMD="$NCCMP -df -t 1e-3 test4a.nc test4b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr9.txt

if test "$?" = "0"; then :; else
    echo "nccmp data comparison failed tolerance tests."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr9.txt"
    echo
    exit 1
fi

# 10 tolerance percentage option test.
echo "10."
CMD="$NCCMP -df -T 10 test4a.nc test4b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" != "0"; then :; else
    echo
    echo "Expected non-zero exit code."
    echo
    exit 1
fi

diff stderr.tmp stderr10.txt

if test "$?" = "0"; then :; else
    echo "nccmp data comparison failed tolerance percentage tests."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr10.txt"
    echo
    exit 1
fi


# 11 exclude var att test1.
echo "11."
CMD="$NCCMP -qmf -A units,standard_name test5a.nc test5b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    echo
    exit 1
fi

diff stderr.tmp stderr11.txt

if test "$?" = "0"; then :; else
    echo "nccmp comparison failed excluding variable attributes test."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr11.txt"
    echo
    exit 1
fi

# 12 exclude var att test.
echo "12."
CMD="$NCCMP -qmf -A standard_name test5a.nc test5b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr12.txt

if test "$?" = "0"; then :; else
    echo "nccmp comparison failed excluding variable attributes test."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr12.txt"
    echo
    exit 1
fi

# 13 test header padded netcdf file vs. unpadded.
echo "13."

CMD="$NCCMP -qdmf padded.nc unpadded.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr13.txt

if test "$?" = "0"; then :; else
    echo "nccmp comparison failed test with padded vs. unpadded header files."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr13.txt"
    echo
    exit 1
fi

# Test string list facilities.
echo "14."
CMD="./test_strlist"
$CMD  > stderr.tmp 2>&1

diff stderr.tmp stderr-test_strlist.txt

if test "$?" = "0"; then :; else
    echo "String list tests failed."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr-test_strlist.txt"
    echo
    exit 1
fi

# Number of records not equal.
echo "15."
CMD="$NCCMP -qdmf test6a.nc test6b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    exit 1
fi

diff stderr.tmp stderr15.txt

if test "$?" = "0"; then :; else
    echo "nccmp comparison failed excluding variable attributes test."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr16.txt"
    echo
    exit 1
fi

# Number of records equal to zero.
echo "16."
CMD="$NCCMP -qdmf test7a.nc test7b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    exit 1
fi

diff stderr.tmp stderr16.txt

if test "$?" = "0"; then :; else
    echo "nccmp comparison failed excluding variable attributes test."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr16.txt"
    echo
    exit 1
fi

# 17 metadata globals exclude
echo "17."
CMD="$NCCMP -mgqf -G author,lab -x data1,data2,data3,data4,recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr17.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp metadata comparison failed on global attributes (excluding history attribute)."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr17.txt"
    echo
    exit 1
fi

# 18 metadata globals exclude with history
echo "18."
CMD="$NCCMP -mgqfh -G author,lab -x data1,data2,data3,data4,recdata,rec test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr18.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp metadata comparison failed on global attributes (excluding history attribute)."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr18.txt"
    echo
    exit 1
fi

# 19 warning on file format difference
echo "19."
CMD="$NCCMP -mdf -w format test1a.nc test1c.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

diff stderr.tmp stderr19.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to ignore file format difference."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr19.txt"
    echo
    exit 1
fi

# 20 unknown warning tag
echo "20."
CMD="$NCCMP -mdf -w blahblah test1a.nc test1c.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

diff stderr.tmp stderr20.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to handle an unknown warning tag."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr20.txt"
    echo
    exit 1
fi

# 21 warning tag 'all'
echo "21."
CMD="$NCCMP -mdf -w all test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

diff stderr.tmp stderr21.txt

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to handle an unknown warning tag."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "diff stderr.tmp stderr21.txt"
    echo
    exit 1
fi

# 22 report-identical when files match
echo "22."
CMD="$NCCMP -mdf -s test1a.nc test1a.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp -s (report identical) failed on files that match."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi

# 23 report-identical when files don't match
echo "23."
CMD="$NCCMP -mdf -s test1a.nc test1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "1"; then :; else
    echo
    echo "nccmp -s (report identical) failed on files that don't match."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi

# 24 Fail on extra eos in var attribute.
echo "24."
CMD="$NCCMP -m -s eos1.nc eos2.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "1"; then :; else
    echo
    echo "nccmp failed on files that have attribute strings with extra trailing end-of-string nulls."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi

# 25 Fail on extra eos in var attribute.
echo "25."
CMD="$NCCMP -m -s -w eos eos1.nc eos2.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to ignore when files that have attribute strings with extra trailing end-of-string nulls."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi

# 26 Ignore different missing_value.
echo "26."
CMD="$NCCMP -d -s -M missing_value1.nc missing_value2.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to ignore when files have different missing_values."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi

# 27 Ignore different missing_value and _FillValue.
echo "27."
CMD="$NCCMP -d -s --missing missing_value1.nc _FillValue1.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp failed to ignore when files have different missing_value and _FillValue."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep 'are identical' stderr.tmp"
    echo
    exit 1
fi

# 28 Nans differ from real values.
echo "28."
CMD="$NCCMP -d -s test_nans1a.nc test_nans1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep '1 <> nan' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp could not differentiate a NaN from a real value."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep '1 <> nan' stderr.tmp"
    echo
    exit 1
fi

# 29 Nans differ.
echo "29."
CMD="$NCCMP -d -s test_nans1b.nc test_nans1c.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep 'nan <> nan' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "NaNs should not be equal."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo "grep '1 <> nan' stderr.tmp"
    echo
    exit 1
fi

# 30 Nans should be equal.
echo "30."
CMD="$NCCMP -d -s -N test_nans1b.nc test_nans1c.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp still differs NaNs with override -N enabled."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo
    exit 1
fi

# 31 Header pads ignored.
echo "31."
CMD="$NCCMP -md -s test_pad1a.nc test_pad1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "0"; then :; else
    echo
    echo "Expected exit code 0."
    exit 1
fi

grep 'are identical' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp should have ignored differing header pads by default."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo
    exit 1
fi

# 32 Header pads difference detected.
echo "32."
CMD="$NCCMP -md -s -P -f test_pad1a.nc test_pad1b.nc"
$CMD  > stderr.tmp 2>&1

if test "$?" = "1"; then :; else
    echo
    echo "Expected exit code 1."
    exit 1
fi

grep 'HEADERPAD' stderr.tmp > /dev/null

if test "$?" = "0"; then :; else
    echo
    echo "nccmp should have found differing header pads with -P option."
    echo "Test that failed: "
    echo "$CMD  > stderr.tmp 2>&1"
    echo
    exit 1
fi

########################################################################
# success
exit 0
