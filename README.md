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
$ make test
./scripts/all.sh
# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value), successive differences, successive symmetric differences, iterations through all values
# processing file  census-income
# bitset_benchmarks
                 5.66                 0.14                 0.13                 0.09                 0.09                 3.82                 0.12                 0.12                 13.08
# stl_vector_benchmarks
                 0.00                 7.73                 7.71                42.97                16.60               443.86                 8.95                 8.78                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 7.94                 9.76                55.39                19.69               425.18                 9.85                10.76                  0.00
# stl_hashset_benchmarks_memtracked
               218.55                49.80               190.99                87.87               122.50                46.89               161.24               270.23                 23.46
# stl_hashset_benchmarks
                 0.00                49.83               191.92                87.92               119.15                48.32               161.96               271.25                 23.32
# bitmagic_benchmarks
                 8.23                 0.43                 0.44                 0.19                 0.23                14.24                 0.44                 0.45                 22.25
# bitmagic_benchmarks -r
                 6.83                 0.48                 0.44                 0.16                 0.22                38.57                 0.44                 0.45                 20.99
# slow_roaring_benchmarks -r
                 2.60                 0.46                 0.41                 0.06                 0.59                56.80                  0.53                  0.54                  6.80
# malloced_roaring_benchmarks -r
                 2.62                 0.33                 0.36                 0.05                 0.55                58.34                  0.38                  0.46                  5.85
# roaring_benchmarks -r
                 2.60                 0.23                 0.27                 0.05                 0.47                65.05                  0.28                  0.37                  6.78
# roaring_benchmarks -c -r
                 2.60                 0.24                 0.28                 0.05                 0.48                67.89                  0.28                  0.37                  6.79
# roaring_benchmarks
                 2.74                 0.20                 0.25                 0.08                 0.47                62.65                  0.22                  0.32                  6.81
# roaring_benchmarks -c
                 2.74                 0.21                 0.25                 0.08                 0.47                71.98                  0.22                  0.32                  6.81
# ewah32_benchmarks
                 3.29                 1.37                 1.95                 0.46                 1.54              3087.66                 2.26                 1.92                  9.43
# ewah64_benchmarks
                 3.86                 0.81                 1.03                 0.25                 1.12              1732.99                 1.29                 1.02                  7.79
# wah32_benchmarks
                 3.37                 1.80                 2.11                 1.83                 3.41             18983.31                 1.76                 2.04                  9.24
# concise_benchmarks
                 2.94                 2.14                 2.44                 2.26                 3.84             19079.02                 2.06                 2.38                  9.11


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.15                 0.15                 0.10                 0.10                 3.76                 0.12                 0.13                 12.50
# stl_vector_benchmarks
                 0.00                 4.54                 5.49                37.55                10.86               572.25                 6.20                 6.04                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.71                 7.49                50.61                13.86               556.46                 7.00                 7.98                  0.00
# stl_hashset_benchmarks_memtracked
               217.89                27.68               167.49                59.83                73.97                47.61               127.01               220.16                 12.77
# stl_hashset_benchmarks
                 0.00                27.32               163.94                59.96                73.07                47.10               126.98               218.12                 12.77
# bitmagic_benchmarks
                 8.52                 0.44                 0.48                 0.19                 0.25                14.74                 0.46                 0.49                 11.29
# bitmagic_benchmarks -r
                 4.57                 0.32                 0.33                 0.19                 0.25                64.02                 0.35                 0.36                  6.53
# slow_roaring_benchmarks -r
                 0.60                 0.18                 0.31                 0.12                 0.03                70.46                  0.26                  0.37                  5.35
# malloced_roaring_benchmarks -r
                 0.62                 0.26                 0.42                 0.11                 0.04                63.39                  0.34                  0.46                  5.33
# roaring_benchmarks -r
                 0.60                 0.17                 0.30                 0.11                 0.03                67.26                  0.24                  0.35                  5.35
# roaring_benchmarks -c -r
                 0.60                 0.17                 0.30                 0.11                 0.03                73.16                  0.24                  0.34                  5.35
# roaring_benchmarks
                 2.99                 0.17                 0.28                 0.09                 0.54                74.82                  0.20                  0.32                  6.58
# roaring_benchmarks -c
                 2.99                 0.17                 0.28                 0.09                 0.55                80.73                  0.19                  0.31                  6.58
# ewah32_benchmarks
                 0.64                 0.27                 0.55                 0.15                 0.12               895.95                 0.56                 0.62                  4.79
# ewah64_benchmarks
                 0.90                 0.20                 0.38                 0.11                 0.10               647.28                 0.42                 0.42                  4.64
# wah32_benchmarks
                 0.65                 0.45                 0.64                 0.46                 0.47              3613.42                 0.46                 0.67                  7.63
