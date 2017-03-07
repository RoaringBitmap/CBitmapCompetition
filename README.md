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

Thus, we output at least 5 numbers:
- The memory usage, expressed as a floating-point numbers representing the number of bits used by value. In practice, this is computed by counting the number of bits used in RAM divided by the total cardinality. The total cardinality is given by the sum of the cardinalities of all sets. Thus if you have 200 sets each containing 10 values, your total cardinality is 2000, if you use 2000 bytes of memory, then your number of bits per value will be 8.0.
- The number of CPU cycles used to compute the successive intersections, normalized by the number of input values.
- The number of CPU cycles used to compute the successive unions, normalized by the number of input values.
- The number of CPU cycles used to compute the total union using a naive algorithm, normalized by the number of input values.
- The number of CPU cycles used to compute the total union using a heap-based or sort algorithm, normalized by the number of input values. In some cases, there might not be a reasonable heap-based or sort algorithm, but that is ok.
- The number of CPU cycles to for each quartile query.
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


Note: for some selected formats, we also include the successive differences (ANDNOT) as well as the symmetric difference (XOR), and also the time required to iterate over all bitmaps.

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
# For each data set, we print data size (in bits per value), successive intersections, successive unions and total unions [we compute the total  union first naively and then (if supported) using a heap-based approach], followed by quartile point queries (in cycles per input value)
# processing file  census-income
# bitset_benchmarks
                 5.66                 0.18                 0.18                 0.11                 0.11                 3.05                 0.16                 0.17                 11.68
# stl_vector_benchmarks
                 0.00                 3.94                 6.36                38.51                20.82               714.62                 7.27                 6.03                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.76                 6.89                45.47                18.56              1146.53                 5.55                 5.84                  0.00
# stl_hashset_benchmarks_memtracked
               266.65                65.80               393.03               102.96               168.81                52.94               215.89               420.58                 19.59
# stl_hashset_benchmarks
                 0.00                62.03               360.19               101.16               159.78                52.20               217.03               425.11                 17.52
# bitmagic_benchmarks
                 8.23                 0.84                 0.43                 0.19                 0.23                12.51                 0.53                 0.44                 29.66
# bitmagic_benchmarks -r
                 6.83                 0.40                 0.38                 0.14                 0.18                48.82                 0.62                 0.49                 29.51
# slow_roaring_benchmarks -r
                 2.60                 0.96                 1.63                 0.84                 1.26               154.07                  0.70                  0.57                 25.97
# malloced_roaring_benchmarks -r
                 2.62                 4.90                 2.97                 0.81                 5.31               114.79                  3.18                  2.38                 25.32
# roaring_benchmarks -r
                 2.60                 0.72                 0.83                 0.74                 0.78                95.76                  0.33                  0.33                 29.12
# roaring_benchmarks -c -r
                 2.60                 0.24                 0.25                 0.04                 0.44                59.70                  0.22                  0.31                 24.16
# roaring_benchmarks
                 2.74                 0.18                 0.21                 0.06                 0.44                56.84                  0.18                  0.26                 28.46
# roaring_benchmarks -c
                 2.74                 0.20                 0.21                 0.06                 0.43                56.30                  0.18                  0.26                 26.52
# ewah32_benchmarks
                 3.29                 1.13                 1.73                 0.43                 1.95              2477.16                 2.18                 1.78                 12.52
# ewah64_benchmarks
                 3.86                 0.68                 1.02                 0.24                 1.11              1282.22                 1.27                 0.94                 12.33
# wah32_benchmarks
                 3.37                 1.36                 1.87                 1.78                 3.49              6802.09                 1.64                 1.66                  8.99
# concise_benchmarks
                 2.94                 2.66                 2.08                 2.89                 4.27              6979.84                 1.93                 2.29                  8.20


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.19                 0.19                 0.10                 0.10                 3.38                 0.15                 0.17                  9.44
# stl_vector_benchmarks
                 0.00                 1.75                 5.24                36.27                14.13               615.51                 5.03                 5.43                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 1.82                 5.70                43.68                12.36               433.77                 5.14                 6.88                  0.00
