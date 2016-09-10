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
               888.07                32.62                23.32                16.42                16.14                 3.42
# stl_vector_benchmarks
                32.00                 3.95                 7.71               765.14                39.59              2694.59
# stl_hashset_benchmarks
               216.84                 1.12               196.81               338.56               381.25                35.00
# bitmagic_benchmarks
               254.06                 6.08                19.63                 7.13                 9.81                10.46
# bitmagic_benchmarks -r
                40.05                 1.82                 3.09                 4.68                 3.72                11.35
# roaring_benchmarks -r
                 2.16                 0.14                 0.94                 3.55                 3.82                21.82
# roaring_benchmarks -c -r
                 2.16                 0.14                 0.55                 3.54                 3.91                21.59
# roaring_benchmarks
                 6.09                 0.15                 1.21                 3.93                 8.42                20.73
# roaring_benchmarks -c
                 6.09                 0.15                 0.53                 4.09                 8.41                23.60
# ewah32_benchmarks
                 2.91                 0.45                 2.65               188.42                 5.65               480.38
# ewah64_benchmarks
                 4.54                 0.37                 2.22               137.55                 5.01               369.14
# wah32_benchmarks
                 2.95                 1.50                 2.91               179.65                 5.60              1793.21
# concise_benchmarks
                 2.48                 1.89                 3.27               216.75                 6.34              1887.63


# processing file  census-income_srt
# bitset_benchmarks
                 6.01                 0.16                 0.16                 0.09                 0.09                 4.16
# stl_vector_benchmarks
                32.00                 4.62                 6.86                46.53                13.06             17834.55
# stl_hashset_benchmarks
               217.89                26.55               162.19                57.93                71.44                46.61
# bitmagic_benchmarks
                 8.52                 0.46                 0.48                 0.19                 0.24                15.27
# bitmagic_benchmarks -r
                 4.57                 0.35                 0.35                 0.19                 0.25                60.63
# roaring_benchmarks -r
                 0.60                 0.17                 0.31                 0.17                 0.03                68.00
# roaring_benchmarks -c -r
                 0.60                 0.17                 0.30                 0.17                 0.03                71.34
# roaring_benchmarks
                 2.99                 0.18                 0.28                 0.28                 0.58                63.48
# roaring_benchmarks -c
                 2.99                 0.18                 0.29                 0.28                 0.58                65.72
# ewah32_benchmarks
                 0.64                 0.26                 0.58                 0.15                 0.12               827.67
# ewah64_benchmarks
                 0.90                 0.20                 0.41                 0.11                 0.10               542.23
# wah32_benchmarks
                 0.65                 0.43                 0.62                 0.47                 0.47              3619.54
# concise_benchmarks
                 0.55                 0.54                 0.74                 0.55                 0.57              3796.65


# processing file  census1881
# bitset_benchmarks
               523.54                19.20                13.98                 9.77                 9.55                 3.37
# stl_vector_benchmarks
                32.00                 3.28                 7.35               641.20                73.94              4002.67
# stl_hashset_benchmarks
               212.26                 0.42               304.63              1049.15              1226.12                36.12
# bitmagic_benchmarks
               102.18                 2.76                 8.07                 2.98                 4.75                10.36
# bitmagic_benchmarks -r
                62.63                 2.01                 4.45                 2.22                 3.88                11.98
# roaring_benchmarks -r
                15.08                 0.12                 1.15                 2.50                 8.30                16.61
# roaring_benchmarks -c -r
                15.08                 0.12                 0.48                 2.53                 8.24                18.02
# roaring_benchmarks
                15.97                 0.11                 1.07                 2.63                 6.22                16.78
# roaring_benchmarks -c
                15.97                 0.11                 0.41                 2.65                 6.02                17.88
# ewah32_benchmarks
                33.77                 3.29                29.23               272.81                57.72              7860.26
# ewah64_benchmarks
                43.77                 1.80                15.27                98.47                28.47              4143.68
# wah32_benchmarks
                34.32                13.90                28.52               350.90                42.44             26124.10
# concise_benchmarks
                25.56                20.43                32.53               383.65                55.60             29619.32


# processing file  census-income
# bitset_benchmarks
                 5.66                 0.15                 0.14                 0.08                 0.09                 4.18
# stl_vector_benchmarks
                32.00                 7.83                 9.18                52.12                18.84             18276.04
# stl_hashset_benchmarks
               218.55                49.07               188.24                85.06               117.21                45.95
# bitmagic_benchmarks
                 8.23                 0.45                 0.45                 0.18                 0.25                14.40
# bitmagic_benchmarks -r
                 6.83                 0.50                 0.46                 0.16                 0.22                39.96
# roaring_benchmarks -r
                 2.60                 0.35                 0.28                 0.17                 0.49                58.83
