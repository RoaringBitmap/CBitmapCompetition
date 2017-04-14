# CBitmapCompetition

## Description
The goal of this project is to compare on realistic data different C and C++ techniques representing sets of integers.

We start from a collection of sets of sorted integers.
For example, we might have 200 sets.
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
./scripts/all.sh
# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value), successive differences, successive symmetric differences, iterations through all values, then we have pairwise count aggregates for successive intersections, successive unions, successive differences, successive symmetric differences
# processing file  census-income
# bitset_benchmarks
                 5.66                 0.14                 0.13                 0.09                 0.09                 3.67                  0.12                  0.12                     7.54                    0.09                  0.07                  0.09                  0.09
# stl_vector_benchmarks
                 0.00                 7.67                 7.74                43.71                16.72               419.50                  8.37                  8.67                     0.00                    4.86                  4.58                  5.20                  4.83
# stl_vector_benchmarks_memtracked
                32.00                 7.95                 9.76                55.76                20.10               426.18                  9.31                 10.48                     0.00                    4.95                  4.63                  5.20                  4.95
# stl_hashset_benchmarks_memtracked
               218.55                49.17               188.39                88.21               119.66                47.56                159.36                267.20                    23.08                   10.92                 10.98                 39.46                 11.00
# stl_hashset_benchmarks
                 0.00                49.21               187.94                86.34               115.61                47.61                159.83                268.15                    23.02                   10.88                 10.88                 39.29                 10.90
# bitmagic_benchmarks
                 8.23                 0.43                 0.43                 0.18                 0.23                14.38                  0.43                  0.44                    21.58                    0.17                  0.22                  0.21                  0.21
# bitmagic_benchmarks -r
                 6.83                 0.47                 0.43                 0.16                 0.22                34.72                  0.43                  0.45                    20.55                    0.16                  0.39                  0.33                  0.42
# slow_roaring_benchmarks -r
                 2.60                 0.45                 0.42                 0.06                 0.61                61.82                  0.53                  0.54                     6.78                    0.35                  0.35                  0.35                  0.35
# malloced_roaring_benchmarks -r
                 2.62                 0.33                 0.36                 0.05                 0.54                60.32                  0.38                  0.46                     6.72                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks -r
                 2.60                 0.23                 0.27                 0.05                 0.46                64.09                  0.28                  0.37                     5.89                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks -c -r
                 2.60                 0.24                 0.28                 0.05                 0.46                67.58                  0.28                  0.37                     5.88                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks
                 2.74                 0.20                 0.25                 0.08                 0.46                64.93                  0.22                  0.32                     5.84                    0.10                  0.10                  0.10                  0.10
# roaring_benchmarks -c
                 2.74                 0.20                 0.25                 0.08                 0.46                67.15                  0.22                  0.32                     5.84                    0.10                  0.10                  0.10                  0.10
# ewah32_benchmarks
                 3.29                 1.44                 2.01                 0.50                 1.57              3254.39                  1.69                  1.97                    13.11                    0.80                  1.11                  0.95                  1.13
# ewah64_benchmarks
                 3.86                 0.84                 1.06                 0.27                 1.15              1753.87                  0.95                  1.04                     9.10                    0.43                  0.60                  0.50                  0.60
# wah32_benchmarks
                 3.37                 1.81                 2.11                 1.83                 3.35             19330.04                  1.99                  2.04                     9.23                    0.98                  0.99                  1.00                  0.99
# concise_benchmarks
                 2.94                 2.13                 2.45                 2.19                 3.80             19050.89                  2.29                  2.39                     9.35                    1.24                  1.31                  1.25                  1.30


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.14                 0.14                 0.09                 0.10                 3.81                  0.12                  0.13                     6.72                    0.09                  0.07                  0.10                  0.10
# stl_vector_benchmarks
                 0.00                 4.51                 5.46                37.26                10.79               574.22                  5.40                  5.97                     0.00                    2.11                  1.89                  2.30                  2.10
# stl_vector_benchmarks_memtracked
                32.00                 4.67                 7.50                50.86                14.43               542.56                  6.46                  7.94                     0.00                    2.12                  1.90                  2.29                  2.13
# stl_hashset_benchmarks_memtracked
               217.89                27.01               163.11                58.68                72.01                46.88                125.67                216.64                    12.61                    5.88                  5.88                 26.61                  5.87
