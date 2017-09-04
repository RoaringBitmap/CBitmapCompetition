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
                 5.66                 0.13                 0.13                 0.09                 0.09                 3.86                  0.12                  0.12                     7.50                    0.09                  0.07                  0.09                  0.09
# stl_vector_benchmarks
                 0.00                 7.83                 7.68                43.39                16.58               408.20                  8.39                  8.80                     0.00                    4.87                  4.61                  5.21                  4.80
# stl_vector_benchmarks_memtracked
                32.00                 7.95                 9.78                55.78                20.08               415.23                  9.35                 10.48                     0.00                    4.94                  4.64                  5.20                  4.93
# stl_hashset_benchmarks_memtracked
               194.63                54.92               205.96                96.06               124.78                62.37                174.33                263.19                    28.69                   13.29                 13.26                 47.11                 13.24
# stl_hashset_benchmarks
                 0.00                54.94               206.26                95.85               124.77                59.62                176.09                263.82                    28.51                   13.07                 13.09                 47.13                 13.21
# bitmagic_benchmarks
                 7.76                 0.31                 0.33                 0.18                 0.21                15.72                  0.31                  0.32                    21.79                    0.11                  0.14                  0.13                  0.13
# bitmagic_benchmarks -r
                 4.46                 0.38                 0.35                 0.17                 0.20                38.76                  0.33                  0.35                    20.70                    0.14                  0.37                  0.28                  0.39
# slow_roaring_benchmarks -r
                 2.60                 0.45                 0.41                 0.06                 0.59               137.14                  0.53                  0.53                     5.90                    0.36                  0.36                  0.36                  0.36
# malloced_roaring_benchmarks -r
                 2.62                 0.33                 0.36                 0.05                 0.54               141.65                  0.38                  0.46                     5.89                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks -r
                 2.60                 0.23                 0.27                 0.05                 0.47               137.20                  0.28                  0.37                     5.89                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks -c -r
                 2.60                 0.23                 0.28                 0.05                 0.46               139.82                  0.28                  0.37                     5.87                    0.13                  0.14                  0.13                  0.13
# roaring_benchmarks
                 2.74                 0.20                 0.25                 0.08                 0.46               135.93                  0.22                  0.32                     5.86                    0.10                  0.10                  0.10                  0.10
# roaring_benchmarks -c
                 2.74                 0.20                 0.25                 0.08                 0.46               139.06                  0.22                  0.32                     5.89                    0.10                  0.10                  0.10                  0.10
# ewah32_benchmarks
                 3.29                 1.41                 2.02                 0.50                 1.58              3145.41                  1.72                  1.96                    13.09                    0.80                  1.11                  0.95                  1.13
# ewah64_benchmarks
                 3.86                 0.85                 1.07                 0.27                 1.17              1756.75                  0.96                  1.05                     9.09                    0.43                  0.60                  0.50                  0.61
# wah32_benchmarks
                 3.37                 1.80                 2.11                 1.83                 3.40             19339.73                  1.98                  2.04                     9.23                    0.98                  0.99                  1.00                  0.99
# concise_benchmarks
                 2.94                 2.14                 2.44                 2.25                 3.83             19060.32                  2.32                  2.38                     9.33                    1.25                  1.32                  1.26                  1.31


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.15                 0.14                 0.09                 0.10                 3.57                  0.12                  0.13                     6.60                    0.09                  0.07                  0.10                  0.10
# stl_vector_benchmarks
                 0.00                 4.52                 5.46                37.16                10.76               563.68                  5.54                  5.99                     0.00                    2.10                  1.87                  2.28                  2.10
# stl_vector_benchmarks_memtracked
                32.00                 4.67                 7.49                50.89                14.45               531.60                  6.46                  7.92                     0.00                    2.10                  1.91                  2.29                  2.11
# stl_hashset_benchmarks_memtracked
               195.02                28.46               171.37                58.86                74.43                54.46                132.94                215.33                    12.63                    6.66                  6.66                 29.41                  6.66
# stl_hashset_benchmarks
                 0.00                28.32               171.25                59.15                73.73                55.61                133.48                214.87                    12.58                    6.71                  6.73                 29.46                  6.74
# bitmagic_benchmarks
                 7.99                 0.31                 0.36                 0.18                 0.22                16.25                  0.33                  0.35                    10.69                    0.11                  0.14                  0.13                  0.14