# stl_hashset_benchmarks_memtracked
               268.91                39.61               348.29                71.37                99.10                84.92               186.52               384.17                 10.36
# stl_hashset_benchmarks
                 0.00                66.84               354.18                76.25               100.26                47.54               188.75               391.62                 12.63
# bitmagic_benchmarks
                 8.52                 0.95                 0.43                 0.25                 0.40                12.89                 0.40                 0.43                 21.32
# bitmagic_benchmarks -r
                 4.57                 0.28                 0.59                 0.23                 0.28               101.47                 0.49                 0.30                 10.18
# slow_roaring_benchmarks -r
                 0.60                 1.31                 1.20                 1.07                 0.06               149.28                  0.50                  0.80                  6.98
# malloced_roaring_benchmarks -r
                 0.62                 2.90                 3.54                 0.97                 0.46               129.59                  2.92                  3.85                  7.06
# roaring_benchmarks -r
                 0.60                 0.72                 1.07                 0.86                 0.06               161.91                  0.47                  0.61                  6.37
# roaring_benchmarks -c -r
                 0.60                 0.22                 0.27                 0.07                 0.06                71.03                  0.26                  0.30                  8.80
# roaring_benchmarks
                 2.99                 0.16                 0.24                 0.09                 0.55                64.81                  0.19                  0.27                 28.63
# roaring_benchmarks -c
                 2.99                 0.24                 0.34                 0.06                 0.49                65.68                  0.19                  0.27                 32.11
# ewah32_benchmarks
                 0.64                 0.20                 0.46                 0.13                 0.11               731.45                 0.60                 0.63                  4.59
# ewah64_benchmarks
                 0.90                 0.28                 0.45                 0.09                 0.09               479.76                 0.78                 0.41                  4.86
# wah32_benchmarks
                 0.65                 0.34                 0.49                 0.41                 0.43              1764.96                 0.85                 1.19                  7.72
# concise_benchmarks
                 0.55                 0.87                 0.61                 0.55                 0.58              1711.63                 0.52                 0.64                 10.75


# processing file  census1881
# bitset_benchmarks
               523.54                19.44                53.30                16.25                15.66                 3.42                17.62                50.91                 27.86
# stl_vector_benchmarks
                 0.00                 1.20                 6.85               740.25                66.01               123.31                 4.43                 5.46                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 1.23                 8.32               724.55                64.50               126.16                 4.71                 6.83                  0.00
# stl_hashset_benchmarks_memtracked
               282.84                 0.36               502.39              1525.21              1387.99                30.75               311.64               627.35                 40.93
# stl_hashset_benchmarks
                 0.00                 0.32               511.28              1479.40              1480.83                30.52               342.10               626.76                 41.78
# bitmagic_benchmarks
               102.18                 6.12                13.52                 5.13                 7.15                10.62                 6.00                 8.07                 71.59
# bitmagic_benchmarks -r
                62.63                 2.33                 4.71                 2.16                 3.83                11.44                 3.11                 4.79                 70.83
# slow_roaring_benchmarks -r
                15.08                 0.18                 5.77                 4.09                11.00                11.72                  1.03                  1.64                 12.60
# malloced_roaring_benchmarks -r
                15.35                 4.81                46.59                 6.60                54.39                11.81                 33.04                 59.92                 19.47
# roaring_benchmarks -r
                15.08                 0.71                 3.94                 2.29                 8.00                12.35                  1.02                  1.45                 12.77
# roaring_benchmarks -c -r
                15.08                 0.20                 0.63                 2.90                10.13                14.94                  0.26                  0.38                 20.81
# roaring_benchmarks
                15.97                 0.17                 1.59                 2.30                11.84                22.59                  1.55                  2.72                 15.02
# roaring_benchmarks -c
                15.97                 0.20                 0.48                 2.45                 7.74                13.24                  0.23                  0.36                 13.40
# ewah32_benchmarks
                33.77                 2.75                30.61               247.97                53.59              6736.12                22.00                34.38                 20.19