# stl_hashset_benchmarks
                 0.00                27.15               162.65                59.28                72.18                48.51                126.54                218.59                    12.64                    5.88                  5.88                 26.56                  5.88
# bitmagic_benchmarks
                 8.52                 0.44                 0.47                 0.19                 0.24                14.57                  0.45                  0.47                    10.52                    0.16                  0.22                  0.21                  0.22
# bitmagic_benchmarks -r
                 4.57                 0.32                 0.33                 0.19                 0.25                51.28                  0.34                  0.36                     6.88                    0.12                  0.20                  0.21                  0.22
# slow_roaring_benchmarks -r
                 0.60                 0.18                 0.31                 0.12                 0.03                80.16                  0.26                  0.38                     5.35                    0.12                  0.13                  0.13                  0.13
# malloced_roaring_benchmarks -r
                 0.62                 0.27                 0.42                 0.11                 0.04                67.21                  0.35                  0.47                     5.35                    0.12                  0.12                  0.12                  0.12
# roaring_benchmarks -r
                 0.60                 0.17                 0.30                 0.11                 0.03                65.11                  0.25                  0.36                     5.33                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks -c -r
                 0.60                 0.17                 0.30                 0.11                 0.03                69.97                  0.24                  0.35                     5.33                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks
                 2.99                 0.17                 0.27                 0.09                 0.53                74.20                  0.20                  0.32                     5.64                    0.08                  0.08                  0.08                  0.08
# roaring_benchmarks -c
                 2.99                 0.17                 0.29                 0.09                 0.54                79.53                  0.19                  0.31                     5.64                    0.08                  0.08                  0.08                  0.08
# ewah32_benchmarks
                 0.64                 0.28                 0.56                 0.16                 0.13               947.01                  0.41                  0.63                     6.07                    0.19                  0.29                  0.24                  0.30
# ewah64_benchmarks
                 0.90                 0.21                 0.39                 0.11                 0.10               658.09                  0.29                  0.43                     5.95                    0.14                  0.21                  0.17                  0.23
# wah32_benchmarks
                 0.65                 0.45                 0.64                 0.47                 0.47              3713.62                  0.55                  0.67                     7.63                    0.26                  0.29                  0.28                  0.29
# concise_benchmarks
                 0.55                 0.55                 0.76                 0.58                 0.61              3788.77                  0.65                  0.79                     7.88                    0.33                  0.38                  0.35                  0.38


# processing file  census1881
# bitset_benchmarks
               523.54                19.35                13.61                 9.75                 9.50                 2.33                 11.07                 13.23                    43.06                    6.34                  7.57                  8.27                  9.76
# stl_vector_benchmarks
                 0.00                 3.28                 6.11               539.70                66.02               170.05                  4.89                  6.37                     0.00                    1.46                  1.27                  1.63                  1.45
# stl_vector_benchmarks_memtracked
                32.00                 3.28                 8.08               712.06                71.93               165.30                  6.07                  8.49                     0.00                    1.42                  1.27                  1.63                  1.45
# stl_hashset_benchmarks_memtracked
               212.26                 0.42               288.10               997.93              1204.73                37.92                216.25                376.23                    56.69                    0.40                  0.19                 39.76                  0.41
# stl_hashset_benchmarks
                 0.00                 0.42               287.57               995.43              1199.09                38.30                217.05                378.54                    54.77                    0.38                  0.18                 38.80                  0.38
# bitmagic_benchmarks
               102.18                 2.65                 8.35                 3.02                 4.85                10.07                  4.79                  8.35                    83.45                    0.30                  3.34                  1.96                  3.35
# bitmagic_benchmarks -r
                62.63                 1.88                 4.43                 2.20                 3.91                11.50                  3.21                  4.53                    70.43                    0.10                  1.61                  2.28                  1.58
# slow_roaring_benchmarks -r
                15.08                 0.12                 1.08                 4.47                10.90                16.40                  0.63                  0.99                     6.11                    0.09                  0.10                  0.09                  0.09
# malloced_roaring_benchmarks -r
                15.35                 0.32                 2.78                 2.67                10.30                16.95                  1.56                  2.66                     6.09                    0.08                  0.10                  0.08                  0.09
# roaring_benchmarks -r
                15.08                 0.12                 1.10                 2.50                 8.54                16.68                  0.61                  0.99                     5.16                    0.08                  0.09                  0.08                  0.09
