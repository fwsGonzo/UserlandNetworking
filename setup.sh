git submodule update --init --recursive
mkdir -p build/ && cd build
cmake .. && make -j
############################
echo *** Run with './build/testsuite'
