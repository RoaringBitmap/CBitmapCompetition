mkdir -p roaringlib && cd roaringlib && cmake ../CRoaring && make && cd ..
gcc -O3 -march=native  -o roaring_benchmarks roaring_benchmarks.c -ICRoaring/include -ICRoaring/benchmarks -Lroaringlib -lroaring
export LD_LIBRARY_PATH=./roaringlib/:$LD_LIBRARY_PATH
./roaring_benchmarks CRoaring/benchmarks/realdata/census1881
./roaring_benchmarks -r CRoaring/benchmarks/realdata/census1881
g++ -O3 -o bitmagic_benchmarks bitmagic_benchmarks.cpp -Ibm3.7.0/src -ICRoaring/benchmarks
./bitmagic_benchmarks CRoaring/benchmarks/realdata/census1881
