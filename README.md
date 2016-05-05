# CBitmapCompetition

## Description
The goal of this project is to compare on realistic data different C and C++ techniques representing sets of integers.

We start from a collection of sets of sorted integers.
For example, we might have 200 sets. 
For each of these collections of sets, we do the following :  

-  We report the memory usage as best we can. Of course, data can always be compressed more or less aggressively to disk, but an important metric is how much RAM the bitmaps are using. It can be difficult to track every byte allocated, but we should do our best to get the memory usage within 1%. In C++, for STL containers, this can be done with a custom memory allocator that tracks memory allocation. 
-  Intersections between successive bitmaps, each time generating a new bitmap. Thus if there are 200 bitmaps, we generate 199 temporary bitmaps representing the intersection. We then query and check the cardinality of the each intermediate bitmap (by summing them up).
-  Union between successive bitmaps, each time generating a new bitmap. We then query and check the cardinality of the result by summing them up.
-  Finally, we compute the union of the whole set and check the cardinality of the result. This can be done in various ways. A basic way involves doing successive in-place unions which should be the default. In this instance, it is fair to avoid creating temporary bitmaps.

Thus, we output at least 5 numbers: 
- The memory usage, expressed as a floating-point numbers representing the number of bits used by value. In practice, this is computed by counting the number of bits used in RAM divided by the total cardinality. The total cardinality is given by the sum of the cardinalities of all sets. Thus if you have 200 sets each containing 10 values, your total cardinality is 2000, if you use 2000 bytes of memory, then your number of bits per value will be 8.0.
- The number of CPU cycles used to compute the successive intersections, normalized by the number of input values.
- The number of CPU cycles used to compute the successive unions, normalized by the number of input values.
- The number of CPU cycles used to compute the total union using a naive algorithm, normalized by the number of input values.
- The number of CPU cycles used to compute the total union using a heap-based or sort algorithm, normalized by the number of input values. In some cases, there might not be a reasonable heap-based or sort algorithm, but that is ok.
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
