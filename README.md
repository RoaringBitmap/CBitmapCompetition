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
               888.07                32.61                23.32                16.21                16.12                 3.39
# stl_vector_benchmarks
                32.00                 3.97                 7.72               764.15                39.41              2700.11
# stl_hashset_benchmarks
               216.84                 1.13               196.77               340.13               382.57                35.20
# bitmagic_benchmarks
               254.06                 6.11                19.59                 7.10                 9.89                10.42
# bitmagic_benchmarks -r
                40.05                 1.81                 3.09                 4.69                 3.71                11.35
# roaring_benchmarks -r
                 2.16                 0.14                 0.94                 4.03                 4.04                17.15
# roaring_benchmarks -c -r
                 2.16                 0.14                 0.55                 4.05                 4.12                18.61
# roaring_benchmarks
                 6.09                 0.14                 1.21                 4.52                 8.65                18.37
# roaring_benchmarks -c
                 6.09                 0.14                 0.52                 4.61                 8.73                20.70
# ewah32_benchmarks
                 2.91                 0.45                 2.65               188.28                 5.62               487.17
# ewah64_benchmarks
                 4.54                 0.40                 2.22               137.60                 4.97               368.76
# wah32_benchmarks
                 2.95                 1.50                 2.92               179.63                 5.61              1790.44
# concise_benchmarks
                 2.48                 1.89                 3.26               216.96                 6.33              1864.39


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.16                 0.16                 0.09                 0.09                 4.12
# stl_vector_benchmarks
                32.00                 4.63                 6.93                46.80                13.11             17905.73
# stl_hashset_benchmarks
               217.89                26.75               163.55                58.18                72.85                46.28
# bitmagic_benchmarks
                 8.52                 0.46                 0.49                 0.19                 0.26                14.87
# bitmagic_benchmarks -r
                 4.57                 0.35                 0.35                 0.19                 0.25                57.95
# roaring_benchmarks -r
                 0.60                 0.17                 0.30                 0.17                 0.04                42.44
# roaring_benchmarks -c -r
                 0.60                 0.17                 0.29                 0.17                 0.04                43.63
# roaring_benchmarks
                 2.99                 0.18                 0.28                 0.28                 0.60                42.48
# roaring_benchmarks -c
                 2.99                 0.19                 0.29                 0.29                 0.60                40.60
# ewah32_benchmarks
                 0.64                 0.26                 0.58                 0.15                 0.12               828.37
# ewah64_benchmarks
                 0.90                 0.20                 0.41                 0.11                 0.10               542.60
# wah32_benchmarks
                 0.65                 0.43                 0.62                 0.47                 0.47              3599.91
# concise_benchmarks
                 0.55                 0.54                 0.74                 0.55                 0.57              3797.09


# processing file  census1881
# bitset_benchmarks
               523.54                19.20                14.02                 9.76                 9.53                 3.37
# stl_vector_benchmarks
                32.00                 3.29                 7.33               640.41                73.89              4001.28
# stl_hashset_benchmarks
               212.26                 0.42               287.69              1045.55              1223.74                36.72
# bitmagic_benchmarks
               102.18                 2.78                 8.07                 2.98                 4.73                10.36
# bitmagic_benchmarks -r
                62.63                 2.00                 4.44                 2.21                 3.86                12.14
# roaring_benchmarks -r
                15.08                 0.11                 1.13                 2.60                 7.96                13.75
# roaring_benchmarks -c -r
                15.08                 0.11                 0.47                 2.64                 7.99                15.22
# roaring_benchmarks
                15.97                 0.11                 1.05                 2.69                 6.22                14.12
# roaring_benchmarks -c
                15.97                 0.11                 0.41                 2.72                 6.11                15.68
# ewah32_benchmarks
                33.77                 3.28                29.12               272.72                57.80              7831.22
# ewah64_benchmarks
                43.77                 1.80                15.27                98.33                28.48              4149.66
# wah32_benchmarks
                34.32                14.03                28.51               351.03                42.56             26010.52
# concise_benchmarks
                25.56                20.37                32.48               383.46                55.53             29897.34


# processing file  census-income
# bitset_benchmarks
                 5.66                 0.15                 0.14                 0.08                 0.08                 4.08
# stl_vector_benchmarks
                32.00                 7.87                 9.31                52.73                18.87             18314.31
# stl_hashset_benchmarks
               218.55                49.11               189.85                86.20               116.21                47.75
# bitmagic_benchmarks
                 8.23                 0.45                 0.45                 0.18                 0.23                15.16
# bitmagic_benchmarks -r
                 6.83                 0.49                 0.45                 0.16                 0.22                37.24
# roaring_benchmarks -r
                 2.60                 0.34                 0.28                 0.17                 0.48                42.53