# bitmagic_benchmarks -r
                 1.89                 0.25                 0.27                 0.19                 0.23                62.28                  0.26                  0.27                     6.82                    0.12                  0.20                  0.19                  0.22
# slow_roaring_benchmarks -r
                 0.60                 0.17                 0.31                 0.12                 0.03                94.64                  0.26                  0.38                     5.33                    0.13                  0.14                  0.13                  0.13
# malloced_roaring_benchmarks -r
                 0.62                 0.27                 0.42                 0.11                 0.04                95.42                  0.35                  0.47                     5.34                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks -r
                 0.60                 0.17                 0.30                 0.11                 0.03                90.86                  0.25                  0.36                     5.33                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks -c -r
                 0.60                 0.17                 0.30                 0.11                 0.03                96.77                  0.24                  0.35                     5.33                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks
                 2.99                 0.17                 0.28                 0.09                 0.54               126.52                  0.20                  0.32                     5.64                    0.08                  0.08                  0.08                  0.08
# roaring_benchmarks -c
                 2.99                 0.17                 0.28                 0.09                 0.54               128.76                  0.19                  0.31                     5.64                    0.08                  0.08                  0.08                  0.08
# ewah32_benchmarks
                 0.64                 0.28                 0.56                 0.16                 0.13               914.18                  0.41                  0.62                     6.10                    0.19                  0.29                  0.23                  0.30
# ewah64_benchmarks
                 0.90                 0.21                 0.40                 0.12                 0.10               656.42                  0.30                  0.43                     5.95                    0.14                  0.21                  0.17                  0.23
# wah32_benchmarks
                 0.65                 0.45                 0.65                 0.47                 0.47              3698.85                  0.54                  0.66                     7.63                    0.27                  0.28                  0.28                  0.28
# concise_benchmarks
                 0.55                 0.56                 0.76                 0.60                 0.62              3804.61                  0.65                  0.79                     7.86                    0.34                  0.39                  0.35                  0.38


# processing file  census1881
# bitset_benchmarks
               523.54                19.48                13.58                 9.72                 9.42                 2.50                 11.04                 13.21                    43.08                    6.34                  7.58                  8.26                  9.81
# stl_vector_benchmarks
                 0.00                 3.28                 6.05               542.15                65.62               163.52                  5.02                  6.39                     0.00                    1.40                  1.28                  1.63                  1.45
# stl_vector_benchmarks_memtracked
                32.00                 3.28                 8.09               706.92                73.89               167.69                  6.05                  8.48                     0.00                    1.46                  1.30                  1.63                  1.46
# stl_hashset_benchmarks_memtracked
               194.74                 0.48               260.42               995.99              1224.24                38.30                231.77                362.42                    66.99                    0.46                  0.22                 44.05                  0.44
# stl_hashset_benchmarks
                 0.00                 0.48               260.02              1002.17              1228.17                39.44                232.81                362.91                    66.98                    0.46                  0.22                 44.15                  0.47
# bitmagic_benchmarks
                98.93                 1.91                 6.77                 3.00                 4.24                10.93                  3.69                  6.64                    82.56                    0.24                  2.19                  1.42                  2.20
# bitmagic_benchmarks -r
                46.32                 1.07                 3.68                 2.17                 3.83                12.61                  2.55                  3.61                    69.69                    0.09                  1.17                  1.60                  1.11
# slow_roaring_benchmarks -r
                15.08                 0.12                 1.08                 4.04                10.43                29.36                  0.63                  0.98                     5.23                    0.09                  0.10                  0.09                  0.09
# malloced_roaring_benchmarks -r
                15.35                 0.31                 2.77                 2.69                10.25                29.45                  1.56                  2.67                     5.16                    0.08                  0.09                  0.08                  0.09
# roaring_benchmarks -r
                15.08                 0.12                 1.10                 2.50                 8.59                28.83                  0.62                  0.98                     5.18                    0.08                  0.10                  0.08                  0.09
# roaring_benchmarks -c -r
                15.08                 0.12                 0.50                 2.50                 8.45                31.22                  0.25                  0.36                     5.19                    0.08                  0.10                  0.09                  0.09
# roaring_benchmarks
                15.97                 0.11                 1.03                 2.67                 6.04                29.47                  0.63                  1.08                     5.16                    0.08                  0.08                  0.08                  0.08
