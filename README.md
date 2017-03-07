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

Thus, we output at least 5 numbers:
- The memory usage, expressed as a floating-point numbers representing the number of bits used by value. In practice, this is computed by counting the number of bits used in RAM divided by the total cardinality. The total cardinality is given by the sum of the cardinalities of all sets. Thus if you have 200 sets each containing 10 values, your total cardinality is 2000, if you use 2000 bytes of memory, then your number of bits per value will be 8.0.
- The number of CPU cycles used to compute the successive intersections, normalized by the number of input values.
- The number of CPU cycles used to compute the successive unions, normalized by the number of input values.
- The number of CPU cycles used to compute the total union using a naive algorithm, normalized by the number of input values.
- The number of CPU cycles used to compute the total union using a heap-based or sort algorithm, normalized by the number of input values. In some cases, there might not be a reasonable heap-based or sort algorithm, but that is ok.
- The number of CPU cycles to for each quartile query.
- The number of CPU cycles used to compute the successive differences, normalized by the number of input values.
- The number of CPU cycles used to compute the successive symmetric differences, normalized by the number of input values.
- The number of CPU cycles used to iterate through all values, normalized by the number of input values. 
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
                 5.66                 0.13                 0.13                 0.09                 0.09                 3.67                 0.12                 0.12                 13.08
# stl_vector_benchmarks
                 0.00                 7.73                 7.71                43.10                16.59               439.67                 8.92                 8.81                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 7.93                 9.77                55.32                19.67               425.85                 9.85                10.76                  0.00
# stl_hashset_benchmarks_memtracked
               218.55                49.76               190.11                87.59               118.73                48.67               161.29               269.03                 23.27
# stl_hashset_benchmarks
                 0.00                49.96               190.40                87.89               123.77                47.03               161.26               268.83                 23.61
# bitmagic_benchmarks
                 8.23                 0.43                 0.44                 0.19                 0.24                14.06                 0.44                 0.45                 22.27
# bitmagic_benchmarks -r
                 6.83                 0.48                 0.44                 0.16                 0.22                37.36                 0.44                 0.45                 20.98
# slow_roaring_benchmarks -r
                 2.60                 0.45                 0.41                 0.06                 0.62                81.89                  0.53                  0.53                 16.70
# malloced_roaring_benchmarks -r
                 2.62                 0.33                 0.36                 0.05                 0.54                60.44                  0.38                  0.45                 16.80
# roaring_benchmarks -r
                 2.60                 0.23                 0.27                 0.05                 0.47                58.60                  0.29                  0.37                 16.70
# roaring_benchmarks -c -r
                 2.60                 0.23                 0.28                 0.05                 0.47                63.36                  0.28                  0.37                 16.71
# roaring_benchmarks
                 2.74                 0.21                 0.25                 0.08                 0.47                59.42                  0.23                  0.33                 17.15
# roaring_benchmarks -c
                 2.74                 0.20                 0.25                 0.08                 0.47                60.06                  0.22                  0.32                 17.17
# ewah32_benchmarks
                 3.29                 1.38                 1.95                 0.46                 1.53              3089.28                 2.26                 1.93                  8.77
# ewah64_benchmarks
                 3.86                 0.81                 1.03                 0.25                 1.12              1733.34                 1.29                 1.02                  7.78
# wah32_benchmarks
                 3.37                 1.80                 2.11                 1.83                 3.40             18962.61                 1.76                 2.04                  9.24
# concise_benchmarks
                 2.94                 2.14                 2.44                 2.25                 3.85             19056.72                 2.06                 2.38                  9.07


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.15                 0.14                 0.10                 0.10                 3.68                 0.12                 0.13                 12.50
# stl_vector_benchmarks
                 0.00                 4.54                 5.50                37.37                10.85               574.29                 6.18                 6.04                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.71                 7.48                50.74                13.83               538.82                 6.99                 8.01                  0.00
# stl_hashset_benchmarks_memtracked
               217.89                27.21               164.34                59.75                73.94                48.55               126.69               217.54                 12.73