# ewah64_benchmarks
                43.77                 1.88                12.64                94.16                32.43              4205.79                14.83                11.50                 20.02
# wah32_benchmarks
                34.32                15.62                23.78               416.56                50.60             15831.77                17.39                22.48                 25.72
# concise_benchmarks
                25.56                18.08                32.36               498.71                61.98             18122.73                21.35                34.72                 28.10


# processing file  census1881_srt
# bitset_benchmarks
               888.07                32.84                94.84                24.72                33.26                 4.26                30.99                70.63                 30.07
# stl_vector_benchmarks
                 0.00                 1.73                 8.36               828.61                35.05               109.96                 4.76                 5.31                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 2.53                 6.18               823.76                38.88               113.84                 5.08                 5.40                  0.00
# stl_hashset_benchmarks_memtracked
               276.19                 1.08               383.94               560.84               570.39                27.45               219.16               389.72                 12.84
# stl_hashset_benchmarks
                 0.00                 1.13               386.31               538.43               613.26                28.60               227.73               395.43                 12.43
# bitmagic_benchmarks
               254.06                 8.39                27.74                 7.30                11.42                 9.09                11.16                19.03                 32.62
# bitmagic_benchmarks -r
                40.05                 1.74                 4.54                 3.80                 3.31                12.79                 3.42                 3.81                 13.26
# slow_roaring_benchmarks -r
                 2.16                 0.21                 2.50                 2.68                 4.81                12.68                  1.27                  2.25                  6.81
# malloced_roaring_benchmarks -r
                 2.77                14.45               133.55                10.39               121.41                23.53                 59.19                105.75                  7.88
# roaring_benchmarks -r
                 2.16                 0.28                 2.35                 2.73                 4.55                13.48                  1.21                  2.29                  7.44
# roaring_benchmarks -c -r
                 2.16                 0.27                 0.67                 2.84                 5.05                15.46                  0.36                  0.43                  6.98
# roaring_benchmarks
                 6.09                 0.28                 2.59                 3.21                 7.57                14.48                  1.34                  2.52                 22.80
# roaring_benchmarks -c
                 6.09                 0.24                 0.64                 3.10                 7.75                16.70                  0.35                  0.42                 22.38
# ewah32_benchmarks
                 2.91                 0.40                 2.05               185.10                 5.89               421.78                 2.22                 2.28                  5.65
# ewah64_benchmarks
                 4.54                 0.41                 2.10               145.17                 8.34               519.17                 1.79                 1.68                  6.20
# wah32_benchmarks
                 2.95                 2.21                 7.00               245.96                 5.20              1004.26                 1.98                 2.51                  8.64
# concise_benchmarks
                 2.48                 1.80                 2.78               282.22                 6.28              1736.25                 2.80                 2.66                  8.64


# processing file  weather_sept_85
# bitset_benchmarks
                15.26                 0.47                 0.52                 0.36                 0.34                 4.36                 0.43                 0.54                 10.31
# stl_vector_benchmarks
                 0.00                 3.41                 8.99               134.34                30.06               536.20                 8.35                 7.97                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 3.47                 9.27               139.56                31.30               516.70                 7.03                 8.28                  0.00
# stl_hashset_benchmarks_memtracked
               280.50                37.73               468.34               226.84               264.51                58.08               296.59               561.80                 28.01
# stl_hashset_benchmarks
                 0.00                39.34               498.13               231.48               265.26               120.16               343.98               582.07                 28.48
# bitmagic_benchmarks
                15.06                 0.95                 0.89                 0.33                 0.49                12.37                 0.79                 0.75                 36.09
# bitmagic_benchmarks -r
                10.06                 0.83                 1.08                 0.45                 0.44                38.50                 0.67                 0.78                 40.10
# slow_roaring_benchmarks -r
                 5.38                 0.73                 0.81                 0.19                 1.35                70.65                  0.87                  1.12                 24.44
# malloced_roaring_benchmarks -r
                 5.42                 4.47                 4.62                 0.20                 6.96                73.40                  4.76                  5.12                 24.20
