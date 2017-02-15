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


Note: for some selected formats, we also include the successive differences (ANDNOT) as well as the symmetric difference (XOR), and also the time required to iterate over all btimaps.

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
                 5.66                 0.14                 0.14                 0.09                 0.09                 3.61
# stl_vector_benchmarks
                 0.00                 7.90                 7.77                43.87                16.82               424.84
# stl_vector_benchmarks_memtracked
                32.00                 7.95                 9.69                54.87                19.56               421.40
# stl_hashset_benchmarks_memtracked
               218.55                49.79               192.23                88.52               119.65                47.85
# stl_hashset_benchmarks
                 0.00                49.92               192.26                87.41               117.64                45.78
# bitmagic_benchmarks
                 8.23                 0.43                 0.44                 0.19                 0.23                13.89                 0.44                 0.44
# bitmagic_benchmarks -r
                 6.83                 0.48                 0.44                 0.16                 0.22                33.49                 0.43                 0.45
# slow_roaring_benchmarks -r
                 2.60                 0.56                 0.41                 0.06                 0.60                73.47                  0.58                  0.53
# roaring_benchmarks -r
                 2.60                 0.34                 0.27                 0.05                 0.47                66.10                  0.34                  0.37
# ewah32_benchmarks
                 3.29                 1.37                 1.96                 0.46                 1.53              3088.68
# ewah64_benchmarks
                 3.86                 0.81                 1.03                 0.25                 1.12              1734.40
# wah32_benchmarks
                 3.37                 1.79                 2.11                 1.83                 3.40             18781.43
# concise_benchmarks
                 2.94                 2.14                 2.45                 2.19                 4.05             19049.08


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.15                 0.15                 0.09                 0.10                 3.79
# stl_vector_benchmarks
                 0.00                 4.55                 5.51                37.32                10.90               643.86
# stl_vector_benchmarks_memtracked
                32.00                 4.73                 7.48                50.31                13.85               537.33
# stl_hashset_benchmarks_memtracked
               217.89                27.52               166.58                59.30                72.88                47.01
# stl_hashset_benchmarks
                 0.00                28.32               166.78                59.56                72.59                45.48
# bitmagic_benchmarks -r
                 4.57                 0.32                 0.33                 0.19                 0.25                51.59                 0.35                 0.36
# slow_roaring_benchmarks -r
                 0.60                 0.18                 0.31                 0.12                 0.03                64.13                  0.26                  0.38
# roaring_benchmarks -r
                 0.60                 0.17                 0.31                 0.11                 0.03                63.18                  0.24                  0.35
# ewah32_benchmarks
                 0.64                 0.27                 0.55                 0.15                 0.12               896.74
# ewah64_benchmarks
                 0.90                 0.20                 0.38                 0.11                 0.10               647.48
# wah32_benchmarks
                 0.65                 0.45                 0.65                 0.46                 0.46              3599.27
# concise_benchmarks
                 0.55                 0.56                 0.76                 0.59                 0.61              3770.17


# processing file  census1881
# bitset_benchmarks
               523.54                19.61                13.91                 9.87                 9.62                 2.38
# stl_vector_benchmarks
                 0.00                 3.30                 6.07               591.74                66.85               167.44
# stl_vector_benchmarks_memtracked
                32.00                 3.31                 8.12               721.34                74.42               159.67
# stl_hashset_benchmarks_memtracked
               212.26                 0.41               293.77              1237.41              1714.23                38.62
# stl_hashset_benchmarks
                 0.00                 0.54               344.40              1397.38              1705.95                36.22
# bitmagic_benchmarks -r
                62.63                 2.34                 5.28                 2.79                 4.95                11.52                 3.84                 5.32
# slow_roaring_benchmarks -r
                15.08                 0.13                 1.13                 4.10                11.13                17.51                  0.71                  1.04
# roaring_benchmarks -r
                15.08                 0.13                 1.17                 2.58                 9.14                16.90                  0.67                  1.05
# ewah32_benchmarks
                33.77                 3.28                27.46               264.30                51.67              8449.99
# ewah64_benchmarks
                43.77                 2.03                14.73                99.87                26.30              5115.59
# wah32_benchmarks
                34.32                14.37                29.60               388.06                45.18             26055.74
# concise_benchmarks
                25.56                21.00                33.78               399.81                57.75             29877.20


# processing file  census1881_srt
# bitset_benchmarks
               888.07                40.77                30.86                24.67                24.64                 2.59
# stl_vector_benchmarks
                 0.00                 3.99                 6.28               656.86                35.93               207.74
# stl_vector_benchmarks_memtracked
                32.00                 4.01                 8.41               867.98                42.18               215.52
# stl_hashset_benchmarks_memtracked
               216.84                 1.49               219.31               423.03               474.66                36.45
# stl_hashset_benchmarks
                 0.00                 1.44               220.04               422.29               478.08                35.04
# bitmagic_benchmarks -r
                40.05                 1.78                 3.05                 4.64                 3.83                11.14                 3.71                 3.16
# slow_roaring_benchmarks -r
                 2.16                 0.14                 1.08                 3.27                 3.85                17.65                  0.57                  0.95
# roaring_benchmarks -r
                 2.16                 0.18                 1.11                 3.20                 3.86                18.78                  0.59                  0.97
# ewah32_benchmarks
                 2.91                 0.45                 2.44               166.89                 5.13               530.88
