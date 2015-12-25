#!/bin/bash
export MEMCHECK=1
export srcdir=.

e=tmp.valgrind
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NOCOLOR='\033[0m'

for f in test_nccmp_??.sh; do
    rm -f $e 
    ./$f
    cp -f $e $e.$f

    if test "$?" = "0"; then :; else
        echo -e ${RED}FAIL: $f ${NOCOLOR}
    fi
    
    [ -f $e ] || echo -e "${RED}ERROR: $e not found ${NOCOLOR}"
    if test $(grep -c 'no leaks' $e) = "1"; then 
        echo -e ${GREEN}PASS${NOCOLOR}
    else
        echo -e ${RED}FAIL${NOCOLOR}: Memory leak in $f
    fi
done

