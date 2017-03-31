#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#ifdef __cplusplus
extern "C" {
#endif
#include "benchmark.h"
#include "numbersfromtextfiles.h"
#ifdef __cplusplus
}
#endif

#include "ewah.h" /* EWAHBoolArray */

/**
 * Once you have collected all the integers, build the bitmaps.
 */
static std::vector<EWAHBoolArray<uint32_t> > create_all_bitmaps(size_t *howmany,
        uint32_t **numbers, size_t count) {
    if (numbers == NULL) return std::vector<EWAHBoolArray<uint32_t> >();
    std::vector<EWAHBoolArray<uint32_t> > answer(count);
    for (size_t i = 0; i < count; i++) {
        EWAHBoolArray<uint32_t> & bm = answer[i];
        uint32_t * mynumbers = numbers[i];
        for(size_t j = 0; j < howmany[i] ; ++j) {
            bm.set(mynumbers[j]);
        }
        bm.trim();
    }
    return answer;
}

static void printusage(char *command) {
    printf(
        " Try %s directory \n where directory could be "
        "benchmarks/realdata/census1881\n",
        command);
    ;
    printf("the -v flag turns on verbose mode");

}

int main(int argc, char **argv) {
    int c;
    const char *extension = ".txt";
    bool verbose = false;
    uint64_t data[13];
    while ((c = getopt(argc, argv, "ve:h")) != -1) switch (c) {
        case 'e':
            extension = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        case 'h':
            printusage(argv[0]);
            return 0;
        default:
            abort();
        }
    if (optind >= argc) {
        printusage(argv[0]);
        return -1;
    }
    char *dirname = argv[optind];
    size_t count;

    size_t *howmany = NULL;
    uint32_t **numbers =
        read_all_integer_files(dirname, extension, &howmany, &count);
    if (numbers == NULL) {
        printf(
            "I could not find or load any data file with extension %s in "
            "directory %s.\n",
            extension, dirname);
        return -1;
    }
    uint32_t maxvalue = 0;
    for (size_t i = 0; i < count; i++) {
      if( howmany[i] > 0 ) {
        if(maxvalue < numbers[i][howmany[i]-1]) {
           maxvalue = numbers[i][howmany[i]-1];
         }
      }
    }
    uint64_t totalcard = 0;
    for (size_t i = 0; i < count; i++) {
      totalcard += howmany[i];
    }
    uint64_t successivecard = 0;
    for (size_t i = 1; i < count; i++) {
       successivecard += howmany[i-1] + howmany[i];
    }


    uint64_t cycles_start = 0, cycles_final = 0;

    RDTSC_START(cycles_start);
    std::vector<EWAHBoolArray<uint32_t> > bitmaps = create_all_bitmaps(howmany, numbers, count);
    RDTSC_FINAL(cycles_final);
    if (bitmaps.empty()) return -1;
    if(verbose) printf("Loaded %d bitmaps from directory %s \n", (int)count, dirname);
    uint64_t totalsize = 0;

    for (int i = 0; i < (int) count; ++i) {
        EWAHBoolArray<uint32_t> & bv = bitmaps[i];
        totalsize += bv.sizeInBytes(); // should be close enough to memory usage
    }
    data[0] = totalsize;

    if(verbose) printf("Total size in bytes =  %" PRIu64 " \n", totalsize);

    uint64_t successive_and = 0;
    uint64_t successive_or = 0;
    uint64_t total_or = 0;
    uint64_t total_count = 0;
    uint64_t successive_andnot = 0;
    uint64_t successive_xor = 0;


    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        EWAHBoolArray<uint32_t>  tempand;
        bitmaps[i].logicaland(bitmaps[i + 1],tempand);
        successive_and += tempand.numberOfOnes();
    }
    RDTSC_FINAL(cycles_final);
    data[1] = cycles_final - cycles_start;
    if(verbose) printf("Successive intersections on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        EWAHBoolArray<uint32_t>  tempor;
        bitmaps[i].logicalor(bitmaps[i + 1],tempor);
        successive_or += tempor.numberOfOnes();
    }
    RDTSC_FINAL(cycles_final);
    data[2] = cycles_final - cycles_start;
    if(verbose) printf("Successive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);
    RDTSC_START(cycles_start);
    if(count>1) {
        EWAHBoolArray<uint32_t>  totalorbitmap;
        bitmaps[0].logicalor(bitmaps[1],totalorbitmap);
        for(int i = 2 ; i < (int) count; ++i) {
          EWAHBoolArray<uint32_t>  tmp;
          totalorbitmap.logicalor(bitmaps[i],tmp);
          tmp.swap(totalorbitmap);
        }
        total_or = totalorbitmap.numberOfOnes();
    }
    RDTSC_FINAL(cycles_final);
    data[3] = cycles_final - cycles_start;
    if(verbose) printf("Total naive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    if(count>1) {
        EWAHBoolArray<uint32_t>  totalorbitmap;
        const EWAHBoolArray<uint32_t>  ** allofthem = new const EWAHBoolArray<uint32_t>* [count];
        for(int i = 0 ; i < (int) count; ++i) allofthem[i] = & bitmaps[i];
        fast_logicalor_tocontainer<uint32_t>(count, allofthem,totalorbitmap);
        total_or = totalorbitmap.numberOfOnes();
        delete[] allofthem;
    }
    RDTSC_FINAL(cycles_final);
    data[4] = cycles_final - cycles_start;
    if(verbose) printf("Total heap unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    uint64_t quartcount;
    STARTBEST(quartile_test_repetitions)
    quartcount = 0;
    for (size_t i = 0; i < count ; ++i) {
      quartcount += bitmaps[i].get(maxvalue/4);
      quartcount += bitmaps[i].get(maxvalue/2);
      quartcount += bitmaps[i].get(3*maxvalue/4);
    }
    ENDBEST(data[5])

    if(verbose) printf("Quartile queries on %zu bitmaps took %" PRIu64 " cycles\n", count,
           data[5]);

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        EWAHBoolArray<uint32_t>  tempandnot;
        bitmaps[i].logicalandnot(bitmaps[i + 1],tempandnot);
        successive_andnot += tempandnot.numberOfOnes();
    }
    RDTSC_FINAL(cycles_final);
    data[6] = cycles_final - cycles_start;

    if(verbose) printf("Successive differences on %zu bitmaps took %" PRIu64 " cycles\n", count,
           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        EWAHBoolArray<uint32_t>  tempxor;
        bitmaps[i].logicalxor(bitmaps[i + 1],tempxor);
        successive_xor += tempxor.numberOfOnes();
    }
    RDTSC_FINAL(cycles_final);
    data[7] = cycles_final - cycles_start;

    if(verbose) printf("Successive symmetric differences on %zu bitmaps took %" PRIu64 " cycles\n", count,
           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    for (size_t i = 0; i < count; ++i) {
        EWAHBoolArray<uint32_t> & b = bitmaps[i];
        for (auto j = b.begin(); j != b.end(); ++j) {
            total_count++;
        }
    }
    RDTSC_FINAL(cycles_final);
    data[8] = cycles_final - cycles_start;
    assert(total_count == totalcard);

    if(verbose) printf("Iterating over %zu bitmaps took %" PRIu64 " cycles\n", count,
           cycles_final - cycles_start);

    if(verbose) printf("Collected stats  %" PRIu64 "  %" PRIu64 "  %" PRIu64 " %" PRIu64 "\n",successive_and,successive_or,total_or,quartcount);

    assert(successive_xor + successive_and == successive_or);

    /**
    * and, or, andnot and xor cardinality
    */
    uint64_t successive_andcard = 0;
    uint64_t successive_orcard = 0;
    uint64_t successive_andnotcard = 0;
    uint64_t successive_xorcard = 0;
    
    for (int i = 0; i < (int)count - 1; ++i) {
        assert(bitmaps[i].logicalandcount(bitmaps[i + 1]) == bitmaps[i].logicaland(bitmaps[i + 1]).numberOfOnes());
        assert(bitmaps[i].logicalorcount(bitmaps[i + 1]) == bitmaps[i].logicalor(bitmaps[i + 1]).numberOfOnes());
        assert(bitmaps[i].logicalxorcount(bitmaps[i + 1]) == bitmaps[i].logicalxor(bitmaps[i + 1]).numberOfOnes());
        assert(bitmaps[i].logicalandnotcount(bitmaps[i + 1]) == bitmaps[i].logicalandnot(bitmaps[i + 1]).numberOfOnes());
    }

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        successive_andcard += bitmaps[i].logicalandcount(bitmaps[i + 1]);
    }
    RDTSC_FINAL(cycles_final);
    data[9] = cycles_final - cycles_start;

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        successive_orcard += bitmaps[i].logicalorcount(bitmaps[i + 1]);
    }
    RDTSC_FINAL(cycles_final);
    data[10] = cycles_final - cycles_start;

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        successive_andnotcard += bitmaps[i].logicalandnotcount(bitmaps[i + 1]);
    }
    RDTSC_FINAL(cycles_final);
    data[11] = cycles_final - cycles_start;

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        successive_xorcard += bitmaps[i].logicalxorcount(bitmaps[i + 1]);
    }
    RDTSC_FINAL(cycles_final);
    data[12] = cycles_final - cycles_start;

    assert(successive_andcard == successive_and);
    assert(successive_xorcard == successive_xor);
    assert(successive_andnotcard == successive_andnot);
    assert(successive_orcard == successive_or);

    /**
    * end and, or, andnot and xor cardinality
    */

    printf(" %20.2f %20.2f %20.2f %20.2f %20.2f %20.2f  %20.2f  %20.2f     %20.2f    %20.2f  %20.2f  %20.2f  %20.2f\n",
      data[0]*8.0/totalcard,
      data[1]*1.0/successivecard,
      data[2]*1.0/successivecard,
      data[3]*1.0/totalcard,
      data[4]*1.0/totalcard,
      data[5]*1.0/(3*count),
      data[6]*1.0/successivecard,
      data[7]*1.0/successivecard,
      data[8]*1.0/totalcard,
      data[9]*1.0/successivecard,
      data[10]*1.0/successivecard,
      data[11]*1.0/successivecard,
      data[12]*1.0/successivecard
    );
    for (int i = 0; i < (int)count; ++i) {
        free(numbers[i]);
        numbers[i] = NULL;  // paranoid
    }
    free(howmany);
    free(numbers);

    return 0;
}