# stl_hashset_benchmarks
                 0.00                27.19               164.23                59.54                73.59                47.25               126.55               217.58                 12.78
# bitmagic_benchmarks
                 8.52                 0.44                 0.47                 0.19                 0.25                14.77                 0.46                 0.48                 11.28
# bitmagic_benchmarks -r
                 4.57                 0.33                 0.33                 0.19                 0.25                59.60                 0.35                 0.36                  6.53
# slow_roaring_benchmarks -r
                 0.60                 0.18                 0.31                 0.12                 0.03                64.66                  0.26                  0.37                  8.43
# malloced_roaring_benchmarks -r
                 0.62                 0.26                 0.42                 0.11                 0.04                62.24                  0.34                  0.46                  7.51
# roaring_benchmarks -r
                 0.60                 0.17                 0.30                 0.11                 0.03                66.07                  0.25                  0.35                  8.43
# roaring_benchmarks -c -r
                 0.60                 0.17                 0.30                 0.11                 0.03                73.82                  0.24                  0.34                  8.43
# roaring_benchmarks
                 2.99                 0.17                 0.28                 0.09                 0.55                58.30                  0.20                  0.32                 16.89
# roaring_benchmarks -c
                 2.99                 0.17                 0.29                 0.09                 0.54                66.20                  0.19                  0.31                 16.86
# ewah32_benchmarks
                 0.64                 0.27                 0.54                 0.15                 0.12               896.90                 0.56                 0.61                  4.70
# ewah64_benchmarks
                 0.90                 0.20                 0.38                 0.11                 0.09               646.01                 0.42                 0.42                  4.64
# wah32_benchmarks
                 0.65                 0.45                 0.65                 0.47                 0.47              3610.19                 0.46                 0.66                  7.63
# concise_benchmarks
                 0.55                 0.56                 0.76                 0.60                 0.61              3764.73                 0.56                 0.79                  7.57


# processing file  census1881
# bitset_benchmarks
               523.54                19.60                13.67                 9.87                 9.62                 2.41                11.15                13.31                 35.55
# stl_vector_benchmarks
                 0.00                 3.30                 6.10               545.75                66.51               178.45                 5.88                 6.43                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 3.29                 8.12               718.74                75.37               165.93                 6.84                 8.55                  0.00
# stl_hashset_benchmarks_memtracked
               212.26                 0.42               294.90              1011.54              1220.37                38.74               221.34               382.77                 57.69
# stl_hashset_benchmarks
                 0.00                 0.42               297.02              1014.92              1225.76                38.30               221.95               385.98                 57.92
# bitmagic_benchmarks
               102.18                 2.70                 8.06                 3.05                 4.87                10.12                 4.87                 8.16                 88.14
# bitmagic_benchmarks -r
                62.63                 1.91                 4.32                 2.21                 3.94                11.38                 3.29                 4.38                 70.76
# slow_roaring_benchmarks -r
                15.08                 0.12                 1.09                 4.52                11.10                17.31                  0.63                  0.98                  9.09
# malloced_roaring_benchmarks -r
                15.35                 0.30                 2.79                 2.70                10.36                17.40                  1.57                  2.63                 10.08
# roaring_benchmarks -r
                15.08                 0.12                 1.11                 2.52                 8.68                16.88                  0.62                  0.99                  9.17
# roaring_benchmarks -c -r
                15.08                 0.12                 0.51                 2.53                 8.65                19.15                  0.25                  0.36                  9.17
# roaring_benchmarks
                15.97                 0.12                 1.03                 2.69                 6.20                17.71                  0.64                  1.09                  9.38
# roaring_benchmarks -c
                15.97                 0.12                 0.42                 2.69                 6.53                19.33                  0.25                  0.34                  9.38
# ewah32_benchmarks
                33.77                 3.27                27.40               264.67                50.83              8370.01                23.65                27.80                 22.26
# ewah64_benchmarks
                43.77                 1.82                14.64               100.90                26.39              4825.58                13.30                14.61                 21.43
