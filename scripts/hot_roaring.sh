#!/bin/bash
######################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
declare -a commands=( 'hot_slow_roaring_benchmarks -r' 'hot_roaring_benchmarks -r'   );
echo "# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value), successive differences, successive symmetric differences, iterations through all values, then we have pairwise count aggregates for successive intersections, successive unions, successive differences, successive symmetric differences "
for f in  census-income census-income_srt census1881  census1881_srt  weather_sept_85  weather_sept_85_srt wikileaks-noquotes  wikileaks-noquotes_srt ; do
  echo "# processing file " $f
  for mode in intersection union difference symdifference wideunion wideunionheap intersectioncount unioncount differencecount symdifferencecount ; do
  for t in "${commands[@]}"; do
     echo "#" $t " -m " $mode "CRoaring/benchmarks/realdata/"$f
    perf record -q ./$t -m $mode  CRoaring/benchmarks/realdata/$f;
    perf report -F Overhead,Symbol |cat |grep -v '^#'|head -1
  done
  done
  echo
  echo
done
