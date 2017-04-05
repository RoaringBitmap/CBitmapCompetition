#!/bin/bash
######################
# To add a technique, simply append the file name of your executable to the commands array below
#######################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
declare -a commands=( 'slow_roaring_benchmarks -r' 'roaring_benchmarks -r'   );
echo "# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value), successive differences, successive symmetric differences, iterations through all values, then we have pairwise count aggregates for successive intersections, successive unions, successive differences, successive symmetric differences "
for f in  census-income census-income_srt census1881  census1881_srt  weather_sept_85  weather_sept_85_srt wikileaks-noquotes  wikileaks-noquotes_srt ; do
  echo "# processing file " $f
  for t in "${commands[@]}"; do
     echo "#" $t
    ./$t CRoaring/benchmarks/realdata/$f;
  done
  echo
  echo
done
