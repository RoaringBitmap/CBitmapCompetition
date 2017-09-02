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
./scripts/all.sh
# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value), successive differences, successive symmetric differences, iterations through all values, then we have pairwise count aggregates for successive intersections, successive unions, successive differences, successive symmetric differences
# processing file  census-income
# bitset_benchmarks
                 5.66                 0.14                 0.13                 0.09                 0.09                 3.68                  0.12                  0.12                     7.53                    0.09                  0.07                  0.09                  0.09
# stl_vector_benchmarks
                 0.00                 7.69                 7.74                43.68                16.74               411.45                  8.40                  8.67                     0.00                    4.86                  4.57                  5.20                  4.83
# stl_vector_benchmarks_memtracked
                32.00                 7.90                 9.82                55.75                20.04               419.91                  9.33                 10.49                     0.00                    4.95                  4.64                  5.20                  4.95
# stl_hashset_benchmarks_memtracked
               194.63                54.91               208.00                96.34               126.17                62.05                177.03                266.38                    28.73                   13.24                 13.28                 47.11                 13.28
# stl_hashset_benchmarks
                 0.00                54.66               205.44                96.50               124.72                60.11                174.52                263.51                    28.53                   13.07                 13.12                 47.15                 13.15
# bitmagic_benchmarks
                 7.76                 0.38                 0.38                 0.18                 0.21                12.86                  0.37                  0.38                    21.49                    0.19                  0.23                  0.21                  0.21
# bitmagic_benchmarks -r
                 4.46                 0.42                 0.39                 0.16                 0.20                32.65                  0.37                  0.39                    20.44                    0.16                  0.40                  0.32                  0.42
# slow_roaring_benchmarks -r
                 2.60                 0.45                 0.42                 0.06                 0.61                59.86                  0.53                  0.54                     6.78                    0.35                  0.35                  0.35                  0.35
# malloced_roaring_benchmarks -r
                 2.62                 0.33                 0.36                 0.05                 0.54                60.15                  0.38                  0.46                     6.72                    0.13                  0.14                  0.13                  0.13
# roaring_benchmarks -r
                 2.60                 0.23                 0.27                 0.05                 0.47                65.27                  0.28                  0.37                     5.87                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks -c -r
                 2.60                 0.23                 0.28                 0.05                 0.47                68.18                  0.28                  0.37                     5.91                    0.13                  0.13                  0.13                  0.13
# roaring_benchmarks
                 2.74                 0.20                 0.25                 0.08                 0.46                64.12                  0.22                  0.32                     5.86                    0.10                  0.10                  0.10                  0.10
# roaring_benchmarks -c
                 2.74                 0.20                 0.25                 0.08                 0.46                66.58                  0.22                  0.32                     5.86                    0.10                  0.10                  0.10                  0.10
# ewah32_benchmarks
                 3.29                 1.44                 2.01                 0.50                 1.56              3255.27                  1.69                  1.96                    13.08                    0.80                  1.11                  0.95                  1.13
# ewah64_benchmarks
                 3.86                 0.84                 1.06                 0.27                 1.15              1753.35                  0.95                  1.05                     9.10                    0.43                  0.60                  0.50                  0.61
# wah32_benchmarks
                 3.37                 1.81                 2.11                 1.83                 3.34             19327.40                  2.00                  2.05                     9.23                    0.97                  0.99                  1.00                  1.00
# concise_benchmarks
                 2.94                 2.12                 2.45                 2.19                 3.81             19079.27                  2.29                  2.38                     9.34                    1.24                  1.30                  1.25                  1.30


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.14                 0.14                 0.10                 0.10                 3.58                  0.12                  0.13                     6.79                    0.09                  0.07                  0.10                  0.10
# stl_vector_benchmarks
                 0.00                 4.52                 5.46                37.25                10.81               550.92                  5.41                  5.97                     0.00                    2.11                  1.88                  2.28                  2.10
# stl_vector_benchmarks_memtracked
                32.00                 4.67                 7.50                50.93                14.50               532.95                  6.45                  7.92                     0.00                    2.12                  1.90                  2.29                  2.12
# stl_hashset_benchmarks_memtracked
               195.02                28.38               171.81                59.17                74.27                54.83                132.93                215.75                    12.61                    6.64                  6.64                 29.42                  6.64