# roaring_benchmarks -c -r
                15.08                 0.12                 0.51                 2.50                 8.48                18.73                  0.25                  0.36                     5.19                    0.08                  0.10                  0.08                  0.09
# roaring_benchmarks
                15.97                 0.11                 1.03                 2.67                 6.04                17.72                  0.63                  1.09                     5.14                    0.08                  0.08                  0.08                  0.08
# roaring_benchmarks -c
                15.97                 0.11                 0.41                 2.67                 6.31                19.27                  0.23                  0.34                     5.19                    0.08                  0.09                  0.08                  0.08
# ewah32_benchmarks
                33.77                 3.73                27.57               269.42                52.44              8826.72                 15.68                 27.64                    29.49                    3.71                 12.30                  8.11                 12.00
# ewah64_benchmarks
                43.77                 1.93                14.80               101.93                26.25              4886.84                  8.51                 14.75                    26.64                    1.81                  6.80                  4.17                  6.71
# wah32_benchmarks
                34.32                14.90                29.17               387.46                43.98             26729.20                 22.04                 29.19                    29.31                    8.67                 12.39                 10.97                 12.09
# concise_benchmarks
                25.56                20.33                33.82               398.91                57.11             29946.19                 26.49                 34.18                    32.03                   12.09                 16.84                 14.65                 16.80


# processing file  census1881_srt
# bitset_benchmarks
               888.07                32.71                23.53                16.11                16.00                 2.68                 19.30                 22.44                    41.17                   11.39                 12.52                 14.19                 16.20
# stl_vector_benchmarks
                 0.00                 3.95                 6.21               617.31                33.59               155.90                  5.61                  6.37                     0.00                    1.77                  1.60                  1.99                  1.77
# stl_vector_benchmarks_memtracked
                32.00                 3.98                 8.37               834.43                41.11               147.94                  6.83                  8.73                     0.00                    1.71                  1.56                  1.99                  1.77
# stl_hashset_benchmarks_memtracked
               216.84                 1.13               202.46               336.22               378.03                37.63                166.27                247.35                    17.52                    0.89                  0.71                 20.62                  0.69
# stl_hashset_benchmarks
                 0.00                 1.13               199.39               335.20               377.75                37.01                166.27                247.03                    17.42                    0.88                  0.73                 20.52                  0.67
# bitmagic_benchmarks
               254.06                 5.90                20.17                 7.27                 9.97                10.51                 11.52                 20.42                    31.98                    0.93                  8.36                  5.03                  8.37
# bitmagic_benchmarks -r
                40.05                 1.64                 2.98                 4.59                 3.69                11.26                  3.52                  3.11                    11.98                    0.12                  0.55                  4.79                  0.54
# slow_roaring_benchmarks -r
                 2.16                 0.13                 1.06                 3.28                 3.80                18.30                  0.56                  0.93                     6.38                    0.06                  0.15                  0.09                  0.12
# malloced_roaring_benchmarks -r
                 2.77                 0.53                 5.00                 3.59                 8.46                19.34                  2.72                  4.91                     6.36                    0.06                  0.14                  0.09                  0.13
# roaring_benchmarks -r
                 2.16                 0.13                 1.08                 3.21                 3.82                18.28                  0.56                  0.95                     6.32                    0.06                  0.14                  0.09                  0.13
# roaring_benchmarks -c -r
                 2.16                 0.13                 0.56                 3.29                 3.92                21.03                  0.31                  0.42                     6.35                    0.06                  0.16                  0.10                  0.14
# roaring_benchmarks
                 6.09                 0.14                 1.18                 3.94                 8.24                19.46                  0.69                  1.13                     5.58                    0.08                  0.09                  0.08                  0.08
# roaring_benchmarks -c
                 6.09                 0.14                 0.49                 4.12                 8.30                22.83                  0.30                  0.39                     5.60                    0.08                  0.10                  0.09                  0.09
# ewah32_benchmarks
                 2.91                 0.50                 2.48               168.06                 5.12               561.27                  1.46                  2.48                     9.05                    0.44                  1.06                  0.73                  1.06
# ewah64_benchmarks
                 4.54                 0.40                 2.08               125.16                 4.39               438.79                  1.22                  2.09                     8.16                    0.32                  0.96                  0.59                  0.96