# wah32_benchmarks
                34.32                14.47                29.29               387.97                45.00             25999.44                18.17                28.96                 29.52
# concise_benchmarks
                25.56                20.80                33.34               401.70                57.32             30001.84                21.77                34.12                 30.66


# processing file  census1881_srt
# bitset_benchmarks
               888.07                33.22                23.89                16.27                16.22                 2.67                19.62                22.71                 32.01
# stl_vector_benchmarks
                 0.00                 3.99                 6.19               623.01                35.05               162.08                 6.56                 6.47                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 3.96                 8.48               834.67                41.32               155.60                 7.54                 8.92                  0.00
# stl_hashset_benchmarks_memtracked
               216.84                 1.17               201.81               341.96               383.90                40.18               167.47               246.72                 17.84
# stl_hashset_benchmarks
                 0.00                 1.18               201.65               338.52               384.84                36.82               167.26               247.29                 17.78
# bitmagic_benchmarks
               254.06                 6.03                19.57                 7.22                10.06                 9.98                11.73                19.85                 46.49
# bitmagic_benchmarks -r
                40.05                 1.66                 2.94                 4.58                 3.68                10.86                 3.63                 3.09                 11.12
# slow_roaring_benchmarks -r
                 2.16                 0.13                 1.06                 3.27                 3.84                19.25                  0.56                  0.94                  9.54
# malloced_roaring_benchmarks -r
                 2.77                 0.51                 4.84                 3.54                 8.29                20.40                  2.63                  4.77                  8.91
# roaring_benchmarks -r
                 2.16                 0.14                 1.08                 3.18                 3.87                18.54                  0.56                  0.94                  9.52
# roaring_benchmarks -c -r
                 2.16                 0.13                 0.57                 3.26                 3.93                21.16                  0.31                  0.42                  9.54
# roaring_benchmarks
                 6.09                 0.15                 1.18                 3.98                 8.23                20.48                  0.70                  1.12                 15.36
# roaring_benchmarks -c
                 6.09                 0.15                 0.49                 4.13                 8.44                24.15                  0.30                  0.37                 15.24
# ewah32_benchmarks
                 2.91                 0.44                 2.45               167.38                 5.08               530.56                 2.31                 2.48                  6.67
# ewah64_benchmarks
                 4.54                 0.36                 2.05               122.18                 4.42               437.07                 1.94                 2.05                  6.57
# wah32_benchmarks
                 2.95                 1.62                 3.03               197.87                 5.73              1741.38                 1.88                 2.99                 10.01
# concise_benchmarks
                 2.48                 1.94                 3.37               223.38                 6.57              1863.80                 2.10                 3.39                  9.84


# processing file  weather_sept_85
# bitset_benchmarks
                15.26                 0.43                 0.41                 0.36                 0.38                 3.61                 0.40                 0.41                 14.82
# stl_vector_benchmarks
                 0.00                 7.28                 8.15                99.56                28.03               554.29                 9.05                 9.15                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 7.33                 9.73               120.20                31.35               569.07                 9.70                10.95                  0.00
# stl_hashset_benchmarks_memtracked
               219.92                30.45               254.36               214.80               242.69                52.18               212.38               382.63                 34.50
# stl_hashset_benchmarks
                 0.00                30.66               254.58               216.25               243.48                53.83               212.47               383.06                 34.67
# bitmagic_benchmarks
                15.06                 0.72                 0.80                 0.38                 0.46                13.76                 0.75                 0.82                 34.00
# bitmagic_benchmarks -r
                10.06                 0.68                 0.69                 0.28                 0.39                28.68                 0.66                 0.71                 32.05
# slow_roaring_benchmarks -r
                 5.38                 0.98                 0.90                 0.27                 1.46               104.15                  1.11                  1.06                 16.64
# malloced_roaring_benchmarks -r
                 5.42                 0.56                 0.73                 0.17                 1.30                74.93                  0.62                  0.91                 16.88
