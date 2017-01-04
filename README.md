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


Note: for some selected formats, we also include the successive differences (ANDNOT) as well as the symmetric difference (XOR).

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
# processing file  census1881_srt
# bitset_benchmarks
               888.07                32.69                21.97                16.09                16.01                 2.76
# stl_vector_benchmarks
                 0.00                 3.95                 6.20               620.39                35.29               150.35
# stl_vector_benchmarks_memtracked
                32.00                 3.95                 8.42               842.59                41.55               149.40
# stl_hashset_benchmarks_memtracked
               216.84                 1.15               202.47               337.57               382.85                36.67
# stl_hashset_benchmarks
                 0.00                 1.14               204.85               336.55               381.86                34.64
# bitmagic_benchmarks
               254.06                 5.99                19.40                 7.13                 9.80                10.34
# bitmagic_benchmarks -r
                40.05                 1.75                 2.94                 4.58                 3.65                11.70
# slow_roaring_benchmarks -r
                 2.16                 0.14                 1.08                 3.26                 3.80                17.31
# malloced_roaring_benchmarks -r
                 2.77                 0.53                 4.84                 3.58                 8.44                18.09
# roaring_benchmarks -r
                 2.16                 0.14                 1.09                 3.19                 3.84                17.62
# roaring_benchmarks -c -r
                 2.16                 0.13                 0.57                 3.29                 3.91                19.26
# roaring_benchmarks
                 6.09                 0.14                 1.20                 3.96                 8.47                19.86
# roaring_benchmarks -c
                 6.09                 0.14                 0.50                 4.15                 8.51                21.42
# ewah32_benchmarks
                 2.91                 0.45                 2.44               166.62                 5.10               529.85
# ewah64_benchmarks
                 4.54                 0.36                 2.03               121.08                 4.32               436.92
# wah32_benchmarks
                 2.95                 1.84                 3.06               197.52                 5.74              1765.99
# concise_benchmarks
                 2.48                 1.90                 3.41               221.43                 6.62              1860.49


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.15                 0.15                 0.10                 0.10                 3.96
# stl_vector_benchmarks
                 0.00                 4.52                 5.47                37.26                10.85               557.48
# stl_vector_benchmarks_memtracked
                32.00                 4.69                 7.45                50.27                13.79               553.58
# stl_hashset_benchmarks_memtracked
               217.89                27.39               166.14                59.08                74.51                47.62
# stl_hashset_benchmarks
                 0.00                27.41               165.05                59.38                73.96                45.66
# bitmagic_benchmarks
                 8.52                 0.45                 0.47                 0.19                 0.25                14.92
# bitmagic_benchmarks -r
                 4.57                 0.34                 0.33                 0.19                 0.25                51.19
# slow_roaring_benchmarks -r
                 0.60                 0.18                 0.31                 0.19                 0.03                64.95
# malloced_roaring_benchmarks -r
                 0.62                 0.27                 0.42                 0.17                 0.04                53.73
# roaring_benchmarks -r
                 0.60                 0.17                 0.31                 0.17                 0.03                66.10
# roaring_benchmarks -c -r
                 0.60                 0.17                 0.30                 0.17                 0.03                71.85
# roaring_benchmarks
                 2.99                 0.18                 0.28                 0.28                 0.59                74.50
# roaring_benchmarks -c
                 2.99                 0.18                 0.28                 0.28                 0.59                77.78
# ewah32_benchmarks
                 0.64                 0.27                 0.55                 0.15                 0.12               896.87
# ewah64_benchmarks
                 0.90                 0.20                 0.38                 0.11                 0.10               647.64
# wah32_benchmarks
                 0.65                 0.45                 0.65                 0.47                 0.47              3599.45
# concise_benchmarks
                 0.55                 0.56                 0.76                 0.59                 0.61              3761.63


# processing file  census1881
# bitset_benchmarks
               523.54                19.58                13.41                 9.71                 9.46                 2.44
# stl_vector_benchmarks
                 0.00                 3.28                 6.08               542.32                66.75               167.34
# stl_vector_benchmarks_memtracked
                32.00                 3.28                 8.14               719.23                74.76               164.85
# stl_hashset_benchmarks_memtracked
               212.26                 0.41               294.87              1008.50              1214.96                38.24