# roaring_benchmarks -r
                 5.38                 0.35                 0.70                 0.13                 1.06                71.26                  0.37                  0.69                 26.76
# roaring_benchmarks -c -r
                 5.38                 0.49                 0.74                 0.28                 1.29                78.33                  0.41                  1.00                 27.09
# roaring_benchmarks
                 5.44                 0.34                 0.55                 0.26                 1.70               158.17                  0.54                  0.76                 28.27
# roaring_benchmarks -c
                 5.44                 0.34                 0.51                 0.14                 1.16                82.46                  0.41                  1.11                 26.21
# ewah32_benchmarks
                 6.67                 2.25                 3.74                 2.15                 4.54             11310.47                 4.47                 4.80                 12.93
# ewah64_benchmarks
                 7.87                 1.33                 2.64                 1.47                 3.09              7047.77                 2.76                 2.02                 12.26
# wah32_benchmarks
                 6.82                 3.11                 4.50                 6.56                 8.20             29101.36                 4.35                 4.03                 11.00
# concise_benchmarks
                 5.88                 4.59                 5.04                 6.53                 9.52             32468.55                 4.57                 4.65                 11.05


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.42                 0.63                 0.24                 0.25                 4.13                 0.52                 0.92                  9.01
# stl_vector_benchmarks
                 0.00                 1.95                 7.79               105.54                24.24               682.13                 6.45                 7.71                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 1.74                 7.93               110.41                23.92               759.59                 6.06                 7.15                  0.00
# stl_hashset_benchmarks_memtracked
               282.04                20.71               389.04                85.42               159.70                54.62               226.06               463.50                 13.92
# stl_hashset_benchmarks
                 0.00                20.87               372.47                85.45               121.40                56.53               205.33               413.85                 12.24
# bitmagic_benchmarks
                 9.23                 0.53                 0.60                 0.25                 0.35                11.47                 0.46                 0.60                 19.12
# bitmagic_benchmarks -r
                 1.98                 0.16                 0.22                 0.16                 0.22                53.89                 0.20                 0.24                 11.87
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.19                 0.03                 0.05                77.65                  0.14                  0.21                  6.44
# malloced_roaring_benchmarks -r
                 0.36                 2.69                 3.94                 0.42                 2.20                86.22                  2.92                  3.78                  8.88
# roaring_benchmarks -r
                 0.34                 0.12                 0.17                 0.03                 0.06                77.94                  0.13                  0.20                  5.93
# roaring_benchmarks -c -r
                 0.34                 0.09                 0.15                 0.03                 0.06                87.34                  0.11                  0.25                  7.16
# roaring_benchmarks
                 3.24                 0.14                 0.23                 0.06                 0.49                53.41                  0.18                  0.26                 28.53
# roaring_benchmarks -c
                 3.24                 0.13                 0.21                 0.09                 0.58                68.34                  0.17                  0.24                 29.68
# ewah32_benchmarks
                 0.54                 0.14                 0.44                 0.28                 0.19              1690.18                 0.83                 0.65                  3.92
# ewah64_benchmarks
                 0.86                 0.13                 0.48                 0.21                 0.07              1303.32                 0.43                 0.38                  4.46
# wah32_benchmarks
                 0.54                 0.43                 0.73                 0.64                 0.44              3531.81                 0.83                 0.88                  7.28
# concise_benchmarks
                 0.43                 0.57                 0.50                 0.93                 0.75              3357.67                 0.44                 0.53                  7.53


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                29.09                57.72                24.63                21.59                 4.34                25.49                59.35                 48.13
# stl_vector_benchmarks
                 0.00                 1.64                 6.04               499.86                52.99               212.38                 5.10                 6.13                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 1.65                 6.27               528.26                53.33               282.51                 5.16                 5.89                  0.00
# stl_hashset_benchmarks_memtracked
               289.14                13.00               390.89               725.75               674.67                44.54               265.43               477.09                 25.20
# stl_hashset_benchmarks
                 0.00                14.10               465.38               743.77               722.59                72.93               336.16               486.81                 34.36