# roaring_benchmarks -c
                15.97                 0.11                 0.40                 2.67                 6.33                31.76                  0.23                  0.39                     5.18                    0.08                  0.09                  0.08                  0.08
# ewah32_benchmarks
                33.77                 3.73                28.13               276.08                53.13              8530.09                 15.86                 27.64                    29.48                    3.71                 12.07                  8.05                 12.07
# ewah64_benchmarks
                43.77                 1.93                15.26               102.67                26.65              4849.75                  8.53                 14.76                    26.92                    1.86                  6.73                  4.20                  6.73
# wah32_benchmarks
                34.32                14.50                29.30               387.58                44.54             26767.28                 21.81                 28.90                    29.24                    9.05                 11.57                 10.76                 11.58
# concise_benchmarks
                25.56                20.78                33.09               401.07                57.46             29969.77                 26.62                 33.29                    31.68                   12.11                 16.86                 14.53                 16.90


# processing file  census1881_srt
# bitset_benchmarks
               888.07                32.83                23.45                16.05                15.96                 2.48                 19.29                 22.34                    41.16                   11.40                 12.51                 14.22                 16.24
# stl_vector_benchmarks
                 0.00                 3.95                 6.22               609.89                32.97               149.36                  5.78                  6.42                     0.00                    1.71                  1.57                  2.00                  1.77
# stl_vector_benchmarks_memtracked
                32.00                 3.96                 8.38               827.46                40.02               154.50                  6.78                  8.73                     0.00                    1.77                  1.62                  1.99                  1.78
# stl_hashset_benchmarks_memtracked
               194.93                 1.40               195.89               349.91               393.71                38.24                169.37                241.49                    20.58                    1.03                  0.84                 21.42                  0.75
# stl_hashset_benchmarks
                 0.00                 1.41               195.69               348.17               392.02                39.10                169.77                241.23                    20.36                    1.04                  0.84                 21.38                  0.78
# bitmagic_benchmarks
               249.26                 4.78                16.53                 7.14                 9.10                11.26                  9.15                 16.39                    31.91                    0.76                  5.53                  3.68                  5.52
# bitmagic_benchmarks -r
                16.00                 0.84                 2.47                 4.59                 3.62                13.57                  2.94                  2.58                    10.88                    0.12                  0.52                  3.19                  0.52
# slow_roaring_benchmarks -r
                 2.16                 0.13                 1.05                 3.27                 3.81                19.57                  0.55                  0.93                     6.33                    0.06                  0.15                  0.09                  0.12
# malloced_roaring_benchmarks -r
                 2.77                 0.53                 5.03                 3.58                 8.49                19.97                  2.78                  4.97                     6.34                    0.06                  0.14                  0.09                  0.13
# roaring_benchmarks -r
                 2.16                 0.13                 1.07                 3.21                 3.82                19.55                  0.57                  0.96                     6.33                    0.06                  0.14                  0.09                  0.13
# roaring_benchmarks -c -r
                 2.16                 0.13                 0.56                 3.29                 3.91                20.58                  0.31                  0.42                     6.35                    0.06                  0.16                  0.10                  0.14
# roaring_benchmarks
                 6.09                 0.14                 1.18                 3.94                 8.25                28.31                  0.70                  1.13                     5.57                    0.08                  0.09                  0.08                  0.08
# roaring_benchmarks -c
                 6.09                 0.15                 0.49                 4.10                 8.31                30.02                  0.30                  0.37                     5.59                    0.08                  0.10                  0.09                  0.09
# ewah32_benchmarks
                 2.91                 0.49                 2.50               171.78                 5.21               541.58                  1.46                  2.46                     8.75                    0.44                  1.07                  0.72                  1.07
# ewah64_benchmarks
                 4.54                 0.39                 2.15               129.35                 4.62               442.70                  1.25                  2.09                     8.16                    0.32                  0.96                  0.59                  0.95
# wah32_benchmarks
                 2.95                 1.61                 3.05               197.71                 5.71              1815.34                  2.30                  2.96                     9.96                    1.03                  1.27                  1.05                  1.27
# concise_benchmarks
                 2.48                 1.94                 3.37               224.17                 6.57              1888.59                  2.60                  3.37                    10.16                    1.24                  1.65                  1.27                  1.65