# roaring_benchmarks -c -r
                 2.60                 0.34                 0.28                 0.17                 0.49                44.55
# roaring_benchmarks
                 2.74                 0.31                 0.26                 0.19                 0.46                43.87
# roaring_benchmarks -c
                 2.74                 0.31                 0.26                 0.19                 0.46                42.05
# ewah32_benchmarks
                 3.29                 1.34                 2.11                 0.47                 1.63              2855.30
# ewah64_benchmarks
                 3.86                 0.80                 1.11                 0.25                 1.25              1441.80
# wah32_benchmarks
                 3.37                 1.72                 2.05                 1.83                 3.25             18869.52
# concise_benchmarks
                 2.94                 2.10                 2.39                 1.99                 3.89             19296.33


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.34                 0.32                 0.26                 0.27                 3.98
# stl_vector_benchmarks
                32.00                 4.40                 7.26                77.25                16.45             52969.80
# stl_hashset_benchmarks
               219.63                13.68               186.44                72.04               108.30                47.63
# bitmagic_benchmarks
                 9.23                 0.40                 0.55                 0.23                 0.29                14.24
# bitmagic_benchmarks -r
                 1.98                 0.20                 0.23                 0.21                 0.34                49.89
# roaring_benchmarks -r
                 0.34                 0.08                 0.16                 0.15                 0.06                59.04
# roaring_benchmarks -c -r
                 0.34                 0.08                 0.15                 0.15                 0.06                73.81
# roaring_benchmarks
                 3.24                 0.12                 0.24                 0.27                 0.62                58.22
# roaring_benchmarks -c
                 3.24                 0.12                 0.21                 0.27                 0.61                72.26
# ewah32_benchmarks
                 0.54                 0.18                 0.50                 0.33                 0.10              1952.49
# ewah64_benchmarks
                 0.86                 0.16                 0.43                 0.25                 0.08              1470.15
# wah32_benchmarks
                 0.54                 0.33                 0.53                 0.56                 0.37              7312.59
# concise_benchmarks
                 0.43                 0.40                 0.60                 0.65                 0.42              7356.85


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.77                18.47                14.12                14.86                 3.77
# stl_vector_benchmarks
                32.00                 4.18                 8.68               612.84                50.00              1039.96
# stl_hashset_benchmarks
               228.86                23.83               170.18               260.57               349.41                41.80
# bitmagic_benchmarks
               381.41                13.35                25.48                 9.50                13.13                11.20
# bitmagic_benchmarks -r
                80.83                 3.95                 5.48                 9.01                 7.27                15.00
# roaring_benchmarks -r
                 1.63                 0.66                 2.12                 2.25                 7.67                21.02
# roaring_benchmarks -c -r
                 1.63                 0.65                 1.84                 2.31                 7.87                24.33
# roaring_benchmarks
                10.67                 0.81                 3.00                 3.71                13.50                23.02
# roaring_benchmarks -c
                10.67                 0.80                 2.18                 3.50                13.77                28.23
# ewah32_benchmarks
                 2.63                 0.71                 2.48               130.63                13.72               213.77
# ewah64_benchmarks
                 4.66                 0.70                 2.36               104.11                12.88               189.13
# wah32_benchmarks
                 2.67                 1.21                 2.44               120.81                11.93               520.50
# concise_benchmarks
                 2.23                 1.36                 2.48               143.50                13.24               506.72


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                26.67                20.72                14.93                14.81                 4.56
# stl_vector_benchmarks
                32.00                 4.13                 8.60               642.67                57.40              1019.88
# stl_hashset_benchmarks
               226.35                12.37               222.07               372.77               483.54                42.50
# bitmagic_benchmarks
               474.39                25.15                59.64                12.42                16.44                11.47
# bitmagic_benchmarks -r
                89.29                 5.04                 7.04                12.22                17.16                22.91
# roaring_benchmarks -r
                 5.89                 1.48                 3.66                 3.07                24.22                27.48
# roaring_benchmarks -c -r
                 5.89                 1.47                 3.29                 3.17                24.22                34.84
# roaring_benchmarks
                16.49                 1.48                 4.54                 4.42                20.17                36.51
# roaring_benchmarks -c
                16.49                 1.48                 3.59                 4.48                20.36                43.37
# ewah32_benchmarks
                10.83                 2.73                 9.75               348.16                53.38               885.04
# ewah64_benchmarks
                19.41                 2.63                 9.42               209.91                46.09               788.26
# wah32_benchmarks
                10.89                 5.67                10.41               373.74                44.62              2362.96
# concise_benchmarks
                10.25                 6.27                10.82               437.35                50.34              2358.21

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