# bitmagic_benchmarks
               474.39                17.11                43.46                15.13                18.51                25.17                26.17                50.77                 53.06
# bitmagic_benchmarks -r
                89.29                 4.18                 7.71                10.00                17.84                24.80                 6.79                 7.36                 11.85
# slow_roaring_benchmarks -r
                 5.89                 5.37                 6.08                 2.82                20.45                21.35                  3.70                  5.51                  9.63
# malloced_roaring_benchmarks -r
                 7.04                77.58               192.69                 6.70               272.65                23.32                125.65                189.41                 10.55
# roaring_benchmarks -r
                 5.89                 1.98                 5.14                 2.22                17.34                25.37                  3.54                  5.19                  9.63
# roaring_benchmarks -c -r
                 5.89                 2.38                 3.98                 2.33                45.71                42.77                  3.08                  3.18                  9.69
# roaring_benchmarks
                16.49                 2.52                 5.93                 3.91                31.08                58.58                  3.71                  5.55                 13.43
# roaring_benchmarks -c
                16.49                 2.30                 3.99                 5.94                21.82                43.96                  2.19                  3.56                 13.81
# ewah32_benchmarks
                10.83                 2.20                 8.01               294.27                43.75               769.42                 9.78                 7.95                 13.21
# ewah64_benchmarks
                19.41                 2.17                 8.02               164.70                51.02               698.46                 8.64                 7.61                 13.87
# wah32_benchmarks
                10.89                11.89                20.71               510.34                42.10              1258.26                 6.45                 8.25                 25.37
# concise_benchmarks
                10.25                 5.87                 9.68               497.08                50.65              1223.74                 7.23                 9.22                 13.88


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                47.82                47.93                16.55                16.71                 3.72                20.05                38.51                 24.70
# stl_vector_benchmarks
                 0.00                 1.70                 6.80               611.68                67.90               310.68                 9.70                 9.23                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 1.58                 5.74               480.70                46.92               206.58                 5.21                 5.60                  0.00
# stl_hashset_benchmarks_memtracked
               289.64                38.67               333.53               479.20               490.19                60.47               180.24               446.51                 12.48
# stl_hashset_benchmarks
                 0.00                34.27               348.48               459.34               488.22                64.45               229.10               382.65                 11.46
# bitmagic_benchmarks
               381.41                13.42                45.53                12.26                15.03                 8.80                17.29                25.14                 37.16
# bitmagic_benchmarks -r
                80.83                 3.55                 7.93                 8.50                 6.13                14.75                 9.40                 8.47                  9.10
# slow_roaring_benchmarks -r
                 1.63                 1.19                 6.84                 1.99                 7.66                15.52                  2.21                  3.42                 11.75
# malloced_roaring_benchmarks -r
                 2.58                62.49               145.17                 6.08               150.42                14.87                138.11                177.40                  7.61
# roaring_benchmarks -r
                 1.63                 1.26                 3.74                 2.22                 7.50                15.86                  2.37                  3.67                  6.52
# roaring_benchmarks -c -r
                 1.63                 1.13                 2.13                 1.67                 7.89                16.34                  1.34                  1.70                  6.47
# roaring_benchmarks
                10.67                 1.34                 3.72                 2.47                12.44                26.21                  2.30                  3.96                 21.42
# roaring_benchmarks -c
                10.67                 1.31                 2.27                 2.42                12.43                29.57                  1.46                  2.03                 34.73
# ewah32_benchmarks
                 2.63                 0.99                 3.30               145.00                20.46               296.92                 7.29                 5.93                 14.30
# ewah64_benchmarks
                 4.66                 0.61                 2.17                92.23                11.78               173.08                 2.90                 2.37                  7.66
# wah32_benchmarks
                 2.67                 1.85                 6.21               207.65                11.68               290.15                 1.63                 2.05                  8.42
# concise_benchmarks
                 2.23                 1.30                 5.00               183.10                13.03               263.12                 1.63                 2.28                 10.65
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