# wah32_benchmarks
                 2.95                 1.61                 3.03               196.83                 5.67              1809.85                  2.32                  2.99                     9.94                    0.99                  1.32                  1.05                  1.30
# concise_benchmarks
                 2.48                 1.89                 3.42               221.65                 6.62              1888.86                  2.56                  3.41                    10.15                    1.22                  1.65                  1.29                  1.66


# processing file  weather_sept_85
# bitset_benchmarks
                15.26                 0.39                 0.37                 0.33                 0.35                 3.64                  0.37                  0.37                    10.08                    0.27                  0.22                  0.28                  0.28
# stl_vector_benchmarks
                 0.00                 7.23                 7.78                95.15                27.86               570.57                  8.26                  8.68                     0.00                    4.57                  4.39                  5.10                  4.54
# stl_vector_benchmarks_memtracked
                32.00                 7.40                 9.68               115.67                31.39               554.21                  9.15                 10.44                     0.00                    4.65                  4.39                  5.06                  4.62
# stl_hashset_benchmarks_memtracked
               219.92                30.21               252.12               213.07               240.54                53.09                209.60                381.14                    34.08                   13.84                 13.85                 49.89                 13.87
# stl_hashset_benchmarks
                 0.00                30.09               251.81               212.00               239.19                53.70                210.50                383.18                    33.95                   13.80                 13.80                 49.66                 13.79
# bitmagic_benchmarks
                15.06                 0.70                 0.79                 0.37                 0.46                13.68                  0.74                  0.80                    33.14                    0.32                  0.42                  0.39                  0.42
# bitmagic_benchmarks -r
                10.06                 0.67                 0.69                 0.28                 0.39                26.65                  0.65                  0.71                    31.76                    0.23                  0.62                  0.54                  0.65
# slow_roaring_benchmarks -r
                 5.38                 0.99                 0.89                 0.27                 1.44                71.06                  1.11                  1.06                     7.28                    0.86                  0.87                  0.87                  0.86
# malloced_roaring_benchmarks -r
                 5.42                 0.56                 0.73                 0.16                 1.29                72.11                  0.62                  0.91                     7.20                    0.26                  0.27                  0.27                  0.27
# roaring_benchmarks -r
                 5.38                 0.40                 0.57                 0.16                 1.05                80.57                  0.45                  0.76                     6.43                    0.26                  0.27                  0.26                  0.27
# roaring_benchmarks -c -r
                 5.38                 0.40                 0.56                 0.16                 1.06                87.60                  0.44                  0.75                     6.43                    0.26                  0.27                  0.26                  0.27
# roaring_benchmarks
                 5.44                 0.38                 0.54                 0.15                 1.02                77.26                  0.43                  0.75                     6.42                    0.25                  0.25                  0.25                  0.25
# roaring_benchmarks -c
                 5.44                 0.38                 0.54                 0.16                 1.03                89.91                  0.42                  0.73                     6.42                    0.25                  0.26                  0.25                  0.25
# ewah32_benchmarks
                 6.67                 2.69                 4.40                 2.34                 4.52             13745.76                  3.55                  4.40                    15.89                    1.63                  2.50                  2.06                  2.51
# ewah64_benchmarks
                 7.87                 1.55                 2.23                 1.28                 2.27              7381.55                  1.88                  2.21                    11.39                    0.84                  1.27                  1.02                  1.28
# wah32_benchmarks
                 6.82                 3.61                 4.64                 4.93                 6.78             65073.09                  4.31                  4.53                    12.43                    2.06                  2.13                  2.14                  2.13
# concise_benchmarks
                 5.88                 4.48                 5.44                 5.88                 8.00             68978.34                  5.09                  5.43                    12.77                    2.73                  2.88                  2.76                  2.88


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.31                 0.29                 0.24                 0.26                 3.64                  0.28                  0.29                     6.73                    0.19                  0.17                  0.21                  0.21
# stl_vector_benchmarks
                 0.00                 4.36                 5.88                66.39                13.94               668.81                  5.61                  6.18                     0.00                    1.97                  1.75                  2.19                  1.97
# stl_vector_benchmarks_memtracked
                32.00                 4.41                 8.02                84.52                17.87               655.77                  6.66                  8.30                     0.00                    1.98                  1.78                  2.19                  1.98
# stl_hashset_benchmarks_memtracked
               219.63                13.71               185.34                72.85               107.27                47.82                140.52                242.19                    12.61                    5.76                  5.76                 27.07                  5.77