# processing file  weather_sept_85
# bitset_benchmarks
                15.26                 0.39                 0.37                 0.33                 0.34                 3.51                  0.36                  0.37                    10.09                    0.27                  0.22                  0.28                  0.28
# stl_vector_benchmarks
                 0.00                 7.36                 7.66                92.97                27.76               565.46                  8.31                  8.69                     0.00                    4.54                  4.36                  5.09                  4.51
# stl_vector_benchmarks_memtracked
                32.00                 7.41                 9.97               119.26                31.91               573.58                  9.24                 10.73                     0.00                    4.62                  4.39                  5.06                  4.60
# stl_hashset_benchmarks_memtracked
               194.93                35.29               249.40               235.82               255.10                61.21                245.26                357.22                    40.68                   17.45                 17.45                 60.72                 17.49
# stl_hashset_benchmarks
                 0.00                35.28               248.02               234.90               254.06                61.71                245.33                354.91                    40.85                   17.50                 17.51                 61.05                 17.54
# bitmagic_benchmarks
                14.80                 0.59                 0.67                 0.37                 0.44                15.91                  0.61                  0.66                    33.26                    0.24                  0.28                  0.27                  0.28
# bitmagic_benchmarks -r
                 8.79                 0.59                 0.59                 0.28                 0.36                30.48                  0.55                  0.59                    31.69                    0.21                  0.57                  0.44                  0.60
# slow_roaring_benchmarks -r
                 5.38                 0.98                 0.88                 0.24                 1.37               149.79                  1.12                  1.05                     6.42                    0.88                  0.88                  0.87                  0.88
# malloced_roaring_benchmarks -r
                 5.42                 0.56                 0.73                 0.16                 1.29               148.88                  0.62                  0.91                     6.44                    0.26                  0.27                  0.26                  0.27
# roaring_benchmarks -r
                 5.38                 0.40                 0.57                 0.16                 1.06               152.47                  0.45                  0.76                     6.43                    0.27                  0.27                  0.26                  0.27
# roaring_benchmarks -c -r
                 5.38                 0.40                 0.57                 0.16                 1.06               158.77                  0.44                  0.75                     6.43                    0.27                  0.27                  0.26                  0.27
# roaring_benchmarks
                 5.44                 0.38                 0.54                 0.15                 1.03               153.49                  0.43                  0.75                     6.43                    0.25                  0.25                  0.25                  0.25
# roaring_benchmarks -c
                 5.44                 0.38                 0.54                 0.16                 1.03               161.39                  0.42                  0.73                     6.43                    0.25                  0.25                  0.25                  0.25
# ewah32_benchmarks
                 6.67                 2.63                 4.45                 2.34                 4.60             13302.34                  3.60                  4.37                    15.85                    1.64                  2.49                  2.05                  2.52
# ewah64_benchmarks
                 7.87                 1.56                 2.26                 1.29                 2.31              7357.76                  1.90                  2.21                    11.52                    0.84                  1.27                  1.02                  1.29
# wah32_benchmarks
                 6.82                 3.59                 4.60                 4.93                 6.84             65057.21                  4.28                  4.50                    12.40                    2.09                  2.10                  2.14                  2.10
# concise_benchmarks
                 5.88                 4.52                 5.41                 6.01                 8.07             68908.94                  5.11                  5.41                    12.81                    2.75                  2.92                  2.77                  2.89


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.41                 0.31                 0.26                 0.27                 3.43                  0.29                  0.30                     6.80                    0.19                  0.17                  0.21                  0.21
# stl_vector_benchmarks
                 0.00                 4.36                 6.06                68.23                13.89               671.17                  5.79                  6.26                     0.00                    1.97                  1.75                  2.18                  1.97
# stl_vector_benchmarks_memtracked
                32.00                 4.41                 7.95                84.26                17.87               642.37                  6.67                  8.30                     0.00                    1.97                  1.78                  2.19                  1.98
# stl_hashset_benchmarks_memtracked
               195.07                15.13               183.00                81.78               111.60                60.02                149.15                239.26                    12.93                    7.01                  7.01                 32.22                  7.02
# stl_hashset_benchmarks
                 0.00                15.05               177.54                81.67               111.04                61.52                148.93                235.69                    12.89                    7.13                  7.13                 32.27                  7.14
# bitmagic_benchmarks
                 9.03                 0.32                 0.45                 0.23                 0.27                14.77                  0.36                  0.45                    10.14                    0.12                  0.18                  0.15                  0.18
