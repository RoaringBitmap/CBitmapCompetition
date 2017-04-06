#!/bin/bash
######################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
declare -a commands=( 'hot_slow_roaring_benchmarks -r' 'hot_roaring_benchmarks -r'   );
for f in  census-income census-income_srt census1881  census1881_srt  weather_sept_85  weather_sept_85_srt wikileaks-noquotes  wikileaks-noquotes_srt ; do
  echo "# processing file " $f
  for mode in intersection union difference symdifference wideunion wideunionheap intersectioncount unioncount differencecount symdifferencecount ; do
  for t in "${commands[@]}"; do
     echo "#" $t " -m " $mode "CRoaring/benchmarks/realdata/"$f
    perf record -q ./$t -m $mode  CRoaring/benchmarks/realdata/$f;
    perf report -F Overhead,Symbol |cat |grep -v '^#'|head -3
  done
  done
  echo
  echo
done