# concise_benchmarks
                 0.55                 0.56                 0.76                 0.59                 0.62              3770.97                 0.56                 0.79                  7.57


# processing file  census1881
# bitset_benchmarks
               523.54                19.54                13.67                 9.90                 9.60                 2.43                11.17                13.33                 35.48
# stl_vector_benchmarks
                 0.00                 3.28                 6.10               548.37                68.65               176.06                 5.87                 6.41                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 3.31                 8.13               714.45                74.90               164.94                 6.87                 8.51                  0.00
# stl_hashset_benchmarks_memtracked
               212.26                 0.42               294.85              1010.98              1220.98                38.24               221.37               384.01                 57.82
# stl_hashset_benchmarks
                 0.00                 0.42               297.08              1011.98              1224.85                38.64               221.20               386.00                 58.20
# bitmagic_benchmarks
               102.18                 2.71                 8.08                 3.06                 4.84                10.16                 4.91                 8.14                 89.95
# bitmagic_benchmarks -r
                62.63                 1.92                 4.33                 2.23                 3.96                11.81                 3.26                 4.39                 70.92
# slow_roaring_benchmarks -r
                15.08                 0.12                 1.08                 4.05                10.58                16.84                  0.63                  1.00                  6.08
# malloced_roaring_benchmarks -r
                15.35                 0.30                 2.71                 2.71                10.39                17.17                  1.54                  2.59                  5.18
# roaring_benchmarks -r
                15.08                 0.12                 1.13                 2.53                 8.80                17.51                  0.64                  0.98                  6.18
# roaring_benchmarks -c -r
                15.08                 0.12                 0.51                 2.59                 8.77                19.12                  0.25                  0.36                  6.12
# roaring_benchmarks
                15.97                 0.11                 1.04                 2.79                 6.18                17.71                  0.63                  1.09                  6.15
# roaring_benchmarks -c
                15.97                 0.11                 0.40                 2.76                 6.52                19.45                  0.23                  0.34                  6.23
# ewah32_benchmarks
                33.77                 3.23                27.34               265.31                50.91              8370.37                23.75                27.95                 24.68
# ewah64_benchmarks
                43.77                 1.83                14.59               101.01                26.21              4829.29                13.29                14.61                 21.65
# wah32_benchmarks
                34.32                14.48                29.26               386.84                45.05             26025.22                18.05                28.95                 29.67
# concise_benchmarks
                25.56                20.84                33.30               400.93                57.47             29871.02                21.70                34.05                 31.15


# processing file  census1881_srt
# bitset_benchmarks
               888.07                33.19                23.92                16.50                16.25                 2.59                19.61                22.71                 32.02
# stl_vector_benchmarks
                 0.00                 3.98                 6.19               623.39                35.41               157.18                 6.58                 6.49                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 3.99                 8.37               833.32                42.05               156.06                 7.49                 8.78                  0.00
# stl_hashset_benchmarks_memtracked
               216.84                 1.13               201.65               337.70               383.44                36.78               167.15               245.90                 18.06
# stl_hashset_benchmarks
                 0.00                 1.14               201.77               337.13               383.67                36.80               167.45               247.72                 17.98
# bitmagic_benchmarks
               254.06                 6.01                19.59                 7.21                 9.93                10.20                11.72                19.87                 46.52
# bitmagic_benchmarks -r
                40.05                 1.88                 3.45                 4.61                 3.75                11.26                 3.69                 3.20                 11.14
# slow_roaring_benchmarks -r
                 2.16                 0.13                 1.06                 3.29                 3.81                17.23                  0.56                  0.93                  6.35
# malloced_roaring_benchmarks -r
                 2.77                 0.52                 4.89                 3.64                 8.34                21.42                  2.66                  4.81                  6.32
# roaring_benchmarks -r
                 2.16                 0.14                 1.07                 3.18                 3.87                18.61                  0.56                  0.94                  6.35
# roaring_benchmarks -c -r
                 2.16                 0.13                 0.57                 3.28                 3.95                20.23                  0.31                  0.43                  6.38
# roaring_benchmarks
                 6.09                 0.14                 1.17                 3.98                 8.25                20.66                  0.69                  1.11                  6.61
# roaring_benchmarks -c
                 6.09                 0.14                 0.50                 4.13                 8.31                23.07                  0.29                  0.38                  6.56
# ewah32_benchmarks
                 2.91                 0.45                 2.45               166.77                 5.07               529.48                 2.32                 2.50                  6.99
# ewah64_benchmarks
                 4.54                 0.36                 2.05               122.39                 4.45               435.70                 1.94                 2.06                  6.57