# stl_hashset_benchmarks
                 0.00                28.32               172.52                59.39                74.33                54.68                132.89                215.87                    12.61                    6.77                  6.76                 29.47                  6.75
# bitmagic_benchmarks
                 7.99                 0.38                 0.42                 0.18                 0.21                13.25                  0.38                  0.41                    10.53                    0.16                  0.22                  0.21                  0.22
# bitmagic_benchmarks -r
                 1.89                 0.27                 0.28                 0.18                 0.22                42.91                  0.27                  0.29                     6.86                    0.12                  0.20                  0.20                  0.22
# slow_roaring_benchmarks -r
                 0.60                 0.18                 0.31                 0.12                 0.03                84.40                  0.26                  0.38                     5.35                    0.12                  0.13                  0.13                  0.13
# malloced_roaring_benchmarks -r
                 0.62                 0.27                 0.42                 0.11                 0.04                69.22                  0.35                  0.47                     5.35                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks -r
                 0.60                 0.17                 0.31                 0.11                 0.03                67.56                  0.25                  0.36                     5.33                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks -c -r
                 0.60                 0.17                 0.30                 0.11                 0.03                72.01                  0.24                  0.34                     5.33                    0.11                  0.12                  0.12                  0.12
# roaring_benchmarks
                 2.99                 0.17                 0.28                 0.09                 0.54                70.41                  0.20                  0.32                     5.64                    0.08                  0.08                  0.08                  0.08
# roaring_benchmarks -c
                 2.99                 0.17                 0.28                 0.09                 0.54                77.94                  0.19                  0.31                     5.64                    0.08                  0.08                  0.08                  0.08
# ewah32_benchmarks
                 0.64                 0.28                 0.56                 0.16                 0.14               947.09                  0.41                  0.62                     6.03                    0.19                  0.29                  0.24                  0.30
# ewah64_benchmarks
                 0.90                 0.21                 0.39                 0.12                 0.10               658.25                  0.30                  0.43                     5.94                    0.14                  0.21                  0.17                  0.23
# wah32_benchmarks
                 0.65                 0.45                 0.65                 0.47                 0.47              3719.13                  0.55                  0.67                     7.63                    0.26                  0.29                  0.28                  0.29
# concise_benchmarks
                 0.55                 0.55                 0.76                 0.59                 0.61              3790.05                  0.65                  0.79                     7.85                    0.33                  0.38                  0.35                  0.38


# processing file  census1881
# bitset_benchmarks
               523.54                19.39                13.60                 9.71                 9.48                 2.40                 11.08                 13.22                    42.67                    6.33                  7.59                  8.29                  9.78
# stl_vector_benchmarks
                 0.00                 3.28                 6.08               543.40                66.33               165.96                  4.91                  6.40                     0.00                    1.46                  1.27                  1.63                  1.45
# stl_vector_benchmarks_memtracked
                32.00                 3.30                 8.09               708.97                74.01               154.48                  6.06                  8.49                     0.00                    1.45                  1.30                  1.63                  1.45
# stl_hashset_benchmarks_memtracked
               194.74                 0.49               264.31              1006.14              1230.86                38.81                232.69                367.16                    67.30                    0.47                  0.22                 44.14                  0.45
# stl_hashset_benchmarks
                 0.00                 0.48               262.71              1001.82              1228.56                39.27                233.72                366.33                    67.50                    0.46                  0.22                 44.13                  0.46
# bitmagic_benchmarks
                98.93                 2.01                 8.11                 3.01                 4.23                 8.79                  4.36                  7.99                    84.24                    0.28                  3.33                  1.92                  3.34
# bitmagic_benchmarks -r
                46.32                 1.19                 4.19                 2.20                 3.84                10.88                  2.81                  4.12                    70.93                    0.10                  1.63                  2.24                  1.58
# slow_roaring_benchmarks -r
                15.08                 0.12                 1.08                 4.47                10.96                16.78                  0.64                  0.98                     6.12                    0.09                  0.10                  0.09                  0.09
# malloced_roaring_benchmarks -r
                15.35                 0.31                 2.76                 2.69                10.41                17.11                  1.56                  2.67                     6.09                    0.08                  0.10                  0.08                  0.09
# roaring_benchmarks -r
                15.08                 0.12                 1.11                 2.51                 8.56                16.97                  0.63                  0.99                     5.16                    0.08                  0.11                  0.08                  0.09