# roaring_benchmarks -r
                 5.38                 0.40                 0.58                 0.16                 1.07                73.92                  0.46                  0.77                 16.65
# roaring_benchmarks -c -r
                 5.38                 0.40                 0.57                 0.16                 1.08                83.96                  0.45                  0.75                 16.65
# roaring_benchmarks
                 5.44                 0.38                 0.55                 0.16                 1.04                79.14                  0.45                  0.75                 16.95
# roaring_benchmarks -c
                 5.44                 0.39                 0.54                 0.16                 1.04                83.08                  0.43                  0.73                 16.96
# ewah32_benchmarks
                 6.67                 2.58                 4.27                 2.22                 4.41             13194.39                 4.71                 4.32                 11.15
# ewah64_benchmarks
                 7.87                 1.50                 2.20                 1.25                 2.24              7336.60                 2.66                 2.17                  9.69
# wah32_benchmarks
                 6.82                 3.59                 4.60                 4.92                 6.88             63789.63                 3.78                 4.53                 12.46
# concise_benchmarks
                 5.88                 4.53                 5.42                 6.02                 8.10             68717.44                 4.50                 5.43                 12.37


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.33                 0.30                 0.25                 0.26                 3.58                 0.29                 0.30                 12.64
# stl_vector_benchmarks
                 0.00                 4.38                 5.86                66.18                14.08               670.22                 6.52                 6.27                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.44                 7.94                82.91                17.31               666.38                 7.30                 8.38                  0.00
# stl_hashset_benchmarks_memtracked
               219.63                13.83               186.34                74.02               108.89                47.33               142.26               243.98                 12.80
# stl_hashset_benchmarks
                 0.00                13.84               185.96                74.15               108.79                48.63               142.52               244.45                 12.75
# bitmagic_benchmarks
                 9.23                 0.40                 0.55                 0.23                 0.29                12.55                 0.47                 0.56                 10.74
# bitmagic_benchmarks -r
                 1.98                 0.19                 0.22                 0.21                 0.33                50.96                 0.24                 0.25                  6.62
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                70.10                  0.15                  0.22                  8.32
# malloced_roaring_benchmarks -r
                 0.36                 0.17                 0.30                 0.04                 0.10                58.07                  0.25                  0.35                  7.37
# roaring_benchmarks -r
                 0.34                 0.09                 0.18                 0.04                 0.05                54.63                  0.15                  0.22                  8.32
# roaring_benchmarks -c -r
                 0.34                 0.09                 0.16                 0.04                 0.05                71.25                  0.14                  0.20                  8.32
# roaring_benchmarks
                 3.24                 0.12                 0.24                 0.07                 0.49                54.40                  0.18                  0.29                 17.01
# roaring_benchmarks -c
                 3.24                 0.12                 0.23                 0.07                 0.50                74.13                  0.16                  0.25                 16.98
# ewah32_benchmarks
                 0.54                 0.18                 0.47                 0.30                 0.10              2135.74                 0.47                 0.50                  4.53
# ewah64_benchmarks
                 0.86                 0.15                 0.39                 0.23                 0.08              1790.85                 0.40                 0.42                  4.53
# wah32_benchmarks
                 0.54                 0.35                 0.55                 0.58                 0.37              7192.97                 0.38                 0.56                  7.46
# concise_benchmarks
                 0.43                 0.41                 0.62                 0.69                 0.45              7295.98                 0.43                 0.64                  7.40


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                26.60                21.10                15.44                15.94                 3.64                18.19                20.24                 37.30
# stl_vector_benchmarks
                 0.00                 4.12                 7.00               515.82                51.81               161.92                 6.70                 7.52                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.20                 9.27               688.61                61.99               183.04                 7.66                 9.72                  0.00
# stl_hashset_benchmarks_memtracked
               226.35                12.70               226.21               384.29               509.54                44.36               196.30               310.88                 20.53
# stl_hashset_benchmarks
                 0.00                12.51               226.40               387.35               509.78                44.30               197.41               311.06                 20.30