# wah32_benchmarks
                 2.95                 1.62                 3.03               197.65                 5.71              1732.16                 1.87                 2.98                  9.93
# concise_benchmarks
                 2.48                 1.95                 3.37               223.41                 6.58              1861.27                 2.10                 3.40                  9.84


# processing file  weather_sept_85
# bitset_benchmarks
                15.26                 0.42                 0.40                 0.35                 0.37                 3.53                 0.39                 0.40                 14.80
# stl_vector_benchmarks
                 0.00                 7.28                 7.94                96.36                27.88               577.45                 8.95                 8.97                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 7.33                 9.97               118.99                31.73               574.14                 9.80                11.15                  0.00
# stl_hashset_benchmarks_memtracked
               219.92                30.69               256.27               215.11               242.31                53.83               212.91               386.14                 34.48
# stl_hashset_benchmarks
                 0.00                30.61               256.79               216.03               244.80                54.56               212.87               386.26                 34.60
# bitmagic_benchmarks
                15.06                 0.71                 0.80                 0.38                 0.46                13.53                 0.75                 0.81                 34.07
# bitmagic_benchmarks -r
                10.06                 0.69                 0.70                 0.28                 0.40                29.86                 0.67                 0.72                 32.39
# slow_roaring_benchmarks -r
                 5.38                 1.01                 0.89                 0.25                 1.39                68.65                  1.11                  1.07                  7.30
# malloced_roaring_benchmarks -r
                 5.42                 0.56                 0.74                 0.17                 1.30                74.29                  0.62                  0.91                  6.37
# roaring_benchmarks -r
                 5.38                 0.40                 0.58                 0.16                 1.08                78.60                  0.46                  0.77                  7.30
# roaring_benchmarks -c -r
                 5.38                 0.40                 0.57                 0.16                 1.09                85.34                  0.45                  0.76                  7.30
# roaring_benchmarks
                 5.44                 0.38                 0.55                 0.16                 1.04                78.79                  0.44                  0.76                  7.32
# roaring_benchmarks -c
                 5.44                 0.38                 0.54                 0.16                 1.05                93.22                  0.43                  0.74                  7.32
# ewah32_benchmarks
                 6.67                 2.57                 4.29                 2.23                 4.41             13185.02                 4.71                 4.31                 12.39
# ewah64_benchmarks
                 7.87                 1.50                 2.20                 1.26                 2.25              7349.61                 2.65                 2.17                  9.67
# wah32_benchmarks
                 6.82                 3.59                 4.60                 4.93                 6.86             63781.26                 3.77                 4.52                 12.44
# concise_benchmarks
                 5.88                 4.53                 5.41                 6.02                 8.09             68796.81                 4.50                 5.42                 12.37


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.34                 0.32                 0.26                 0.27                 3.49                 0.30                 0.32                 12.63
# stl_vector_benchmarks
                 0.00                 4.38                 6.05                68.39                14.03               681.47                 6.53                 6.29                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.44                 7.90                82.73                17.29               663.52                 7.30                 8.37                  0.00
# stl_hashset_benchmarks_memtracked
               219.63                13.83               187.78                74.15               109.34                48.29               142.20               245.30                 12.79
# stl_hashset_benchmarks
                 0.00                13.83               185.97                74.26               108.85                49.23               142.34               244.18                 12.73
# bitmagic_benchmarks
                 9.23                 0.39                 0.54                 0.23                 0.29                12.58                 0.46                 0.55                 10.74
# bitmagic_benchmarks -r
                 1.98                 0.19                 0.22                 0.21                 0.33                54.57                 0.24                 0.25                  6.62
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                54.01                  0.15                  0.22                  5.26
# malloced_roaring_benchmarks -r
                 0.36                 0.16                 0.30                 0.04                 0.10                63.10                  0.24                  0.34                  5.25
# roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                54.42                  0.15                  0.22                  5.26
# roaring_benchmarks -c -r
                 0.34                 0.09                 0.16                 0.04                 0.05                66.78                  0.14                  0.19                  5.26
# roaring_benchmarks
                 3.24                 0.12                 0.24                 0.07                 0.49                58.54                  0.18                  0.29                  6.62
# roaring_benchmarks -c
                 3.24                 0.12                 0.23                 0.07                 0.49                73.09                  0.16                  0.25                  6.62
# ewah32_benchmarks
                 0.54                 0.18                 0.47                 0.30                 0.10              2133.30                 0.47                 0.51                  4.60
# ewah64_benchmarks
                 0.86                 0.15                 0.39                 0.23                 0.08              1790.33                 0.40                 0.42                  4.54
# wah32_benchmarks
                 0.54                 0.35                 0.55                 0.58                 0.37              7179.67                 0.37                 0.56                  7.46