# roaring_benchmarks -c -r
                15.08                 0.12                 0.51                 2.51                 8.52                19.02                  0.25                  0.36                     5.19                    0.08                  0.11                  0.08                  0.09
# roaring_benchmarks
                15.97                 0.11                 1.03                 2.68                 6.07                17.00                  0.64                  1.10                     5.17                    0.08                  0.08                  0.08                  0.08
# roaring_benchmarks -c
                15.97                 0.11                 0.41                 2.66                 6.38                18.85                  0.24                  0.34                     5.17                    0.08                  0.09                  0.08                  0.08
# ewah32_benchmarks
                33.77                 3.77                27.57               269.35                52.00              8826.01                 15.69                 27.65                    29.45                    3.72                 12.34                  8.12                 12.00
# ewah64_benchmarks
                43.77                 1.92                14.80               102.01                26.33              4897.62                  8.50                 14.72                    26.58                    1.81                  6.70                  4.19                  6.71
# wah32_benchmarks
                34.32                14.98                29.17               387.34                43.99             26582.83                 22.02                 29.28                    29.44                    8.68                 12.12                 11.00                 12.15
# concise_benchmarks
                25.56                20.38                33.84               398.92                57.28             30004.04                 26.48                 34.27                    31.70                   12.14                 16.84                 14.70                 16.77


# processing file  census1881_srt
# bitset_benchmarks
               888.07                32.80                23.59                16.10                16.05                 2.48                 19.35                 22.44                    41.36                   11.40                 12.54                 14.21                 16.26
# stl_vector_benchmarks
                 0.00                 3.96                 6.18               618.43                34.11               153.14                  5.60                  6.38                     0.00                    1.77                  2.20                  2.00                  1.77
# stl_vector_benchmarks_memtracked
                32.00                 3.95                 8.40               831.51                40.49               151.70                  6.82                  8.74                     0.00                    1.78                  1.56                  1.99                  1.77
# stl_hashset_benchmarks_memtracked
               194.93                 1.41               197.97               350.39               394.44                38.87                169.90                244.28                    20.56                    1.01                  0.79                 21.73                  0.78
# stl_hashset_benchmarks
                 0.00                 1.40               197.04               350.98               395.11                39.47                169.90                242.56                    20.45                    1.05                  0.83                 21.43                  0.80
# bitmagic_benchmarks
               249.26                 5.11                19.89                 7.13                 9.10                 9.02                 10.85                 19.72                    31.82                    0.92                  8.38                  4.92                  8.39
# bitmagic_benchmarks -r
                16.00                 0.95                 2.47                 4.61                 3.62                10.26                  2.93                  2.53                    10.96                    0.12                  0.54                  4.79                  0.53
# slow_roaring_benchmarks -r
                 2.16                 0.13                 1.06                 3.26                 3.80                18.37                  0.56                  0.94                     6.38                    0.06                  0.13                  0.09                  0.12
# malloced_roaring_benchmarks -r
                 2.77                 0.53                 5.00                 3.59                 8.50                17.64                  2.79                  5.04                     6.38                    0.06                  0.14                  0.09                  0.13
# roaring_benchmarks -r
                 2.16                 0.13                 1.08                 3.22                 3.83                17.94                  0.57                  0.96                     6.32                    0.06                  0.14                  0.09                  0.13
# roaring_benchmarks -c -r
                 2.16                 0.13                 0.56                 3.29                 3.92                21.08                  0.32                  0.43                     6.34                    0.07                  0.15                  0.11                  0.14
# roaring_benchmarks
                 6.09                 0.14                 1.18                 3.96                 8.25                19.81                  0.70                  1.13                     5.57                    0.08                  0.09                  0.08                  0.08
# roaring_benchmarks -c
                 6.09                 0.14                 0.49                 4.12                 8.33                22.86                  0.30                  0.37                     5.59                    0.08                  0.10                  0.09                  0.09
# ewah32_benchmarks
                 2.91                 0.50                 2.49               167.95                 5.13               561.39                  1.46                  2.48                     9.10                    0.44                  1.07                  0.73                  1.06
# ewah64_benchmarks
                 4.54                 0.39                 2.09               125.01                 4.39               440.48                  1.22                  2.09                     8.18                    0.32                  0.97                  0.59                  0.95
# wah32_benchmarks
                 2.95                 1.63                 3.05               196.97                 5.69              1814.42                  2.33                  2.99                    10.01                    0.98                  1.32                  1.06                  1.30