# stl_hashset_benchmarks
                 0.00                13.75               184.94                73.75               107.93                47.24                141.88                244.78                    12.62                    5.78                  5.78                 27.04                  5.78
# bitmagic_benchmarks
                 9.23                 0.39                 0.54                 0.23                 0.29                13.30                  0.45                  0.56                     9.94                    0.16                  0.27                  0.23                  0.27
# bitmagic_benchmarks -r
                 1.98                 0.19                 0.22                 0.21                 0.33                42.68                  0.24                  0.25                     7.00                    0.09                  0.14                  0.20                  0.15
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.18                 0.04                 0.05                69.60                  0.15                  0.22                     5.26                    0.06                  0.07                  0.06                  0.07
# malloced_roaring_benchmarks -r
                 0.36                 0.17                 0.30                 0.04                 0.10                58.15                  0.25                  0.35                     5.26                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                53.76                  0.15                  0.22                     5.25                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -c -r
                 0.34                 0.09                 0.16                 0.04                 0.05                65.13                  0.14                  0.20                     5.26                    0.06                  0.07                  0.06                  0.07
# roaring_benchmarks
                 3.24                 0.12                 0.24                 0.07                 0.48                57.81                  0.18                  0.28                     5.68                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -c
                 3.24                 0.12                 0.23                 0.07                 0.49                72.16                  0.16                  0.25                     5.68                    0.07                  0.07                  0.07                  0.07
# ewah32_benchmarks
                 0.54                 0.19                 0.47                 0.32                 0.11              2266.94                  0.34                  0.51                     5.88                    0.14                  0.23                  0.19                  0.24
# ewah64_benchmarks
                 0.86                 0.16                 0.39                 0.24                 0.09              1815.40                  0.28                  0.42                     5.81                    0.12                  0.20                  0.16                  0.21
# wah32_benchmarks
                 0.54                 0.35                 0.55                 0.58                 0.37              7497.58                  0.45                  0.57                     7.46                    0.21                  0.25                  0.23                  0.25
# concise_benchmarks
                 0.43                 0.41                 0.62                 0.69                 0.44              7326.76                  0.51                  0.64                     7.68                    0.26                  0.30                  0.27                  0.30


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                25.89                20.41                14.86                15.46                 3.85                 17.77                 19.74                    46.14                   11.70                 11.75                 13.45                 14.89
# stl_vector_benchmarks
                 0.00                 4.16                 6.97               515.92                51.06               158.52                  5.79                  7.30                     0.00                    2.20                  2.07                  2.52                  2.20
# stl_vector_benchmarks_memtracked
                32.00                 4.11                 9.21               690.95                61.57               161.64                  7.07                  9.67                     0.00                    2.33                  2.18                  2.54                  2.23
# stl_hashset_benchmarks_memtracked
               226.35                12.40               225.96               377.95               495.90                43.86                194.14                309.10                    19.82                   10.18                  8.33                 35.39                  8.83
# stl_hashset_benchmarks
                 0.00                12.41               224.69               377.05               496.94                44.97                194.92                310.41                    19.99                   10.06                  8.49                 35.40                  8.86
# bitmagic_benchmarks
               474.39                24.56                59.57                11.95                16.35                11.19                 22.76                 31.74                    59.59                    5.93                 14.37                 10.79                 14.36
# bitmagic_benchmarks -r
                89.29                 4.60                 6.62                12.18                16.79                20.39                  7.66                  7.33                    12.95                    1.42                  2.19                  7.04                  2.22
# slow_roaring_benchmarks -r
                 5.89                 1.50                 4.05                 2.88                24.39                34.48                  2.70                  3.92                     9.59                    0.87                  1.32                  1.08                  1.27
# malloced_roaring_benchmarks -r
                 7.04                 4.52                10.91                 3.14                30.78                31.04                  7.79                 11.19                     9.58                    0.86                  1.29                  1.09                  1.28
# roaring_benchmarks -r
                 5.89                 1.50                 4.04                 2.80                24.27                27.31                  2.71                  3.93                     9.54                    0.85                  1.28                  1.07                  1.27
# roaring_benchmarks -c -r
                 5.89                 1.50                 3.51                 2.88                24.40                34.66                  2.40                  3.24                     9.65                    0.89                  1.38                  1.15                  1.36