# concise_benchmarks
                 0.43                 0.41                 0.62                 0.69                 0.45              7260.64                 0.43                 0.64                  7.40


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                26.67                20.82                15.47                16.04                 3.63                18.51                20.26                 36.95
# stl_vector_benchmarks
                 0.00                 4.13                 7.03               515.60                52.14               171.12                 6.69                 7.47                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.18                 9.26               690.12                61.67               161.97                 7.70                 9.73                  0.00
# stl_hashset_benchmarks_memtracked
               226.35                12.64               226.81               384.18               507.11                44.82               196.80               311.96                 20.95
# stl_hashset_benchmarks
                 0.00                12.60               226.36               383.33               505.51                44.14               198.47               311.26                 21.19
# bitmagic_benchmarks
               474.39                24.99                59.90                12.46                16.81                11.25                23.21                31.51                 92.17
# bitmagic_benchmarks -r
                89.29                 4.64                 6.58                12.08                16.85                21.15                 7.79                 7.33                 13.07
# slow_roaring_benchmarks -r
                 5.89                 1.49                 4.06                 2.75                24.43                28.45                  2.65                  3.83                  9.55
# malloced_roaring_benchmarks -r
                 7.04                 4.46                10.71                 3.02                30.64                30.87                  7.58                 10.84                  9.54
# roaring_benchmarks -r
                 5.89                 1.51                 4.10                 2.67                24.31                29.53                  2.70                  3.86                  9.52
# roaring_benchmarks -c -r
                 5.89                 1.53                 3.51                 2.76                24.43                33.04                  2.36                  3.14                  9.77
# roaring_benchmarks
                16.49                 1.45                 4.37                 4.30                20.50                37.37                  2.42                  4.60                  6.34
# roaring_benchmarks -c
                16.49                 1.37                 3.67                 4.34                20.91                47.94                  1.84                  3.67                  6.41
# ewah32_benchmarks
                10.83                 2.66                 9.01               312.66                47.51               974.79                 9.28                 9.10                 14.36
# ewah64_benchmarks
                19.41                 2.56                 8.45               191.93                39.64               977.11                 8.92                 8.33                 13.30
# wah32_benchmarks
                10.89                 6.16                10.87               414.85                46.34              2199.32                 6.80                10.69                 16.06
# concise_benchmarks
                10.25                 6.40                11.33               449.77                52.21              2303.22                 7.04                11.37                 16.21


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.77                18.67                14.35                15.61                 2.86                15.21                17.78                 29.25
# stl_vector_benchmarks
                 0.00                 4.23                 7.20               498.06                42.55               214.01                 6.46                 6.71                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.27                 9.46               671.05                52.67               136.57                 7.36                 8.97                  0.00
# stl_hashset_benchmarks_memtracked
               228.86                24.76               173.62               263.91               358.07                42.67               156.49               228.59                 13.11
# stl_hashset_benchmarks
                 0.00                24.68               173.41               268.71               358.20                43.78               156.99               228.44                 12.91
# bitmagic_benchmarks
               381.41                13.21                25.27                 9.83                13.54                10.74                18.28                26.14                 66.06
# bitmagic_benchmarks -r
                80.83                 3.53                 5.07                 8.84                 7.20                15.44                 5.89                 5.57                  8.57
# slow_roaring_benchmarks -r
                 1.63                 0.67                 2.26                 1.87                 7.25                21.06                  1.27                  1.84                  5.91
# malloced_roaring_benchmarks -r
                 2.58                 2.56                 7.75                 2.05                12.53                22.20                  4.96                  7.54                  5.87
# roaring_benchmarks -r
                 1.63                 0.67                 2.32                 1.78                 7.31                21.71                  1.30                  1.95                  5.96
# roaring_benchmarks -c -r
                 1.63                 0.67                 1.97                 1.87                 7.64                24.09                  1.06                  1.53                  5.98
# roaring_benchmarks
                10.67                 0.73                 2.93                 3.08                13.20                24.81                  1.57                  3.08                  6.68
# roaring_benchmarks -c
                10.67                 0.73                 2.27                 3.14                13.35                29.49                  1.11                  2.13                  6.72
# ewah32_benchmarks
                 2.63                 0.70                 2.29               113.63                12.28               236.89                 2.59                 2.34                  6.29
# ewah64_benchmarks
                 4.66                 0.69                 2.16                90.73                10.98               230.64                 2.55                 2.14                  6.10
# wah32_benchmarks
                 2.67                 1.27                 2.56               132.50                12.39               497.33                 1.59                 2.51                  8.92
# concise_benchmarks
                 2.23                 1.39                 2.63               148.85                13.66               494.77                 1.69                 2.64                  8.93
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