# concise_benchmarks
                 2.48                 1.88                 3.41               221.84                 6.61              1876.38                  2.55                  3.41                    10.12                    1.22                  1.65                  1.31                  1.65


# processing file  weather_sept_85
# bitset_benchmarks
                15.26                 0.39                 0.37                 0.33                 0.34                 3.64                  0.36                  0.37                    10.03                    0.27                  0.22                  0.28                  0.28
# stl_vector_benchmarks
                 0.00                 7.26                 8.01                98.12                28.05               583.89                  8.37                  8.92                     0.00                    4.59                  4.38                  5.12                  4.53
# stl_vector_benchmarks_memtracked
                32.00                 7.43                 9.69               115.59                31.44               563.35                  9.13                 10.46                     0.00                    4.65                  4.39                  5.07                  4.61
# stl_hashset_benchmarks_memtracked
               194.93                35.49               250.44               237.31               256.54                61.98                246.23                359.29                    40.93                   17.52                 17.54                 60.90                 17.53
# stl_hashset_benchmarks
                 0.00                35.00               247.97               234.51               252.65                63.01                241.46                351.32                    40.72                   17.45                 17.52                 60.99                 17.53
# bitmagic_benchmarks
                14.80                 0.69                 0.79                 0.37                 0.43                12.69                  0.72                  0.78                    33.23                    0.32                  0.42                  0.39                  0.42
# bitmagic_benchmarks -r
                 8.79                 0.65                 0.68                 0.27                 0.36                25.83                  0.62                  0.68                    31.68                    0.24                  0.64                  0.52                  0.65
# slow_roaring_benchmarks -r
                 5.38                 0.99                 0.89                 0.27                 1.44                71.49                  1.11                  1.06                     7.28                    0.86                  0.86                  0.86                  0.86
# malloced_roaring_benchmarks -r
                 5.42                 0.56                 0.73                 0.16                 1.29                69.65                  0.62                  0.91                     7.18                    0.27                  0.27                  0.26                  0.26
# roaring_benchmarks -r
                 5.38                 0.40                 0.57                 0.16                 1.06                79.79                  0.45                  0.76                     6.43                    0.26                  0.27                  0.26                  0.27
# roaring_benchmarks -c -r
                 5.38                 0.40                 0.57                 0.16                 1.06                87.13                  0.44                  0.75                     6.43                    0.26                  0.27                  0.26                  0.27
# roaring_benchmarks
                 5.44                 0.38                 0.54                 0.15                 1.03                80.57                  0.44                  0.75                     6.41                    0.25                  0.25                  0.25                  0.25
# roaring_benchmarks -c
                 5.44                 0.38                 0.54                 0.15                 1.03                93.05                  0.42                  0.73                     6.42                    0.25                  0.25                  0.25                  0.25
# ewah32_benchmarks
                 6.67                 2.69                 4.40                 2.34                 4.51             13791.02                  3.55                  4.40                    15.86                    1.63                  2.50                  2.06                  2.51
# ewah64_benchmarks
                 7.87                 1.55                 2.23                 1.29                 2.26              7486.26                  1.89                  2.21                    11.39                    0.84                  1.28                  1.02                  1.28
# wah32_benchmarks
                 6.82                 3.61                 4.59                 4.92                 6.78             65018.81                  4.32                  4.54                    12.43                    2.06                  2.13                  2.14                  2.13
# concise_benchmarks
                 5.88                 4.48                 5.45                 5.89                 8.01             69101.66                  5.09                  5.44                    12.83                    2.73                  2.88                  2.76                  2.87


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.31                 0.29                 0.24                 0.26                 3.56                  0.28                  0.29                     6.73                    0.19                  0.17                  0.21                  0.21
# stl_vector_benchmarks
                 0.00                 4.36                 5.79                72.79                13.93               665.87                  5.71                  6.58                     0.00                    1.98                  1.75                  2.19                  1.97
# stl_vector_benchmarks_memtracked
                32.00                 4.41                 7.97                84.07                17.84               667.60                  6.69                  8.28                     0.00                    1.98                  1.79                  2.19                  1.98
# stl_hashset_benchmarks_memtracked
               195.07                15.09               178.08                81.32               110.98                61.68                148.86                236.19                    12.89                    6.99                  7.00                 32.22                  7.00
