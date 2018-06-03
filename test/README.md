## Adding New Test
After adding a new test file like test_nccmp_123.sh, update the lists in CMakeLists.txt and Makefile.am,
and create a file like stderr123.txt with the expected messages.

If you added a new cdl file, you must update the function "format" in test_nccmp_setup.sh.


## Debugging Failed Test
Run with verbose output: ctest -VV
    
After running the full suite and a test failed, look to see what it printed in build/test/stderr123.tmp.