# stl_hashset_benchmarks
                 0.00                 0.41               293.02               999.85              1191.22                36.18
# bitmagic_benchmarks
               102.18                 2.74                 7.98                 2.99                 4.77                 9.95
# bitmagic_benchmarks -r
                62.63                 1.96                 4.32                 2.23                 3.93                11.65
# slow_roaring_benchmarks -r
                15.08                 0.12                 1.08                 4.03                10.54                16.47
# malloced_roaring_benchmarks -r
                15.35                 0.30                 2.69                 2.66                10.44                17.82
# roaring_benchmarks -r
                15.08                 0.12                 1.11                 2.53                 8.56                17.40
# roaring_benchmarks -c -r
                15.08                 0.12                 0.51                 2.50                 8.57                18.78
# roaring_benchmarks
                15.97                 0.11                 1.03                 2.68                 6.06                17.28
# roaring_benchmarks -c
                15.97                 0.11                 0.41                 2.72                 6.12                18.77
# ewah32_benchmarks
                33.77                 3.28                27.23               263.46                50.94              8373.31
# ewah64_benchmarks
                43.77                 1.85                14.48                99.36                25.83              4820.32
# wah32_benchmarks
                34.32                14.29                29.41               387.30                44.77             25940.71
# concise_benchmarks
                25.56                20.85                33.71               398.36                57.26             29711.60


# processing file  census-income
# bitset_benchmarks
                 5.66                 0.14                 0.14                 0.09                 0.09                 4.02
# stl_vector_benchmarks
                 0.00                 7.84                 7.73                43.71                16.76               416.50
# stl_vector_benchmarks_memtracked
                32.00                 7.86                 9.66                54.83                19.49               411.66
# stl_hashset_benchmarks_memtracked
               218.55                49.50               192.53                87.78               121.11                48.48
# stl_hashset_benchmarks
                 0.00                49.62               192.60                88.55               121.70                46.75
# bitmagic_benchmarks
                 8.23                 0.44                 0.43                 0.18                 0.23                15.25
# bitmagic_benchmarks -r
                 6.83                 0.49                 0.43                 0.16                 0.22                36.78
# slow_roaring_benchmarks -r
                 2.60                 0.56                 0.41                 0.27                 0.62                76.44
# malloced_roaring_benchmarks -r
                 2.62                 0.44                 0.36                 0.17                 0.56                62.08
# roaring_benchmarks -r
                 2.60                 0.34                 0.27                 0.17                 0.48                63.18
# roaring_benchmarks -c -r
                 2.60                 0.34                 0.28                 0.17                 0.48                66.12
# roaring_benchmarks
                 2.74                 0.31                 0.25                 0.19                 0.46                66.36
# roaring_benchmarks -c
                 2.74                 0.31                 0.25                 0.19                 0.47                67.90
# ewah32_benchmarks
                 3.29                 1.37                 1.95                 0.46                 1.54              3089.20
# ewah64_benchmarks
                 3.86                 0.81                 1.03                 0.25                 1.11              1733.77
# wah32_benchmarks
                 3.37                 1.79                 2.11                 1.83                 3.38             18751.95
# concise_benchmarks
                 2.94                 2.14                 2.45                 2.19                 4.05             19039.90


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.32                 0.30                 0.25                 0.27                 3.79
# stl_vector_benchmarks
                 0.00                 4.36                 6.72                75.08                14.01               664.19
# stl_vector_benchmarks_memtracked
                32.00                 4.42                 7.90                83.33                17.39               679.10
# stl_hashset_benchmarks_memtracked
               219.63                13.85               188.59                73.62               108.26                48.88
# stl_hashset_benchmarks
                 0.00                13.83               189.72                73.80               108.07                46.84
# bitmagic_benchmarks
                 9.23                 0.39                 0.54                 0.23                 0.29                13.22
# bitmagic_benchmarks -r
                 1.98                 0.20                 0.22                 0.21                 0.33                45.47
# slow_roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.15                 0.05                58.34
# malloced_roaring_benchmarks -r
                 0.36                 0.16                 0.30                 0.16                 0.10                53.70
# roaring_benchmarks -r
                 0.34                 0.09                 0.18                 0.14                 0.05                51.57
