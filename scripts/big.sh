#!/bin/bash
######################
# To add a technique, simply append the file name of your executable to the commands array below
#######################
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
${DIR}/generatebig.sh
declare -a commands=('bitset_benchmarks' 'stl_vector_benchmarks' 'stl_vector_benchmarks_memtracked' 'stl_hashset_benchmarks_memtracked' 'stl_hashset_benchmarks' 'bitmagic_benchmarks'  'bitmagic_benchmarks -r' 'slow_roaring_benchmarks -r' 'malloced_roaring_benchmarks -r' 'roaring_benchmarks -r' 'roaring_benchmarks -c -r' 'roaring_benchmarks' 'roaring_benchmarks -c'   'ewah32_benchmarks'  'ewah64_benchmarks' 'wah32_benchmarks' 'concise_benchmarks' );
echo "# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value), successive differences, successive symmetric differences, iterations through all values, then we have pairwise count aggregates for successive intersections, successive unions, successive differences, successive symmetric differences "
for t in "${commands[@]}"; do
     echo "#" $t
    ./$t bigtmp;
done
