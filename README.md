# CBitmapCompetition

## Description
The goal of this project is to compare on realistic data different C and C++ techniques representing sets of integers.

We start from a collection of sets of sorted integers.
For example, we might have 200 sets. This collection of sets is considered immutable (none of the benchmarks are allowed to modify them).
For each of these collections of sets, we do the following :  

-  We report the memory usage as best we can. Of course, data can always be compressed more or less aggressively to disk, but an important metric is how much RAM the bitmaps are using. It can be difficult to track every byte allocated, but we should do our best to get the memory usage within 1%. In C++, for STL containers, this can be done with a custom memory allocator that tracks memory allocation.
-  Intersections between successive bitmaps, each time generating a new bitmap. Thus if there are 200 bitmaps, we generate 199 temporary bitmaps representing the intersection. We then query and check the cardinality of the each intermediate bitmap (by summing them up).
-  Union between successive bitmaps, each time generating a new bitmap. We then query and check the cardinality of the result by summing them up.
-  We compute the union of the whole set and check the cardinality of the result. This can be done in various ways. A basic way involves doing successive in-place unions which should be the default. In this instance, it is fair to avoid creating temporary bitmaps.
-  Given that the maximal value stored in any bitmap is N, we check whether integers N/4, N/2 and 3N/4 belong to each bitmap (henceforth a quartile query).
- Differences between successive bitmaps (as with intersections and unions above).
- Symmetric differences between successive bitmaps (as with intersections and unions above).
- Iteration through all set values.
- Computing the intersection size between successive sets without materializing the answer.
- Computing the union size between successive sets without materializing the answer.
- Computing the difference size between successive sets without materializing the answer.
- Computing the symmetric intersection size between successive sets without materializing the answer.

Thus, we output several numbers:
- The memory usage, expressed as a floating-point numbers representing the number of bits used by value. In practice, this is computed by counting the number of bits used in RAM divided by the total cardinality. The total cardinality is given by the sum of the cardinalities of all sets. Thus if you have 200 sets each containing 10 values, your total cardinality is 2000, if you use 2000 bytes of memory, then your number of bits per value will be 8.0.
- The number of CPU cycles used to compute the successive intersections, normalized by the number of input values.
- The number of CPU cycles used to compute the successive unions, normalized by the number of input values.
- The number of CPU cycles used to compute the total union using a naive algorithm, normalized by the number of input values.
- The number of CPU cycles used to compute the total union using a heap-based or sort algorithm, normalized by the number of input values. In some cases, there might not be a reasonable heap-based or sort algorithm, but that is ok.
- The number of CPU cycles to for each quartile query.
- The number of CPU cycles used to compute the successive differences, normalized by the number of input values.
- The number of CPU cycles used to compute the successive symmetric differences, normalized by the number of input values.
- The number of CPU cycles used to iterate through all values, counting them, normalized by the number of input values.
- The number of CPU cycles used to compute the size of successive intersections, normalized by the number of input values.
- The number of CPU cycles used to compute the size of successive unions, normalized by the number of input values.
- The number of CPU cycles used to compute the size of successive differences, normalized by the number of input values.
- The number of CPU cycles used to compute the size of successive symmetric differences, normalized by the number of input values.

The normalization proceeds as follows. For the total union, we divide by the sum of the cardinality of all sets. For the successive intersections and unions, we divide by the number the sum of the cardinalities of the pairs of sets (so that, effectively, all but the first and last sets count twice).

For each competitive technique, we apply the following recipe:

* We assume a Linux-like environment.
* We put any non-standard dependency as a submodule of the project.
* We put the benchmarking code in a separate executable file in the ``src`` directory. Having separate files for each technique introduces redundancies and possibly errors, but it keeps the complexity of the project low. Each executable file is simple and thus easy to examine and debug. If we need to study the performance of one technique with tools like ``perf``, it is much easier of we have several independent executables.
* The executable should be able to take as the sole parameter a directory name containing text files where each text file is a comma-separated list of sorted integers (e.g., one of these directories: https://github.com/RoaringBitmap/CRoaring/tree/master/benchmarks/realdata). We have a header file in the CRoaring project to help parsing such data files... https://github.com/RoaringBitmap/CRoaring/blob/master/benchmarks/numbersfromtextfiles.h Consider using it. The code should take just one such directory at a time as a parameter.
* The executable should output the 4 performance numbers (memory usage, number of cycles...) on a single line, separated by spaces. If anything else is outputted, it should be prefixed by the '#' character. It is allowed for the executable to have a verbose flag (-v) that provides more insight into the results.
* The executable should not try to outsmart the benchmark. Keep your code simple.
* The  ``Makefile`` must be such that  ``make`` will build the executable.
* The scripts/all.sh script should be modified so that the executable is called (this should only require adding the executable file name to the ``commands`` array).

For a working example, see https://github.com/RoaringBitmap/CBitmapCompetition/blob/master/src/bitset_benchmarks.c



## Results

GCC 6.2, Skylake processor (3.4 GHz), CRoaring 0.2.3 (Sept. 9 2016).

For the first column of numbers, smaller is better (it is memory usage) whereas for all other columns, smaller is better (they are CPU cycles per operation).

First column is memory usage per value in bits.
Second column is 2-by-2 intersection time (with cardinality computation). Third column is 2-by-2 union time (with cardinality computation).
Fourth and fifth columns are wide unions times (unions of many sets) using either a naive or a heap-based approach depending on the column.
Last column is random access time.

Where it appears, the "-r" flag means "with better compression". The "-c" flag means "copy-on-write".



```bash
$ make test
./scripts/all.sh
# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value), successive differences, successive symmetric differences, iterations through all values, then we have pairwise count aggregates for successive intersections, successive unions, successive differences, successive symmetric differences
# processing file  census-income
# bitset_benchmarks
                 5.66                 0.13                 0.13                 0.09                 0.09                 3.67                  0.12                  0.12                     7.46                    0.09                  0.07                  0.09                  0.09
# stl_vector_benchmarks
                 0.00                 7.84                 7.69                43.43                16.74               420.20                  8.38                  8.79                     0.00                    4.85                  4.62                  5.21                  4.80
# stl_vector_benchmarks_memtracked
                32.00                 7.86                 9.64                54.97                19.62               439.50                  9.45                 10.73                     0.00                    4.82                  4.61                  5.25                  4.95
# stl_hashset_benchmarks_memtracked
               194.63                55.35               208.06                99.46               131.85                60.23                175.68                266.44                    28.88                   13.57                 13.59                 48.30                 13.58
# stl_hashset_benchmarks
                 0.00                55.28               209.50                98.41               131.48                60.83                176.81                266.03                    28.68                   13.28                 13.38                 47.43                 13.33
# bitmagic_benchmarks
                 7.76                 0.31                 0.33                 0.18                 0.22                15.95                  0.32                  0.32                    21.66                    0.12                  0.15                  0.14                  0.14
# bitmagic_benchmarks -r
                 4.46                 0.38                 0.35                 0.16                 0.20                38.36                  0.34                  0.35                    20.73                    0.14                  0.37                  0.28                  0.39
# slow_roaring_benchmarks -r
                 2.60                 0.45                 0.41                 0.06                 0.60                64.85                  0.52                  0.54                     5.97                    0.36                  0.36                  0.36                  0.36
# malloced_roaring_benchmarks -r
                 2.62                 0.30                 0.36                 0.05                 0.55                62.99                  0.36                  0.46                     5.99                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks -r
                 2.60                 0.22                 0.27                 0.05                 0.47                63.47                  0.28                  0.37                     5.95                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks -c -r
                 2.60                 0.22                 0.27                 0.05                 0.47                64.63                  0.28                  0.37                     5.95                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks
                 2.74                 0.19                 0.25                 0.08                 0.47                63.62                  0.22                  0.32                     5.93                    0.10                  0.10                  0.10                  0.10
# roaring_benchmarks -c
                 2.74                 0.19                 0.25                 0.08                 0.47                65.31                  0.22                  0.32                     5.93                    0.10                  0.10                  0.10                  0.10
# ewah32_benchmarks
                 3.29                 1.41                 2.02                 0.50                 1.58              3145.69                  1.72                  1.96                    13.09                    0.80                  1.11                  0.95                  1.13
# ewah64_benchmarks
                 3.86                 0.85                 1.07                 0.27                 1.18              1755.53                  0.95                  1.05                     9.12                    0.43                  0.60                  0.50                  0.61
# wah32_benchmarks
                 3.37                 1.80                 2.11                 1.83                 3.42             19323.53                  1.98                  2.04                     9.81                    0.98                  0.99                  1.00                  0.98
# concise_benchmarks
                 2.94                 2.15                 2.44                 2.25                 3.86             19107.68                  2.34                  2.38                     9.45                    1.25                  1.32                  1.26                  1.30


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.15                 0.14                 0.10                 0.10                 3.65                  0.12                  0.13                     6.72                    0.09                  0.07                  0.10                  0.10
# stl_vector_benchmarks
                 0.00                 4.52                 5.45                37.25                10.89               563.99                  5.54                  5.99                     0.00                    2.08                  1.87                  2.28                  2.10
# stl_vector_benchmarks_memtracked
                32.00                 4.68                 7.44                50.37                13.92               551.73                  6.70                  7.98                     0.00                    2.10                  1.86                  2.29                  2.12
# stl_hashset_benchmarks_memtracked
               195.02                28.23               172.98                59.20                76.33                55.05                132.91                216.61                    12.67                    6.66                  6.67                 29.80                  6.68
# stl_hashset_benchmarks
                 0.00                28.50               172.71                60.18                77.28                55.39                133.72                215.42                    12.69                    6.76                  6.76                 29.54                  6.77
# bitmagic_benchmarks
                 7.99                 0.32                 0.36                 0.19                 0.23                16.19                  0.33                  0.36                    10.70                    0.11                  0.15                  0.14                  0.14
# bitmagic_benchmarks -r
                 1.89                 0.25                 0.27                 0.19                 0.23                61.71                  0.26                  0.28                     6.84                    0.12                  0.20                  0.19                  0.22
# slow_roaring_benchmarks -r
                 0.60                 0.17                 0.30                 0.12                 0.03                55.90                  0.26                  0.37                     5.34                    0.13                  0.14                  0.13                  0.13
# malloced_roaring_benchmarks -r
                 0.62                 0.24                 0.39                 0.11                 0.05                56.63                  0.33                  0.45                     6.32                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks -r
                 0.60                 0.16                 0.30                 0.11                 0.03                73.95                  0.24                  0.35                     5.35                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks -c -r
                 0.60                 0.16                 0.30                 0.11                 0.03                77.79                  0.24                  0.34                     5.35                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks
                 2.99                 0.16                 0.28                 0.09                 0.54                63.34                  0.19                  0.32                     5.68                    0.08                  0.08                  0.08                  0.08
# roaring_benchmarks -c
                 2.99                 0.16                 0.28                 0.09                 0.55                68.58                  0.18                  0.31                     5.68                    0.08                  0.08                  0.08                  0.08
# ewah32_benchmarks
                 0.64                 0.28                 0.56                 0.16                 0.13               915.91                  0.42                  0.62                     6.11                    0.19                  0.29                  0.23                  0.31
# ewah64_benchmarks
                 0.90                 0.21                 0.40                 0.12                 0.10               656.48                  0.30                  0.43                     5.94                    0.14                  0.21                  0.17                  0.23
# wah32_benchmarks
                 0.65                 0.45                 0.65                 0.47                 0.47              3698.61                  0.54                  0.66                     7.63                    0.26                  0.28                  0.28                  0.28
# concise_benchmarks
                 0.55                 0.56                 0.76                 0.59                 0.62              3803.17                  0.65                  0.79                     7.84                    0.34                  0.38                  0.35                  0.38


# processing file  census1881
# bitset_benchmarks
               523.54                19.50                13.64                 9.85                 9.54                 2.32                 11.10                 13.25                    43.39                    6.31                  7.62                  8.26                  9.79
# stl_vector_benchmarks
                 0.00                 3.28                 6.10               542.25                66.63               177.57                  5.03                  6.38                     0.00                    1.45                  1.27                  1.63                  1.46
# stl_vector_benchmarks_memtracked
                32.00                 3.31                 8.09               722.11                75.54               171.28                  6.07                  8.50                     0.00                    1.46                  1.27                  1.63                  1.46
# stl_hashset_benchmarks_memtracked
               194.74                 0.49               265.16              1006.16              1232.57                40.27                235.58                369.01                    67.66                    0.48                  0.22                 44.47                  0.46
# stl_hashset_benchmarks
                 0.00                 0.48               265.35              1006.96              1235.44                39.94                235.93                369.39                    67.33                    0.46                  0.22                 44.65                  0.47
# bitmagic_benchmarks
                98.93                 1.87                 6.79                 3.02                 4.24                11.00                  3.70                  6.67                    82.82                    0.24                  2.20                  1.43                  2.18
# bitmagic_benchmarks -r
                46.32                 1.07                 3.68                 2.18                 3.89                13.06                  2.56                  3.61                    69.68                    0.09                  1.18                  1.60                  1.11
# slow_roaring_benchmarks -r
                15.08                 0.10                 1.10                 4.08                10.80                16.84                  0.62                  1.10                     6.09                    0.09                  0.10                  0.09                  0.09
# malloced_roaring_benchmarks -r
                15.35                 0.27                 2.80                 2.79                10.86                18.14                  1.53                  2.67                     5.25                    0.08                  0.09                  0.08                  0.08
# roaring_benchmarks -r
                15.08                 0.09                 1.13                 2.56                 8.76                18.68                  0.61                  1.13                     6.08                    0.08                  0.10                  0.08                  0.08
# roaring_benchmarks -c -r
                15.08                 0.09                 0.52                 2.55                 8.76                20.20                  0.25                  0.37                     6.11                    0.08                  0.10                  0.08                  0.09
# roaring_benchmarks
                15.97                 0.09                 1.06                 2.73                 6.61                18.66                  0.58                  1.03                     6.11                    0.08                  0.08                  0.08                  0.08
# roaring_benchmarks -c
                15.97                 0.09                 0.42                 2.73                 6.61                20.44                  0.23                  0.35                     6.13                    0.08                  0.09                  0.08                  0.08
# ewah32_benchmarks
                33.77                 3.74                28.19               276.13                53.98              8533.38                 15.83                 27.50                    29.40                    3.71                 12.07                  8.14                 12.02
# ewah64_benchmarks
                43.77                 1.95                15.08               102.71                26.78              4864.88                  8.51                 14.83                    26.82                    1.81                  6.71                  4.25                  6.74
# wah32_benchmarks
                34.32                14.53                29.27               387.67                44.85             26757.08                 21.90                 28.93                    29.26                    9.04                 11.58                 10.77                 11.55
# concise_benchmarks
                25.56                20.88                33.15               401.82                57.26             30027.37                 26.73                 33.33                    31.71                   12.13                 16.83                 14.49                 16.91


# processing file  census1881_srt
# bitset_benchmarks
               888.07                32.96                23.73                16.22                16.12                 2.61                 19.39                 22.48                    41.24                   11.42                 12.55                 14.21                 16.30
# stl_vector_benchmarks
                 0.00                 3.96                 6.18               618.55                35.22               158.18                  5.76                  6.41                     0.00                    1.70                  1.57                  1.99                  1.78
# stl_vector_benchmarks_memtracked
                32.00                 3.95                 8.39               839.73                41.27               159.29                  6.80                  8.74                     0.00                    1.77                  1.56                  1.94                  1.77
# stl_hashset_benchmarks_memtracked
               194.93                 1.39               200.69               357.66               400.06                39.89                171.04                243.14                    20.37                    1.05                  0.84                 21.12                  0.77
# stl_hashset_benchmarks
                 0.00                 1.41               200.71               353.14               399.22                39.81                171.21                243.20                    20.57                    1.05                  0.84                 21.52                  0.80
# bitmagic_benchmarks
               249.26                 4.93                16.64                 7.23                 9.13                11.40                  9.22                 16.57                    31.85                    0.77                  5.52                  3.66                  5.53
# bitmagic_benchmarks -r
                16.00                 0.83                 2.47                 4.58                 3.63                13.01                  2.94                  2.56                    10.88                    0.12                  0.52                  3.19                  0.52
# slow_roaring_benchmarks -r
                 2.16                 0.14                 0.93                 3.36                 4.29                18.15                  0.57                  0.93                     6.35                    0.06                  0.15                  0.09                  0.12
# malloced_roaring_benchmarks -r
                 2.77                 0.53                 4.98                 3.81                 9.14                19.85                  2.73                  4.87                     7.25                    0.06                  0.14                  0.10                  0.13
# roaring_benchmarks -r
                 2.16                 0.14                 0.95                 3.26                 4.22                19.63                  0.57                  0.95                     6.40                    0.06                  0.14                  0.09                  0.13
# roaring_benchmarks -c -r
                 2.16                 0.14                 0.56                 3.38                 4.32                20.52                  0.32                  0.43                     6.41                    0.06                  0.15                  0.10                  0.14
# roaring_benchmarks
                 6.09                 0.14                 1.18                 4.03                 8.79                25.15                  0.69                  1.12                     5.81                    0.08                  0.09                  0.08                  0.08
# roaring_benchmarks -c
                 6.09                 0.15                 0.51                 4.23                 8.82                26.81                  0.30                  0.39                     5.83                    0.08                  0.10                  0.08                  0.09
# ewah32_benchmarks
                 2.91                 0.50                 2.50               171.88                 5.21               541.83                  1.46                  2.47                     8.82                    0.44                  1.06                  0.72                  1.07
# ewah64_benchmarks
                 4.54                 0.39                 2.16               129.26                 4.60               446.67                  1.23                  2.09                     8.12                    0.32                  0.96                  0.59                  0.96
# wah32_benchmarks
                 2.95                 1.62                 3.03               197.52                 5.72              1784.53                  2.31                  2.97                     9.92                    1.02                  1.28                  1.04                  1.27
# concise_benchmarks
                 2.48                 1.94                 3.38               225.09                 6.56              1888.65                  2.59                  3.38                    10.16                    1.27                  1.65                  1.28                  1.66


# processing file  weather_sept_85
# bitset_benchmarks
                15.26                 0.42                 0.40                 0.35                 0.37                 3.53                  0.39                  0.40                    10.04                    0.27                  0.23                  0.28                  0.29
# stl_vector_benchmarks
                 0.00                 7.36                 7.70                94.08                27.33               582.88                  8.32                  8.72                     0.00                    4.54                  4.36                  5.11                  4.51
# stl_vector_benchmarks_memtracked
                32.00                 7.27                 9.97               118.54                31.49               582.29                  9.43                 11.15                     0.00                    4.53                  4.34                  5.15                  4.61
# stl_hashset_benchmarks_memtracked
               194.93                35.52               251.43               238.98               258.58                61.76                248.34                361.60                    41.40                   17.75                 17.77                 62.65                 17.80
# stl_hashset_benchmarks
                 0.00                35.46               250.10               236.64               255.70                59.45                248.38                358.76                    41.10                   17.52                 17.58                 61.38                 17.57
# bitmagic_benchmarks
                14.80                 0.59                 0.67                 0.37                 0.44                15.85                  0.61                  0.67                    33.14                    0.24                  0.28                  0.27                  0.28
# bitmagic_benchmarks -r
                 8.79                 0.59                 0.60                 0.28                 0.36                31.25                  0.55                  0.59                    31.73                    0.21                  0.57                  0.45                  0.60
# slow_roaring_benchmarks -r
                 5.38                 0.99                 0.88                 0.25                 1.40                80.36                  1.10                  1.06                     6.57                    0.89                  0.90                  0.89                  0.89
# malloced_roaring_benchmarks -r
                 5.42                 0.50                 0.73                 0.17                 1.30                78.05                  0.58                  0.91                     6.47                    0.26                  0.27                  0.26                  0.26
# roaring_benchmarks -r
                 5.38                 0.38                 0.57                 0.16                 1.08                80.31                  0.44                  0.76                     6.57                    0.26                  0.27                  0.27                  0.27
# roaring_benchmarks -c -r
                 5.38                 0.38                 0.57                 0.16                 1.09                92.10                  0.44                  0.75                     6.57                    0.27                  0.27                  0.27                  0.27
# roaring_benchmarks
                 5.44                 0.36                 0.55                 0.16                 1.05                80.15                  0.43                  0.75                     6.56                    0.25                  0.25                  0.25                  0.25
# roaring_benchmarks -c
                 5.44                 0.36                 0.56                 0.16                 1.06                95.01                  0.42                  0.73                     6.57                    0.25                  0.25                  0.25                  0.25
# ewah32_benchmarks
                 6.67                 2.64                 4.46                 2.35                 4.64             13360.00                  3.60                  4.37                    15.85                    1.64                  2.50                  2.05                  2.52
# ewah64_benchmarks
                 7.87                 1.56                 2.26                 1.29                 2.32              7380.11                  1.90                  2.22                    11.43                    0.84                  1.27                  1.03                  1.29
# wah32_benchmarks
                 6.82                 3.58                 4.59                 4.92                 6.89             65157.34                  4.28                  4.50                    12.41                    2.09                  2.10                  2.14                  2.09
# concise_benchmarks
                 5.88                 4.53                 5.41                 6.02                 8.09             69144.02                  5.10                  5.41                    12.77                    2.75                  2.92                  2.77                  2.90


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.33                 0.31                 0.25                 0.27                 3.57                  0.29                  0.31                     6.75                    0.19                  0.18                  0.21                  0.22
# stl_vector_benchmarks
                 0.00                 4.36                 6.13                69.07                14.09               679.61                  5.78                  6.23                     0.00                    1.97                  1.75                  2.19                  1.97
# stl_vector_benchmarks_memtracked
                32.00                 4.41                 7.88                83.10                17.40               673.08                  6.78                  8.39                     0.00                    1.97                  1.75                  2.18                  1.98
# stl_hashset_benchmarks_memtracked
               195.07                15.08               178.96                81.69               111.06                60.38                148.60                236.72                    12.94                    6.99                  7.01                 32.08                  7.00
# stl_hashset_benchmarks
                 0.00                15.11               180.86                82.29               111.54                61.27                149.38                237.04                    12.94                    7.19                  7.20                 32.34                  7.19
# bitmagic_benchmarks
                 9.03                 0.32                 0.46                 0.23                 0.27                14.96                  0.37                  0.46                    10.15                    0.12                  0.18                  0.16                  0.18
# bitmagic_benchmarks -r
                 0.96                 0.16                 0.20                 0.21                 0.33                53.37                  0.21                  0.22                     6.84                    0.09                  0.14                  0.17                  0.14
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                57.41                  0.15                  0.22                     5.26                    0.06                  0.07                  0.06                  0.07
# malloced_roaring_benchmarks -r
                 0.36                 0.15                 0.28                 0.04                 0.10                58.51                  0.23                  0.33                     6.25                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -r
                 0.34                 0.08                 0.17                 0.04                 0.05                57.19                  0.15                  0.22                     5.26                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -c -r
                 0.34                 0.08                 0.16                 0.04                 0.06                74.79                  0.13                  0.20                     5.26                    0.06                  0.07                  0.06                  0.07
# roaring_benchmarks
                 3.24                 0.12                 0.24                 0.07                 0.50                60.45                  0.17                  0.28                     5.71                    0.07                  0.07                  0.06                  0.06
# roaring_benchmarks -c
                 3.24                 0.12                 0.23                 0.07                 0.50                80.61                  0.16                  0.25                     5.71                    0.07                  0.07                  0.07                  0.07
# ewah32_benchmarks
                 0.54                 0.19                 0.48                 0.32                 0.11              2178.33                  0.34                  0.51                     5.94                    0.15                  0.23                  0.19                  0.24
# ewah64_benchmarks
                 0.86                 0.16                 0.41                 0.25                 0.09              1819.95                  0.28                  0.42                     5.81                    0.12                  0.20                  0.16                  0.21
# wah32_benchmarks
                 0.54                 0.34                 0.56                 0.58                 0.37              7482.52                  0.45                  0.56                     7.46                    0.22                  0.24                  0.23                  0.24
# concise_benchmarks
                 0.43                 0.41                 0.62                 0.69                 0.45              7339.01                  0.51                  0.63                     7.68                    0.26                  0.30                  0.27                  0.30


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                26.43                20.61                15.39                15.86                 3.80                 18.36                 20.01                    46.69                   11.69                 11.77                 13.53                 14.85
# stl_vector_benchmarks
                 0.00                 4.15                 6.95               515.14                51.62               184.54                  6.05                  7.41                     0.00                    2.12                  2.37                  2.53                  2.23
# stl_vector_benchmarks_memtracked
                32.00                 4.15                 9.24               695.15                62.37               164.10                  7.22                  9.85                     0.00                    2.19                  2.17                  2.60                  2.23
# stl_hashset_benchmarks_memtracked
               194.68                14.86               229.51               422.94               520.29                52.17                212.66                300.99                    24.26                   12.68                 10.65                 40.67                 11.23
# stl_hashset_benchmarks
                 0.00                14.86               228.66               412.53               521.99                51.60                214.63                301.39                    24.56                   12.76                 10.61                 40.31                 11.26
# bitmagic_benchmarks
               462.53                13.84                25.68                12.14                15.11                12.77                 18.10                 25.28                    59.19                    4.57                  9.50                  7.59                  9.46
# bitmagic_benchmarks -r
                29.84                 3.06                 5.74                11.89                16.62                21.65                  6.00                  5.60                    12.59                    1.57                  2.22                  5.27                  2.21
# slow_roaring_benchmarks -r
                 5.89                 1.41                 3.86                 3.05                24.56                27.46                  2.64                  3.77                     9.53                    0.86                  1.28                  1.06                  1.23
# malloced_roaring_benchmarks -r
                 7.04                 3.70                10.00                 3.46                30.52                29.22                  6.89                 10.08                    10.55                    0.86                  1.31                  1.10                  1.29
# roaring_benchmarks -r
                 5.89                 1.43                 3.87                 3.01                24.53                28.09                  2.65                  3.92                     9.62                    0.87                  1.33                  1.09                  1.29
# roaring_benchmarks -c -r
                 5.89                 1.42                 3.42                 3.03                24.79                34.65                  2.31                  3.20                     9.74                    0.90                  1.38                  1.16                  1.38
# roaring_benchmarks
                16.49                 1.36                 4.44                 4.49                21.10                37.19                  2.36                  4.70                     6.34                    0.76                  0.77                  0.74                  0.75
# roaring_benchmarks -c
                16.49                 1.41                 3.96                 4.59                21.27                49.41                  1.89                  3.78                     6.41                    0.82                  0.84                  0.82                  0.82
# ewah32_benchmarks
                10.83                 2.86                 9.23               328.40                48.87               994.66                  5.98                  8.96                    18.36                    2.46                  4.08                  3.28                  4.09
# ewah64_benchmarks
                19.41                 2.76                 8.79               203.66                41.23               989.90                  5.60                  8.50                    15.85                    2.32                  3.97                  2.95                  3.96
# wah32_benchmarks
                10.89                 6.10                10.88               414.71                46.48              2402.42                  8.25                 10.70                    16.09                    3.85                  4.46                  4.15                  4.47
# concise_benchmarks
                10.25                 6.38                11.30               450.18                52.31              2347.83                  8.44                 11.36                    16.66                    4.11                  4.89                  4.29                  4.93


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.58                18.38                14.51                15.11                 2.74                 15.25                 17.66                    33.55                    8.71                 10.46                 11.02                 12.82
# stl_vector_benchmarks
                 0.00                 4.15                 7.15               496.33                42.65               144.72                  5.74                  6.67                     0.00                    1.69                  1.57                  1.88                  1.70
# stl_vector_benchmarks_memtracked
                32.00                 4.28                 9.51               676.85                53.54               144.41                  7.57                  8.99                     0.00                    1.70                  1.54                  1.88                  1.71
# stl_hashset_benchmarks_memtracked
               195.18                26.12               179.90               273.28               359.64                49.94                159.41                229.05                    14.21                   10.34                  9.19                 25.42                  8.54
# stl_hashset_benchmarks
                 0.00                26.54               181.91               274.20               360.88                50.08                160.82                227.51                    13.17                   10.44                  9.40                 25.77                  8.77
# bitmagic_benchmarks
               370.07                10.15                21.28                 9.64                11.90                11.93                 14.25                 20.98                    38.64                    3.03                  7.69                  5.75                  7.72
# bitmagic_benchmarks -r
                23.99                 1.92                 4.20                 8.92                 7.03                15.30                  4.43                  4.24                     7.85                    0.51                  1.16                  3.64                  1.14
# slow_roaring_benchmarks -r
                 1.63                 0.59                 2.13                 2.06                 7.67                19.26                  1.27                  1.87                     5.96                    0.25                  0.45                  0.35                  0.44
# malloced_roaring_benchmarks -r
                 2.58                 2.15                 7.35                 2.31                12.66                22.79                  4.64                  7.15                     6.86                    0.26                  0.50                  0.37                  0.47
# roaring_benchmarks -r
                 1.63                 0.58                 2.16                 1.94                 7.60                20.64                  1.27                  1.95                     5.89                    0.26                  0.49                  0.37                  0.47
# roaring_benchmarks -c -r
                 1.63                 0.57                 1.91                 1.98                 7.77                23.61                  1.03                  1.47                     6.04                    0.28                  0.55                  0.41                  0.53
# roaring_benchmarks
                10.67                 0.70                 2.96                 3.15                13.64                27.80                  1.51                  3.08                     6.06                    0.35                  0.36                  0.33                  0.34
# roaring_benchmarks -c
                10.67                 0.69                 2.30                 3.20                13.79                31.16                  1.04                  2.15                     6.11                    0.37                  0.40                  0.37                  0.38
# ewah32_benchmarks
                 2.63                 0.73                 2.37               116.17                12.65               239.54                  1.59                  2.33                     8.08                    0.54                  1.00                  0.79                  0.99
# ewah64_benchmarks
                 4.66                 0.73                 2.21                92.82                11.45               233.80                  1.49                  2.16                     7.61                    0.50                  0.92                  0.69                  0.92
# wah32_benchmarks
                 2.67                 1.28                 2.57               132.60                12.38               528.32                  1.93                  2.51                     8.91                    0.76                  1.02                  0.95                  1.02
# concise_benchmarks
                 2.23                 1.43                 2.63               149.06                13.71               497.06                  1.99                  2.66                     9.23                    0.83                  1.16                  1.03                  1.16
```


```bash
$ ./scripts/big.sh

# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first na
ively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value), successive differences, successive
symmetric differences, iterations through all values, then we have pairwise count aggregates for successive intersections, successive unions, successive differ
ences, successive symmetric differences
# bitset_benchmarks
               100.00                25.35                25.37                 5.01                 5.01                 4.92                 25.32
      25.37                    28.90                    2.27                  2.14                  2.28                  2.27
# stl_vector_benchmarks
                 0.00                11.20                14.46               457.04               134.96              1911.23                 14.44
      16.63                     0.00                    8.23                  8.92                  9.27                  8.29
# stl_vector_benchmarks_memtracked
                32.00                11.67                15.67               481.40               108.32              2041.98                 15.08
      17.56                     0.00                    8.38                  8.21                  9.10                  8.24
# stl_hashset_benchmarks_memtracked
# stl_hashset_benchmarks
               100.05                 8.97                 9.21                 4.80                 5.37                15.33                  9.17
       9.20                    78.84                    2.61                  2.95                  2.92                  2.95
# bitmagic_benchmarks -r
                38.52                 9.11                 9.77                 3.92                 5.29                82.06                  8.05                  9.91                    46.28                    5.16                  8.89                  7.24                  9.17
# slow_roaring_benchmarks -r
                13.51                 8.11                 9.34                 4.79                11.74               284.75                  8.33
       9.19                     7.10                    6.82                  6.81                  6.82                  6.81
# malloced_roaring_benchmarks -r
                13.74                 3.99                 6.17                 3.90                 9.71               278.79                  4.67
       6.49                     7.05                    1.41                  1.39                  1.41                  1.39
# roaring_benchmarks -r
                13.51                 2.58                 4.97                 3.87                 8.04               381.29                  2.48
       4.09                     6.87                    1.37                  1.35                  1.37                  1.35
# roaring_benchmarks -c -r
                13.51                 2.58                 4.32                 3.86                 8.04               383.19                  2.47
       4.08                     6.87                    1.37                  1.35                  1.38                  1.35
# roaring_benchmarks
                13.51                 2.58                 4.98                 3.87                 8.02               387.15                  2.48
       4.09                     6.87                    1.36                  1.35                  1.37                  1.35
# roaring_benchmarks -c
                13.51                 2.58                 4.44                 3.87                 8.03               379.28                  2.47
       4.08                     6.87                    1.37                  1.35                  1.37                  1.35
# ewah32_benchmarks
                33.42                18.04                33.54                53.03                72.10          19929633.82                 26.18                 32.55                    25.49                   13.95                 17.66                 16.50                 17.81
# ewah64_benchmarks
                50.33                14.70                24.31                33.17                42.03          15554502.25                 20.09
      24.35                    23.42                   11.11                 14.29                 12.30                 14.25
# wah32_benchmarks
                33.79                24.52                34.85                68.66                65.75          58645138.60                 31.40
      34.78                    27.31                   16.24                 16.50                 16.76                 16.55
# concise_benchmarks
                22.56                27.79                37.54                69.49                77.28          48014150.67                 32.98
      37.83                    27.60                   18.91                 19.30                 18.90                 19.19
```


## Usage

This project uses submodules as dependencies.

To pull all the submodules:
```bash
git pull && git submodule init && git submodule update && git submodule status
```
This needs to be done once at the beginning, and whenever the project has updated a submodule.

```bash
make
make test
```

There are additional tests. To benchmark with a large synthetic dataset, do :
```bash
make bigtest
```

To find "hot" functions in roaring, do:

```bash
make hottest
```



Be aware that if you call ``git pull`` while in a submodule, you are *updating* the project for everyone. Try to stick as much as possible with the ``git pull && git submodule init && git submodule update && git submodule status`` as it is safest (it only impacts your local copy).

## Usage for developers of the project

If one of the submodules changes and you need to update it and update the project (for everyone, not just yourself):
```bash
git submodule foreach git pull origin master
make clean
make
make test
git commit -a
git push
```

Warning: issuing the command ``git submodule foreach git pull origin master`` while in the main directory or simply the command ``git pull origin master`` while in a submodule will change the repository because submodules are fixed to a given version, and you are updating the version to the latest available.


## Using perf for analysis

Example: 

```bash
$ perf record --call-graph dwarf ./hot_roaring_benchmarks -r  -m  wideunion CRoaring/benchmarks/realdata/census1881_srt
$ perf report --call-graph
```