# roaring_benchmarks -c -r
                 0.34                 0.09                 0.16                 0.15                 0.06                63.93
# roaring_benchmarks
                 3.24                 0.13                 0.24                 0.25                 0.58                57.63
# roaring_benchmarks -c
                 3.24                 0.13                 0.23                 0.26                 0.58                69.00
# ewah32_benchmarks
                 0.54                 0.18                 0.47                 0.30                 0.10              2134.78
# ewah64_benchmarks
                 0.86                 0.15                 0.39                 0.23                 0.08              1792.52
# wah32_benchmarks
                 0.54                 0.34                 0.55                 0.58                 0.37              7178.43
# concise_benchmarks
                 0.43                 0.41                 0.62                 0.68                 0.44              7287.08


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.80                17.98                14.46                14.94                 2.98
# stl_vector_benchmarks
                 0.00                 4.17                 7.17               496.91                43.31               170.68
# stl_vector_benchmarks_memtracked
                32.00                 4.27                 9.58               676.28                53.75               163.01
# stl_hashset_benchmarks_memtracked
               228.86                24.62               178.43               262.67               357.22                43.33
# stl_hashset_benchmarks
                 0.00                24.73               176.67               260.78               356.87                42.63
# bitmagic_benchmarks
               381.41                13.18                25.27                 9.82                13.18                11.35
# bitmagic_benchmarks -r
                80.83                 3.80                 5.08                 8.85                 7.19                14.84
# slow_roaring_benchmarks -r
                 1.63                 0.68                 2.29                 1.87                 7.24                19.52
# malloced_roaring_benchmarks -r
                 2.58                 2.52                 7.63                 2.00                12.61                20.66
# roaring_benchmarks -r
                 1.63                 0.68                 2.37                 1.75                 7.33                20.32
# roaring_benchmarks -c -r
                 1.63                 0.68                 2.00                 1.83                 7.53                25.32
# roaring_benchmarks
                10.67                 0.74                 2.97                 3.03                13.62                25.33
# roaring_benchmarks -c
                10.67                 0.74                 2.32                 3.08                13.85                30.20
# ewah32_benchmarks
                 2.63                 0.69                 2.29               113.79                12.31               233.72
# ewah64_benchmarks
                 4.66                 0.68                 2.11                88.95                10.65               229.78
# wah32_benchmarks
                 2.67                 1.26                 2.52               133.02                12.36               506.14
# concise_benchmarks
                 2.23                 1.41                 2.63               149.17                14.03               493.11


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                26.14                19.87                15.62                16.23                 4.15
# stl_vector_benchmarks
                 0.00                 4.18                 7.03               519.83                52.01               200.73
# stl_vector_benchmarks_memtracked
                32.00                 4.16                 9.27               695.20                62.65               187.60
# stl_hashset_benchmarks_memtracked
               226.35                12.47               228.19               381.41               503.96                45.15
# stl_hashset_benchmarks
                 0.00                12.55               246.69               379.17               501.97                42.67
# bitmagic_benchmarks
               474.39                24.81                59.02                12.32                16.46                12.10
# bitmagic_benchmarks -r
                89.29                 4.97                 6.59                12.09                16.97                21.41
# slow_roaring_benchmarks -r
                 5.89                 1.50                 4.06                 2.72                24.33                29.74
# malloced_roaring_benchmarks -r
                 7.04                 4.41                10.65                 3.00                30.46                30.32
# roaring_benchmarks -r
                 5.89                 1.52                 4.12                 2.62                24.29                27.52
# roaring_benchmarks -c -r
                 5.89                 1.52                 3.57                 2.73                24.52                31.24
# roaring_benchmarks
                16.49                 1.39                 4.46                 4.27                20.91                38.77
# roaring_benchmarks -c
                16.49                 1.39                 3.76                 4.35                21.34                46.23
# ewah32_benchmarks
                10.83                 2.67                 9.14               311.90                47.35               974.41
# ewah64_benchmarks
                19.41                 2.55                 8.31               191.74                38.67               976.52
# wah32_benchmarks
                10.89                 5.99                10.70               415.33                46.39              2270.84
# concise_benchmarks
                10.25                 6.43                11.27               444.22                52.48              2288.70
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