# stl_hashset_benchmarks
                 0.00                15.11               178.98                82.40               111.47                60.17                148.87                236.78                    12.94                    7.16                  7.17                 32.27                  7.17
# bitmagic_benchmarks
                 9.03                 0.37                 0.54                 0.22                 0.27                11.50                  0.43                  0.54                     9.97                    0.16                  0.27                  0.23                  0.27
# bitmagic_benchmarks -r
                 0.96                 0.16                 0.21                 0.21                 0.33                39.55                  0.22                  0.22                     6.79                    0.10                  0.14                  0.20                  0.15
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.18                 0.04                 0.05                71.83                  0.15                  0.22                     5.26                    0.06                  0.07                  0.06                  0.07
# malloced_roaring_benchmarks -r
                 0.36                 0.17                 0.30                 0.04                 0.10                59.94                  0.25                  0.34                     5.26                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -r
                 0.34                 0.09                 0.18                 0.04                 0.05                54.81                  0.15                  0.22                     5.25                    0.06                  0.06                  0.06                  0.06
# roaring_benchmarks -c -r
                 0.34                 0.09                 0.16                 0.04                 0.05                66.57                  0.14                  0.20                     5.26                    0.06                  0.07                  0.06                  0.07
# roaring_benchmarks
                 3.24                 0.12                 0.24                 0.07                 0.49                56.67                  0.18                  0.28                     5.68                    0.07                  0.06                  0.06                  0.06
# roaring_benchmarks -c
                 3.24                 0.12                 0.23                 0.07                 0.49                68.37                  0.16                  0.25                     5.69                    0.07                  0.07                  0.06                  0.07
# ewah32_benchmarks
                 0.54                 0.19                 0.48                 0.32                 0.11              2268.34                  0.34                  0.51                     5.87                    0.14                  0.23                  0.19                  0.24
# ewah64_benchmarks
                 0.86                 0.16                 0.39                 0.24                 0.09              1816.63                  0.28                  0.42                     5.77                    0.12                  0.20                  0.16                  0.21
# wah32_benchmarks
                 0.54                 0.35                 0.55                 0.58                 0.37              7510.26                  0.46                  0.56                     7.46                    0.21                  0.25                  0.23                  0.25
# concise_benchmarks
                 0.43                 0.40                 0.62                 0.69                 0.44              7332.51                  0.51                  0.64                     7.68                    0.26                  0.30                  0.27                  0.30


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                25.90                20.35                14.79                15.45                 3.84                 17.74                 19.67                    46.17                   11.67                 11.75                 13.44                 14.86
# stl_vector_benchmarks
                 0.00                 4.15                 6.98               515.92                51.26               167.27                  5.81                  7.32                     0.00                    2.22                  2.10                  2.51                  2.20
# stl_vector_benchmarks_memtracked
                32.00                 4.11                 9.24               691.07                61.81               170.71                  7.07                  9.66                     0.00                    2.33                  2.18                  2.54                  2.23
# stl_hashset_benchmarks_memtracked
               194.68                15.00               229.40               420.93               517.66                52.37                213.26                302.71                    25.34                   12.71                 10.71                 40.89                 11.58
# stl_hashset_benchmarks
                 0.00                14.99               241.01               424.57               520.21                52.78                212.88                302.29                    24.58                   12.68                 10.78                 40.20                 11.46
# bitmagic_benchmarks
               462.53                15.62                30.35                12.27                14.55                 9.97                 21.19                 29.92                    58.95                    5.86                 14.37                 10.71                 14.47
# bitmagic_benchmarks -r
                29.84                 3.24                 5.68                11.90                16.78                18.23                  5.91                  5.52                    12.16                    1.48                  2.22                  7.00                  2.13
# slow_roaring_benchmarks -r
                 5.89                 1.48                 4.07                 2.95                24.43                37.11                  2.71                  3.97                     9.59                    0.86                  1.31                  1.08                  1.27
# malloced_roaring_benchmarks -r
                 7.04                 4.60                10.86                 3.16                30.81                33.17                  7.74                 10.97                     9.58                    0.86                  1.32                  1.09                  1.30
# roaring_benchmarks -r
                 5.89                 1.51                 4.05                 2.86                24.25                28.03                  2.70                  3.94                     9.61                    0.85                  1.29                  1.07                  1.28
# roaring_benchmarks -c -r
                 5.89                 1.50                 3.53                 2.88                24.39                34.15                  2.37                  3.23                     9.72                    0.89                  1.37                  1.15                  1.36