# roaring_benchmarks -c -r
                 2.60                 0.34                 0.28                 0.17                 0.49                59.84
# roaring_benchmarks
                 2.74                 0.31                 0.26                 0.19                 0.46                59.99
# roaring_benchmarks -c
                 2.74                 0.31                 0.26                 0.19                 0.46                61.94
# ewah32_benchmarks
                 3.29                 1.34                 2.10                 0.47                 1.64              2851.43
# ewah64_benchmarks
                 3.86                 0.80                 1.11                 0.25                 1.25              1442.22
# wah32_benchmarks
                 3.37                 1.72                 2.05                 1.82                 3.24             18882.70
# concise_benchmarks
                 2.94                 2.10                 2.39                 1.99                 3.89             19294.65


# processing file  weather_sept_85_srt
# bitset_benchmarks
                11.39                 0.33                 0.31                 0.24                 0.25                 4.13
# stl_vector_benchmarks
                32.00                 4.40                 7.33                77.63                16.40             52961.66
# stl_hashset_benchmarks
               219.63                13.59               185.42                71.73               105.68                47.62
# bitmagic_benchmarks
                 9.23                 0.40                 0.55                 0.24                 0.30                13.55
# bitmagic_benchmarks -r
                 1.98                 0.20                 0.23                 0.21                 0.34                49.91
# roaring_benchmarks -r
                 0.34                 0.09                 0.17                 0.14                 0.05                62.58
# roaring_benchmarks -c -r
                 0.34                 0.09                 0.16                 0.14                 0.06                71.55
# roaring_benchmarks
                 3.24                 0.13                 0.25                 0.25                 0.57                59.25
# roaring_benchmarks -c
                 3.24                 0.13                 0.24                 0.26                 0.58                72.85
# ewah32_benchmarks
                 0.54                 0.18                 0.50                 0.32                 0.10              1954.17
# ewah64_benchmarks
                 0.86                 0.16                 0.43                 0.25                 0.08              1468.79
# wah32_benchmarks
                 0.54                 0.33                 0.54                 0.56                 0.37              7315.13
# concise_benchmarks
                 0.43                 0.40                 0.60                 0.65                 0.42              7357.95


# processing file  wikileaks-noquotes_srt
# bitset_benchmarks
               647.53                19.82                18.66                14.24                15.06                 3.72
# stl_vector_benchmarks
                32.00                 4.17                 8.72               614.41                50.13              1045.53
# stl_hashset_benchmarks
               228.86                23.91               173.58               260.35               351.97                41.57
# bitmagic_benchmarks
               381.41                13.46                25.83                 9.59                13.08                11.51
# bitmagic_benchmarks -r
                80.83                 3.95                 5.50                 9.01                 7.28                15.58
# roaring_benchmarks -r
                 1.63                 0.68                 2.14                 1.70                 7.50                22.58
# roaring_benchmarks -c -r
                 1.63                 0.68                 1.88                 1.75                 7.72                26.42
# roaring_benchmarks
                10.67                 0.81                 3.03                 2.96                13.58                26.22
# roaring_benchmarks -c
                10.67                 0.80                 2.27                 2.97                13.58                31.41
# ewah32_benchmarks
                 2.63                 0.72                 2.49               130.72                13.73               213.14
# ewah64_benchmarks
                 4.66                 0.71                 2.38               104.51                12.86               188.62
# wah32_benchmarks
                 2.67                 1.21                 2.43               120.50                11.89               525.76
# concise_benchmarks
                 2.23                 1.36                 2.47               143.62                13.24               509.71


# processing file  wikileaks-noquotes
# bitset_benchmarks
               795.50                27.23                21.45                15.86                15.88                 4.64
# stl_vector_benchmarks
                32.00                 4.09                 8.62               641.34                57.15               995.04
# stl_hashset_benchmarks
               226.35                12.41               222.26               373.02               487.49                42.89
# bitmagic_benchmarks
               474.39                25.20                59.71                12.11                16.35                11.56
# bitmagic_benchmarks -r
                89.29                 5.03                 7.06                12.21                17.12                23.70
# roaring_benchmarks -r
                 5.89                 1.49                 3.66                 2.81                24.24                31.16
# roaring_benchmarks -c -r
                 5.89                 1.48                 3.31                 2.88                24.31                36.29
# roaring_benchmarks
                16.49                 1.44                 4.53                 4.17                20.43                35.54
# roaring_benchmarks -c
                16.49                 1.45                 3.63                 4.20                20.52                43.66
# ewah32_benchmarks
                10.83                 2.75                 9.78               348.50                53.37               888.46
# ewah64_benchmarks
                19.41                 2.63                 9.45               209.79                46.13               787.59
# wah32_benchmarks
                10.89                 5.67                10.40               373.89                44.61              2373.99
# concise_benchmarks
                10.25                 6.29                10.77               437.78                50.44              2379.50
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
