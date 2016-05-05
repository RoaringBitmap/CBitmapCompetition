#!/bin/bash
######################
# To add a technique, simply append the file name of your executable to the commands array below
#######################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH=$DIR/../roaringlib/:$LD_LIBRARY_PATH
declare -a commands=('bitset_benchmarks' 'stl_vector_benchmarks' 'stl_hashset_benchmarks' 'bitmagic_benchmarks' 'roaring_benchmarks -r' 'roaring_benchmarks' 'ewah32_benchmarks'  'ewah64_benchmarks' 'wah32_benchmarks' 'concise_benchmarks' );
echo "# For data set, we print data size (in bits per value), successive intersections, successive unions and total unions (in cycles per input value) [we compute the total  union either naively or using a heap-based approach]"
for f in census1881_srt  census-income_srt census1881      census-income weather_sept_85_srt  wikileaks-noquotes_srt  wikileaks-noquotes; do
  echo "# processing file " $f
  for t in "${commands[@]}"; do
    echo "#" $t
    ./$t CRoaring/benchmarks/realdata/$f;
  done
  echo
  echo
done