# roaring_benchmarks
                16.49                 1.45                 4.39                 4.30                20.52                41.86                  2.45                  4.63                     5.39                    0.75                  0.76                  0.74                  0.75
# roaring_benchmarks -c
                16.49                 1.45                 3.75                 4.37                20.76                45.94                  1.87                  3.68                     5.45                    0.78                  0.83                  0.80                  0.81
# ewah32_benchmarks
                10.83                 2.91                 8.96               315.69                47.51              1030.63                  5.86                  8.94                    18.37                    2.48                  4.01                  3.27                  4.02
# ewah64_benchmarks
                19.41                 2.73                 8.43               197.20                39.79               985.62                  5.57                  8.42                    15.65                    2.30                  3.97                  2.95                  3.96
# wah32_benchmarks
                10.89                 6.12                10.92               411.78                46.10              2401.85                  8.31                 10.80                    16.06                    3.76                  4.55                  4.14                  4.56
# concise_benchmarks
                10.25                 6.35                11.28               445.25                52.41              2346.39                  8.39                 11.23                    16.65                    3.99                  4.88                  4.35                  4.89


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.09                18.08                14.03                15.19                 2.79                 14.89                 17.38                    33.21                    8.76                 10.43                 11.05                 12.78
# stl_vector_benchmarks
                 0.00                 4.17                 7.19               498.13                42.85               155.16                  5.58                  6.65                     0.00                    1.71                  1.50                  1.99                  1.69
# stl_vector_benchmarks_memtracked
                32.00                 4.24                 9.51               671.63                52.87               142.69                  6.73                  8.93                     0.00                    1.73                  1.55                  1.87                  1.70
# stl_hashset_benchmarks_memtracked
               195.18                26.39               179.65               270.45               355.66                49.23                159.30                228.78                    13.15                   10.09                  9.01                 25.83                  8.53
# stl_hashset_benchmarks
                 0.00                26.26               179.65               271.29               357.28                51.12                159.07                228.46                    13.09                   10.47                  9.18                 26.26                  8.76
# bitmagic_benchmarks
               370.07                11.57                25.34                 9.71                11.73                10.05                 16.89                 25.38                    38.42                    3.83                 11.73                  8.24                 11.72
# bitmagic_benchmarks -r
                23.99                 2.19                 4.14                 8.79                 6.89                14.04                  4.39                  4.19                     7.96                    0.51                  1.10                  5.23                  1.07
# slow_roaring_benchmarks -r
                 1.63                 0.65                 2.29                 1.90                 7.28                22.29                  1.29                  1.88                     5.88                    0.25                  0.47                  0.35                  0.46
# malloced_roaring_benchmarks -r
                 2.58                 2.62                 7.87                 2.04                12.65                21.29                  5.05                  7.69                     5.91                    0.25                  0.48                  0.37                  0.47
# roaring_benchmarks -r
                 1.63                 0.67                 2.31                 1.81                 7.60                20.62                  1.31                  1.98                     5.86                    0.25                  0.49                  0.36                  0.46
# roaring_benchmarks -c -r
                 1.63                 0.67                 1.97                 1.90                 7.46                23.35                  1.07                  1.48                     5.91                    0.27                  0.54                  0.40                  0.52
# roaring_benchmarks
                10.67                 0.78                 2.93                 3.04                13.15                26.58                  1.57                  3.07                     5.73                    0.34                  0.35                  0.33                  0.34
# roaring_benchmarks -c
                10.67                 0.76                 2.28                 3.14                13.39                28.81                  1.11                  2.12                     5.82                    0.37                  0.39                  0.36                  0.37
# ewah32_benchmarks
                 2.63                 0.73                 2.32               113.40                12.39               249.15                  1.57                  2.35                     7.95                    0.54                  0.98                  0.79                  0.98
# ewah64_benchmarks
                 4.66                 0.72                 2.14                89.77                11.06               233.58                  1.48                  2.18                     7.60                    0.50                  0.93                  0.69                  0.94
# wah32_benchmarks
                 2.67                 1.30                 2.58               133.14                12.27               529.83                  1.93                  2.54                     8.95                    0.75                  1.05                  0.95                  1.04
# concise_benchmarks
                 2.23                 1.38                 2.62               148.34                13.74               496.78                  2.00                  2.64                     9.19                    0.82                  1.16                  1.05                  1.16


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

