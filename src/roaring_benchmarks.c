#define __STDC_FORMAT_MACROS 1
#define _GNU_SOURCE

#ifdef RECORD_MALLOCS
#include "cmemcounter.h"
#endif

#include <inttypes.h>
#include "benchmark.h"
#include "numbersfromtextfiles.h"
#include "roaring.c"


/**
 * Once you have collected all the integers, build the bitmaps.
 */
static roaring_bitmap_t **create_all_bitmaps(size_t *howmany,
        uint32_t **numbers, size_t count, bool runoptimize, bool copyonwrite, bool verbose, uint64_t * totalsize) {
    *totalsize = 0;
    if (numbers == NULL) return NULL;
    size_t savedmem = 0;
#ifdef RECORD_MALLOCS
    size_t totalmalloced = 0;
#endif
    roaring_bitmap_t **answer = malloc(sizeof(roaring_bitmap_t *) * count);
#ifdef RECORD_MALLOCS
    size_t bef = malloced_memory_usage;
#endif
    for (size_t i = 0; i < count; i++) {
        answer[i] = roaring_bitmap_of_ptr(howmany[i], numbers[i]);
        answer[i]->copy_on_write = copyonwrite;
        if(runoptimize) roaring_bitmap_run_optimize(answer[i]);
        savedmem += roaring_bitmap_shrink_to_fit(answer[i]);
        *totalsize += roaring_bitmap_portable_size_in_bytes(answer[i]);
    }
#ifdef RECORD_MALLOCS
    size_t aft = malloced_memory_usage;
    totalmalloced += aft - bef;
    if(verbose) printf("total malloc: %zu vs. reported %llu (%f %%) \n",totalmalloced,(unsigned long long)*totalsize,(totalmalloced-*totalsize)*100.0/ *totalsize);
    *totalsize = totalmalloced;
#endif
    if(verbose) printf("saved bytes by shrinking : %zu \n",savedmem);
    return answer;
}

static void printusage(char *command) {
    printf(
        " Try %s directory \n where directory could be "
        "benchmarks/realdata/census1881\n",
        command);
    ;
    printf("the -r flag turns on run optimization");
    printf("the -c flag turns on copy-on-write");
    printf("the -v flag turns on verbose mode");

}


