#!/bin/bash
export MEMCHECK=1

e=tmp.valgrind
RED='\033[0;31m'
NOCOLOR='\033[0m'

for f in test_nccmp_??.sh; do
    rm $e 
    ./$f
    
    if test "$?" = "0"; then :; else
        echo -e ${RED}FAIL: $f ${NOCOLOR}
    fi
    
    [ -f $e ] || echo -e "${RED}ERROR: $e not found ${NOCOLOR}"
    if test $(grep -c 'no leaks' $e) = "1"; then :; else
        echo -e ${RED}FAIL: Memory leak in $f ${NOCOLOR}
    fi
done

