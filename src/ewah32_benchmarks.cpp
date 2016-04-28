#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif 

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
    uint64_t data[5];
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
        const EWAHBoolArray<uint32_t>  ** allofthem = new const EWAHBoolArray<uint32_t>* [count];
        for(int i = 0 ; i < (int) count; ++i) allofthem[i] = & bitmaps[i];
        fast_logicalor_tocontainer<uint32_t>(count, allofthem,totalorbitmap);
        total_or = totalorbitmap.numberOfOnes();
        delete[] allofthem;
    }
    RDTSC_FINAL(cycles_final);
    data[3] = cycles_final - cycles_start;
    if(verbose) printf("Total unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
           cycles_final - cycles_start);
    if(verbose) printf("Collected stats  %" PRIu64 "  %" PRIu64 "  %" PRIu64 "\n",successive_and,successive_or,total_or);
    printf(" %40" PRIu64 " %40" PRIu64 " %40" PRIu64 " %40" PRIu64 "\n",data[0],data[1],data[2],data[3]);

    for (int i = 0; i < (int)count; ++i) {
        free(numbers[i]);
        numbers[i] = NULL;  // paranoid
    }
    free(howmany);
    free(numbers);

    return 0;
}
