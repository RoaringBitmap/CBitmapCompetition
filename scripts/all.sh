#!/bin/bash
######################
# To add a technique, simply append the file name of your executable to the commands array below
#######################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
INIT_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
declare -a commands=('bitset_benchmarks' 'stl_vector_benchmarks' 'stl_hashset_benchmarks' 'bitmagic_benchmarks'  'bitmagic_benchmarks -r' 'roaring_benchmarks -r' 'simpleroaring_benchmarks -r' 'roaring_benchmarks -c -r' 'simpleroaring_benchmarks -c -r' 'roaring_benchmarks' 'simpleroaring_benchmarks'  'roaring_benchmarks -c'  'simpleroaring_benchmarks -c'  'ewah32_benchmarks'  'ewah64_benchmarks' 'wah32_benchmarks' 'concise_benchmarks' );
echo "# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value) "
for f in census1881_srt  census-income_srt census1881      census-income weather_sept_85_srt  wikileaks-noquotes_srt  wikileaks-noquotes; do
  echo "# processing file " $f
  for t in "${commands[@]}"; do
    if [[ $t == "simpleroaring"* ]]; then
  LD_LIBRARY_PATH=$DIR/../simpleroaringlib/:$INIT_LD_LIBRARY_PATH
    fi
   if [[ $t == "roaring"* ]]; then 
  LD_LIBRARY_PATH=$DIR/../roaringlib/:$INIT_LD_LIBRARY_PATH
    fi
     echo "#" $t 
    ./$t CRoaring/benchmarks/realdata/$f;
  done
  echo
  echo
done
