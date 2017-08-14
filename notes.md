# compilation

* CC=/usr/bin/afl-clang CXX=/usr/bin/afl-clang++ cmake ./..

# run with

* ./build% afl-fuzz -i ./../testcases -o session_1 -- ./testsuite

