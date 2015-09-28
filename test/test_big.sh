PASS=0
FAIL=1
TMPDIR=${TMPDIR-/tmp}
THREADS=4
TMP1=$TMPDIR/tmp.big.1.nc
TMP2=$TMPDIR/tmp.big.2.nc
NT=25
NZ=10
NY=1000
NX=2000

echo "Making first test file."
./make_big $NT $NZ $NY $NX 0 $TMP1
echo "Making second test file." 
./make_big $NT $NZ $NY $NX 1 $TMP2 

function run() {
    ../src/nccmp -mdfb -n $THREADS $TMP1 $TMP2 > stderr.tmp 2>&1
	if test "$?" -eq "1"; then :; else
		echo
		echo "run: Expected exit code 1."
		cat stderr.tmp
		exit $FAIL
	fi
	
	#grep -c DIFFER
}

function run_tol() {
    ../src/nccmp -mdfb -n $THREADS -t 1 $TMP1 $TMP2 > stderr.tol.tmp 2>&1
	if test "$?" -eq "1"; then :; else
		echo
		echo "run_tol: Expected exit code 1."
		cat stderr.tmp
		exit $FAIL
	fi
}

time run
time run_tol

exit $PASS