# roaring_benchmarks
                16.49                 1.48                 4.38                 4.28                20.39                39.65                  2.46                  4.64                     5.38                    0.75                  0.77                  0.74                  0.75
# roaring_benchmarks -c
                16.49                 1.46                 3.68                 4.34                20.48                44.98                  1.86                  3.64                     5.44                    0.78                  0.82                  0.80                  0.81
# ewah32_benchmarks
                10.83                 2.92                 8.96               315.80                47.57              1029.84                  5.86                  8.95                    18.44                    2.46                  4.02                  3.26                  4.03
# ewah64_benchmarks
                19.41                 2.75                 8.42               196.82                39.50               986.73                  5.56                  8.40                    15.61                    2.32                  3.97                  2.94                  3.98
# wah32_benchmarks
                10.89                 6.13                10.94               411.77                46.07              2401.88                  8.31                 10.80                    16.08                    3.76                  4.59                  4.14                  4.54
# concise_benchmarks
                10.25                 6.32                11.29               445.13                52.32              2357.57                  8.39                 11.27                    16.78                    4.01                  4.86                  4.38                  4.86


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.04                18.09                14.17                14.66                 2.75                 14.84                 17.35                    33.05                    8.74                 10.41                 10.99                 12.86
# stl_vector_benchmarks
                 0.00                 4.15                 7.17               496.06                42.41               142.61                  5.57                  6.67                     0.00                    1.69                  1.50                  1.93                  1.69
# stl_vector_benchmarks_memtracked
                32.00                 4.23                 9.53               672.79                52.80               141.72                  6.75                  8.95                     0.00                    1.73                  1.55                  1.94                  1.70
# stl_hashset_benchmarks_memtracked
               228.86                23.99               173.53               265.40               352.06                43.08                154.16                227.00                    12.71                    9.41                  8.44                 23.97                  7.93
# stl_hashset_benchmarks
                 0.00                24.49               172.57               266.11               354.14                43.57                155.55                229.69                    12.68                    9.39                  8.39                 23.86                  7.88
# bitmagic_benchmarks
               381.41                12.94                26.16                 9.36                13.28                10.97                 18.08                 26.87                    39.11                    3.86                 11.71                  8.30                 11.76
# bitmagic_benchmarks -r
                80.83                 3.56                 5.09                 8.84                 7.19                14.60                  5.75                  5.59                     8.63                    0.52                  1.13                  5.28                  1.12
# slow_roaring_benchmarks -r
                 1.63                 0.66                 2.28                 1.90                 7.29                22.15                  1.29                  1.87                     5.88                    0.25                  0.47                  0.35                  0.45
# malloced_roaring_benchmarks -r
                 2.58                 2.60                 7.84                 2.03                12.67                20.36                  4.99                  7.55                     5.94                    0.25                  0.49                  0.36                  0.46
# roaring_benchmarks -r
                 1.63                 0.67                 2.30                 1.83                 7.26                20.45                  1.31                  1.98                     5.86                    0.25                  0.48                  0.36                  0.45
# roaring_benchmarks -c -r
                 1.63                 0.67                 1.94                 1.89                 7.56                25.12                  1.07                  1.49                     5.91                    0.27                  0.54                  0.40                  0.52
# roaring_benchmarks
                10.67                 0.76                 3.06                 3.06                13.15                25.85                  1.57                  3.09                     5.74                    0.34                  0.36                  0.33                  0.34
# roaring_benchmarks -c
                10.67                 0.76                 2.26                 3.09                13.30                30.64                  1.12                  2.13                     5.78                    0.37                  0.39                  0.37                  0.38
# ewah32_benchmarks
                 2.63                 0.74                 2.32               113.25                12.45               248.01                  1.58                  2.34                     8.14                    0.55                  1.00                  0.79                  0.99
# ewah64_benchmarks
                 4.66                 0.72                 2.13                89.62                10.76               233.75                  1.49                  2.18                     7.61                    0.50                  0.92                  0.69                  0.93
# wah32_benchmarks
                 2.67                 1.30                 2.58               133.07                12.27               531.05                  1.94                  2.55                     9.55                    0.74                  1.05                  0.95                  1.05
# concise_benchmarks
                 2.23                 1.39                 2.61               148.41                13.74               495.92                  1.99                  2.62                     9.25                    0.83                  1.16                  1.04                  1.18
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