# bitmagic_benchmarks
               474.39                25.07                59.88                12.34                16.98                11.21                22.96                31.29                 92.09
# bitmagic_benchmarks -r
                89.29                 4.58                 6.58                12.06                16.87                21.81                 7.77                 7.35                 12.92
# slow_roaring_benchmarks -r
                 5.89                 1.52                 4.06                 2.78                24.86                27.53                  2.70                  3.85                 13.84
# malloced_roaring_benchmarks -r
                 7.04                 4.45                10.63                 3.03                30.53                27.99                  7.58                 10.78                 13.85
# roaring_benchmarks -r
                 5.89                 1.52                 4.09                 2.70                24.33                29.24                  2.73                  3.94                 13.81
# roaring_benchmarks -c -r
                 5.89                 1.51                 3.53                 2.77                24.48                38.40                  2.38                  3.26                 13.93
# roaring_benchmarks
                16.49                 1.47                 4.58                 4.30                20.98                38.42                  2.53                  4.77                  9.37
# roaring_benchmarks -c
                16.49                 1.47                 3.67                 4.35                20.70                47.24                  1.93                  3.63                  9.35
# ewah32_benchmarks
                10.83                 2.66                 8.98               314.50                47.16               975.01                 9.27                 9.03                 13.15
# ewah64_benchmarks
                19.41                 2.55                 8.45               191.83                39.48               976.92                 8.95                 8.33                 13.27
# wah32_benchmarks
                10.89                 6.09                10.91               415.25                46.38              2188.33                 6.78                10.70                 16.04
# concise_benchmarks
                10.25                 6.38                11.27               450.07                52.22              2294.54                 7.04                11.38                 16.21


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.73                18.53                14.53                15.27                 2.88                15.39                17.84                 29.10
# stl_vector_benchmarks
                 0.00                 4.19                 7.20               497.04                43.02               149.96                 6.50                 6.73                  0.00
# stl_vector_benchmarks_memtracked
                32.00                 4.24                 9.51               670.73                52.55               136.86                 7.35                 9.07                  0.00
# stl_hashset_benchmarks_memtracked
               228.86                24.72               174.36               268.26               359.17                42.73               156.62               229.25                 13.09
# stl_hashset_benchmarks
                 0.00                24.53               172.95               269.74               358.41                43.61               156.50               228.62                 13.18
# bitmagic_benchmarks
               381.41                13.24                25.41                 9.68                13.46                11.15                18.35                26.31                 65.97
# bitmagic_benchmarks -r
                80.83                 3.54                 5.07                 8.85                 7.23                15.32                 5.88                 5.58                  8.56
# slow_roaring_benchmarks -r
                 1.63                 0.68                 2.30                 1.87                 7.29                20.62                  1.30                  1.86                  9.04
# malloced_roaring_benchmarks -r
                 2.58                 2.55                 7.69                 2.01                12.48                22.60                  4.88                  7.43                  8.30
# roaring_benchmarks -r
                 1.63                 0.68                 2.34                 1.78                 7.28                20.39                  1.30                  2.02                  9.03
# roaring_benchmarks -c -r
                 1.63                 0.67                 1.99                 1.87                 7.60                26.32                  1.07                  1.49                  9.10
# roaring_benchmarks
                10.67                 0.78                 2.92                 3.10                13.32                24.82                  1.66                  3.06                 14.37
# roaring_benchmarks -c
                10.67                 0.78                 2.26                 3.12                13.38                29.93                  1.17                  2.13                 14.44
# ewah32_benchmarks
                 2.63                 0.69                 2.31               113.76                12.31               237.00                 2.58                 2.32                  6.05
# ewah64_benchmarks
                 4.66                 0.69                 2.16                90.68                10.96               231.74                 2.56                 2.14                  6.09
# wah32_benchmarks
                 2.67                 1.27                 2.56               132.98                12.39               497.70                 1.59                 2.51                  8.93
# concise_benchmarks
                 2.23                 1.39                 2.63               148.97                13.70               490.00                 1.69                 2.65                  8.91
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
