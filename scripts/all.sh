#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH=$DIR/../roaringlib/:$LD_LIBRARY_PATH
declare -a commands=('bitmagic_benchmarks' 'roaring_benchmarks -r' 'roaring_benchmarks' 'ewah32_benchmarks'  'ewah64_benchmarks' );
echo "# For data set, we print file size (in bytes), successive intersections, successive unions and total unions (in cycles)"
for f in census1881_srt  census-income_srt census1881      census-income weather_sept_85_srt  wikileaks-noquotes_srt  wikileaks-noquotes; do
  echo "# processing file " $f
  for t in "${commands[@]}"; do
    echo "#" $t
    ./$t CRoaring/benchmarks/realdata/$f;
  done
  echo
  echo
done