# bitmagic_benchmarks -r
                 0.96                 0.16                 0.20                 0.21                 0.33                53.05                  0.22                  0.22                     6.84                    0.09                  0.14                  0.17                  0.14
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                68.83                  0.15                  0.22                     5.25                    0.06                  0.07                  0.06                  0.07
# malloced_roaring_benchmarks -r
                 0.36                 0.17                 0.30                 0.04                 0.10                73.13                  0.25                  0.35                     5.26                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                80.67                  0.15                  0.22                     5.25                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -c -r
                 0.34                 0.09                 0.16                 0.04                 0.05                93.18                  0.14                  0.20                     5.26                    0.06                  0.07                  0.06                  0.07
# roaring_benchmarks
                 3.24                 0.12                 0.24                 0.07                 0.49               112.01                  0.18                  0.28                     5.68                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -c
                 3.24                 0.12                 0.23                 0.07                 0.49               123.76                  0.16                  0.25                     5.68                    0.07                  0.07                  0.06                  0.07
# ewah32_benchmarks
                 0.54                 0.19                 0.48                 0.32                 0.11              2177.42                  0.34                  0.51                     5.83                    0.15                  0.23                  0.19                  0.24
# ewah64_benchmarks
                 0.86                 0.16                 0.41                 0.25                 0.09              1819.59                  0.28                  0.42                     5.82                    0.12                  0.20                  0.16                  0.21
# wah32_benchmarks
                 0.54                 0.34                 0.56                 0.58                 0.37              7508.25                  0.45                  0.56                     7.46                    0.22                  0.24                  0.23                  0.24
# concise_benchmarks
                 0.43                 0.41                 0.62                 0.69                 0.45              7338.71                  0.51                  0.63                     7.70                    0.26                  0.30                  0.27                  0.30


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                26.18                20.18                14.82                15.46                 3.83                 17.72                 19.75                    46.81                   11.72                 11.75                 13.50                 14.95
# stl_vector_benchmarks
                 0.00                 4.16                 6.95               513.92                50.98               166.43                  6.04                  7.41                     0.00                    2.20                  2.35                  2.53                  2.23
# stl_vector_benchmarks_memtracked
                32.00                 4.10                 9.23               691.95                61.55               158.50                  7.09                  9.68                     0.00                    2.33                  2.21                  2.55                  2.23
# stl_hashset_benchmarks_memtracked
               194.68                14.96               228.67               407.16               506.92                52.09                211.78                300.67                    24.73                   12.64                 10.54                 40.60                 11.23
# stl_hashset_benchmarks
                 0.00                14.86               228.66               410.82               514.07                51.87                214.02                301.80                    25.65                   12.76                 10.77                 40.48                 11.49
# bitmagic_benchmarks
               462.53                13.76                25.68                11.96                14.53                12.86                 18.36                 25.85                    59.09                    4.53                  9.46                  7.43                  9.47
# bitmagic_benchmarks -r
                29.84                 3.06                 5.77                11.84                16.64                23.00                  5.99                  5.62                    12.48                    1.41                  2.22                  5.24                  2.20
# slow_roaring_benchmarks -r
                 5.89                 1.50                 4.01                 2.89                24.38                29.46                  2.66                  3.83                     9.49                    0.84                  1.27                  1.05                  1.24
# malloced_roaring_benchmarks -r
                 7.04                 4.53                10.91                 3.15                30.66                30.80                  7.76                 11.10                     9.65                    0.86                  1.32                  1.09                  1.28
# roaring_benchmarks -r
                 5.89                 1.53                 4.05                 2.81                24.26                32.57                  2.71                  3.93                     9.52                    0.86                  1.33                  1.08                  1.28
# roaring_benchmarks -c -r
                 5.89                 1.51                 3.51                 2.99                24.47                36.60                  2.36                  3.24                     9.60                    0.89                  1.42                  1.15                  1.38
# roaring_benchmarks
                16.49                 1.44                 4.40                 4.27                20.53                70.82                  2.44                  4.65                     5.38                    0.75                  0.76                  0.74                  0.74
# roaring_benchmarks -c
                16.49                 1.44                 3.70                 4.34                20.73                81.89                  1.86                  3.65                     5.45                    0.78                  0.83                  0.80                  0.81