# ewah64_benchmarks
                 4.54                 0.36                 2.04               121.30                 4.34               437.56
# wah32_benchmarks
                 2.95                 1.57                 3.03               197.89                 5.76              1769.16
# concise_benchmarks
                 2.48                 1.91                 3.39               222.00                 6.66              1861.66


# processing file  weather_sept_85
# bitset_benchmarks
                15.26                 0.59                 0.57                 0.61                 0.63                 3.58
# stl_vector_benchmarks
                 0.00                 7.57                 8.39               105.51                29.54               899.47
# stl_vector_benchmarks_memtracked
                32.00                 7.50                10.86               127.69                32.90               904.89
# stl_hashset_benchmarks_memtracked
               219.92                34.25               286.69               246.48               319.42                54.42
# stl_hashset_benchmarks
                 0.00                39.11               308.20               311.13               352.41                52.49
# bitmagic_benchmarks -r
                10.06                 0.84                 0.86                 0.44                 0.56                27.40                 0.82                 0.87
# slow_roaring_benchmarks -r
                 5.38                 1.15                 0.91                 0.27                 1.47                90.51                  1.21                  1.10
# roaring_benchmarks -r
                 5.38                 0.55                 0.57                 0.16                 1.06                78.11                  0.52                  0.76
# ewah32_benchmarks
                 6.67                 2.56                 4.29                 2.23                 4.39             13142.54
# ewah64_benchmarks
                 7.87                 1.50                 2.17                 1.26                 2.22              7306.96
# wah32_benchmarks
                 6.82                 3.55                 4.61                 4.91                 6.85             62945.58
# concise_benchmarks
                 5.88                 4.53                 5.44                 6.12                 8.52             68348.42


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.31                 0.29                 0.25                 0.26                 3.62
# stl_vector_benchmarks
                 0.00                 4.39                 6.52                71.05                14.09               661.29
# stl_vector_benchmarks_memtracked
                32.00                 4.45                 8.01                84.52                17.43               648.21
# stl_hashset_benchmarks_memtracked
               219.63                13.94               191.10                80.83               112.83                48.55
# stl_hashset_benchmarks
                 0.00                13.93               188.80                74.66               109.25                49.78
# bitmagic_benchmarks -r
                 1.98                 0.19                 0.22                 0.21                 0.34                42.09                 0.25                 0.25
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                56.87                  0.15                  0.22
# roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.04                 0.05                54.16                  0.15                  0.22
# ewah32_benchmarks
                 0.54                 0.18                 0.47                 0.30                 0.10              2134.29
# ewah64_benchmarks
                 0.86                 0.15                 0.39                 0.23                 0.08              1792.34
# wah32_benchmarks
                 0.54                 0.34                 0.55                 0.58                 0.37              7187.56
# concise_benchmarks
                 0.43                 0.41                 0.62                 0.69                 0.44              7255.16


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                25.96                20.02                15.25                15.71                 3.70
# stl_vector_benchmarks
                 0.00                 4.17                 7.09               517.25                51.78               162.90
# stl_vector_benchmarks_memtracked
                32.00                 4.09                 9.30               696.16                62.35               167.07
# stl_hashset_benchmarks_memtracked
               226.35                13.09               228.93               389.72               514.31                43.90
# stl_hashset_benchmarks
                 0.00                12.69               228.78               385.24               511.72                43.48
# bitmagic_benchmarks -r
                89.29                 4.63                 6.64                12.18                17.12                21.22                 7.78                 7.28
# slow_roaring_benchmarks -r
                 5.89                 1.51                 4.07                 2.75                24.42                28.42                  2.69                  3.92
# roaring_benchmarks -r
                 5.89                 1.51                 4.12                 2.71                24.37                27.51                  2.72                  3.92
# ewah32_benchmarks
                10.83                 2.68                 8.89               311.74                47.17               973.89
# ewah64_benchmarks
                19.41                 2.56                 8.32               191.55                38.88               975.16
# wah32_benchmarks
                10.89                 6.01                10.75               415.10                46.42              2280.43
# concise_benchmarks
                10.25                 6.44                11.30               444.45                52.39              2300.83


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.72                18.88                14.58                15.29                 2.94
# stl_vector_benchmarks
                 0.00                 4.16                 7.22               497.70                43.27               153.42
# stl_vector_benchmarks_memtracked
                32.00                 4.28                 9.56               676.04                53.54               147.12
# stl_hashset_benchmarks_memtracked
               228.86                24.69               178.18               270.02               360.99                43.88
# stl_hashset_benchmarks
                 0.00                25.06               176.82               270.77               362.73                42.64
# bitmagic_benchmarks -r
                80.83                 3.55                 5.11                 8.89                 7.26                15.06                 5.88                 5.59
# slow_roaring_benchmarks -r
                 1.63                 0.68                 2.29                 1.87                 7.36                20.94                  1.28                  1.87
# roaring_benchmarks -r
                 1.63                 0.69                 2.39                 1.79                 7.32                20.04                  1.34                  2.02
# ewah32_benchmarks
                 2.63                 0.69                 2.29               113.61                12.31               234.11
# ewah64_benchmarks
                 4.66                 0.68                 2.11                89.05                10.74               228.68
# wah32_benchmarks
                 2.67                 1.26                 2.52               132.85                12.34               507.84
# concise_benchmarks
                 2.23                 1.40                 2.62               148.78                13.75               494.27
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