int main(int argc, char **argv) {
    int c;
    bool runoptimize = false;
    bool verbose = false;
    bool copyonwrite = false;
    char *extension = ".txt";
    uint64_t data[9];
    while ((c = getopt(argc, argv, "cvre:h")) != -1) switch (c) {
        case 'e':
            extension = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        case 'r':
            runoptimize = true;
            if(verbose) printf("enabling run optimization\n");
            break;
        case 'c':
            copyonwrite = true;
            if(verbose) printf("enabling copyonwrite\n");
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
    uint64_t totalsize = 0;
    roaring_bitmap_t **bitmaps = create_all_bitmaps(howmany, numbers, count,runoptimize,copyonwrite, verbose, &totalsize);
    RDTSC_FINAL(cycles_final);
    if (bitmaps == NULL) return -1;
    if(verbose) printf("Loaded %d bitmaps from directory %s \n", (int)count, dirname);
    data[0] = totalsize;
    if(verbose) printf("Total size in bytes =  %" PRIu64 " \n", totalsize);
    uint64_t successive_and = 0;
    uint64_t successive_or = 0;
    uint64_t total_or = 0;
    uint64_t total_count = 0;

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        roaring_bitmap_t *tempand =
            roaring_bitmap_and(bitmaps[i], bitmaps[i + 1]);
        successive_and += roaring_bitmap_get_cardinality(tempand);
        roaring_bitmap_free(tempand);
    }
    RDTSC_FINAL(cycles_final);
    data[1] = cycles_final - cycles_start;

    if(verbose) printf("Successive intersections on %zu bitmaps took %" PRIu64 " cycles\n", count,
           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        roaring_bitmap_t *tempor =
            roaring_bitmap_or(bitmaps[i], bitmaps[i + 1]);
        successive_or += roaring_bitmap_get_cardinality(tempor);
        roaring_bitmap_free(tempor);
    }
    RDTSC_FINAL(cycles_final);
    if(verbose) printf("Successive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
           cycles_final - cycles_start);
    data[2] = cycles_final - cycles_start;
    RDTSC_START(cycles_start);
    roaring_bitmap_t * totalorbitmap = roaring_bitmap_or_many(count,(const roaring_bitmap_t **)bitmaps);
    total_or = roaring_bitmap_get_cardinality(totalorbitmap);
    roaring_bitmap_free(totalorbitmap);
    RDTSC_FINAL(cycles_final);
    if(verbose) printf("Total unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
           cycles_final - cycles_start);
    data[3] = cycles_final - cycles_start;
    RDTSC_START(cycles_start);
    roaring_bitmap_t * totalorbitmapheap = roaring_bitmap_or_many_heap(count,(const roaring_bitmap_t **)bitmaps);
    total_or = roaring_bitmap_get_cardinality(totalorbitmapheap);
    roaring_bitmap_free(totalorbitmapheap);
    RDTSC_FINAL(cycles_final);
    if(verbose) printf("Total unions with heap on %zu bitmaps took %" PRIu64 " cycles\n", count,
           cycles_final - cycles_start);
    data[4] = cycles_final - cycles_start;

    uint64_t quartcount;
    STARTBEST(quartile_test_repetitions)
    quartcount = 0;
    for (size_t i = 0; i < count ; ++i) {
      quartcount += roaring_bitmap_contains(bitmaps[i],maxvalue/4);
      quartcount += roaring_bitmap_contains(bitmaps[i],maxvalue/2);
      quartcount += roaring_bitmap_contains(bitmaps[i],3*maxvalue/4);
    }
    ENDBEST(data[5])

    if(verbose) printf("Quartile queries on %zu bitmaps took %" PRIu64 " cycles\n", count,
           data[5]);

   /***
   * For good measure, we add ANDNOT and XOR
   ***/
       uint64_t successive_andnot = 0;
       uint64_t successive_xor = 0;

       RDTSC_START(cycles_start);
       for (int i = 0; i < (int)count - 1; ++i) {
           roaring_bitmap_t *tempandnot =
               roaring_bitmap_andnot(bitmaps[i], bitmaps[i + 1]);
           successive_andnot += roaring_bitmap_get_cardinality(tempandnot);
           roaring_bitmap_free(tempandnot);
       }
       RDTSC_FINAL(cycles_final);
       data[6] = cycles_final - cycles_start;

       if(verbose) printf("Successive differences on %zu bitmaps took %" PRIu64 " cycles\n", count,
              cycles_final - cycles_start);


       RDTSC_START(cycles_start);
       for (int i = 0; i < (int)count - 1; ++i) {
           roaring_bitmap_t *tempxor =
               roaring_bitmap_xor(bitmaps[i], bitmaps[i + 1]);
           successive_xor += roaring_bitmap_get_cardinality(tempxor);
           roaring_bitmap_free(tempxor);
       }
       RDTSC_FINAL(cycles_final);
       data[7] = cycles_final - cycles_start;

       if(verbose) printf("Successive symmetric differences on %zu bitmaps took %" PRIu64 " cycles\n", count,
              cycles_final - cycles_start);
   /***
   * End of ANDNOT and XOR
   ***/

       RDTSC_START(cycles_start);
       for (size_t i = 0; i < count; ++i) {
          roaring_bitmap_t *ra = bitmaps[i];
          roaring_uint32_iterator_t *  j = roaring_create_iterator(ra);
          while(j->has_value) {
            total_count ++;
            roaring_advance_uint32_iterator(j);
          }
          roaring_free_uint32_iterator(j);
       }
       RDTSC_FINAL(cycles_final);
       data[8] = cycles_final - cycles_start;

       if(verbose) printf("Iterating over %zu bitmaps took %" PRIu64 " cycles\n", count,
              cycles_final - cycles_start);

    assert(totalcard == total_count);

    if(verbose) printf("Collected stats  %" PRIu64 "  %" PRIu64 "  %" PRIu64 " %" PRIu64 "\n",successive_and,successive_or,total_or,quartcount);

    printf(" %20.2f %20.2f %20.2f %20.2f %20.2f %20.2f  %20.2f  %20.2f  %20.2f \n",
      data[0]*8.0/totalcard,
      data[1]*1.0/successivecard,
      data[2]*1.0/successivecard,
      data[3]*1.0/totalcard,
      data[4]*1.0/totalcard,
      data[5]*1.0/(3*count),
      data[6]*1.0/successivecard,
      data[7]*1.0/successivecard,
      data[8]*1.0/totalcard
    );

    for (int i = 0; i < (int)count; ++i) {
        free(numbers[i]);
        numbers[i] = NULL;  // paranoid
        roaring_bitmap_free(bitmaps[i]);
        bitmaps[i] = NULL;  // paranoid
    }
    free(bitmaps);
    free(howmany);
    free(numbers);

    return 0;
}