# ewah32_benchmarks
                10.83                 2.85                 9.18               328.54                48.69               994.34                  5.98                  8.98                    18.28                    2.46                  4.06                  3.28                  4.07
# ewah64_benchmarks
                19.41                 2.74                 8.78               202.76                41.03               990.10                  5.57                  8.42                    15.85                    2.31                  3.98                  2.95                  3.96
# wah32_benchmarks
                10.89                 6.08                10.87               416.38                46.49              2406.46                  8.25                 10.69                    16.04                    3.86                  4.46                  4.13                  4.47
# concise_benchmarks
                10.25                 6.41                11.31               450.57                52.33              2351.00                  8.48                 11.36                    16.68                    4.07                  4.88                  4.29                  4.86


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.37                18.05                13.95                14.66                 2.89                 14.84                 17.20                    32.86                    8.78                 10.34                 11.02                 12.84
# stl_vector_benchmarks
                 0.00                 4.16                 7.14               496.96                42.77               162.51                  5.75                  6.68                     0.00                    1.69                  1.56                  1.90                  1.70
# stl_vector_benchmarks_memtracked
                32.00                 4.25                 9.67               679.23                53.13               148.42                  6.78                  9.09                     0.00                    1.73                  1.56                  1.87                  1.70
# stl_hashset_benchmarks_memtracked
               195.18                26.51               179.13               267.68               353.63                51.28                159.21                229.18                    13.28                   10.10                  8.99                 25.81                  8.47
# stl_hashset_benchmarks
                 0.00                26.04               178.96               263.31               353.97                49.31                159.54                227.77                    13.21                   10.39                  9.34                 25.71                  8.80
# bitmagic_benchmarks
               370.07                10.07                21.08                 9.48                11.85                12.41                 14.16                 20.80                    39.00                    2.96                  7.66                  5.72                  7.62
# bitmagic_benchmarks -r
                23.99                 1.92                 4.21                 8.87                 6.98                15.92                  4.45                  4.24                     7.97                    0.50                  1.16                  3.63                  1.14
# slow_roaring_benchmarks -r
                 1.63                 0.67                 2.26                 1.89                 7.24                22.20                  1.28                  1.85                     5.85                    0.25                  0.46                  0.35                  0.44
# malloced_roaring_benchmarks -r
                 2.58                 2.62                 7.92                 2.04                12.63                22.82                  5.04                  7.65                     5.92                    0.25                  0.49                  0.36                  0.46
# roaring_benchmarks -r
                 1.63                 0.68                 2.32                 1.80                 7.52                22.51                  1.31                  1.98                     5.86                    0.26                  0.49                  0.37                  0.47
# roaring_benchmarks -c -r
                 1.63                 0.67                 1.95                 1.89                 7.47                26.56                  1.06                  1.47                     5.91                    0.28                  0.55                  0.41                  0.52
# roaring_benchmarks
                10.67                 0.75                 2.95                 3.08                13.14                45.50                  1.56                  3.08                     5.72                    0.35                  0.36                  0.33                  0.34
# roaring_benchmarks -c
                10.67                 0.75                 2.28                 3.10                13.32                50.69                  1.10                  2.13                     5.80                    0.37                  0.40                  0.37                  0.38
# ewah32_benchmarks
                 2.63                 0.73                 2.37               116.49                12.60               241.30                  1.59                  2.33                     8.20                    0.55                  0.99                  0.79                  1.00
# ewah64_benchmarks
                 4.66                 0.72                 2.21                92.85                11.43               234.48                  1.49                  2.17                     8.35                    0.51                  0.92                  0.69                  0.93
# wah32_benchmarks
                 2.67                 1.28                 2.57               132.78                12.40               525.12                  1.92                  2.50                     8.94                    0.77                  1.02                  0.96                  1.02
# concise_benchmarks
                 2.23                 1.39                 2.63               148.81                13.74               497.89                  2.00                  2.64                     9.32                    0.83                  1.16                  1.04                  1.15
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
# bitmagic_benchmarks
               100.05                 8.97                 9.21                 4.80                 5.37                15.33                  9.17
       9.20                    78.84                    2.61                  2.95                  2.92                  2.95
# bitmagic_benchmarks -r
                38.52                 8.80                10.10                 3.94                 5.32                65.32                  8.25
      10.20                    46.55                    5.64                  9.08                  7.50                  9.29
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

