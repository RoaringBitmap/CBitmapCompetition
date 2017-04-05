#define __STDC_FORMAT_MACROS 1
#define _GNU_SOURCE

#include <inttypes.h>
#include "benchmark.h"
#include "numbersfromtextfiles.h"
#include "roaring.c"

bool roaring_iterator_increment(uint32_t value, void *param) {
  size_t count;
  memcpy(&count, param, sizeof(size_t));
  count++;
  memcpy(param, &count, sizeof(size_t));
  (void)value;
  return true; // continue till the end
}

/**
 * Once you have collected all the integers, build the bitmaps.
 */
static roaring_bitmap_t **create_all_bitmaps(size_t *howmany,
                                             uint32_t **numbers, size_t count,
                                             bool runoptimize, bool copyonwrite,
                                             bool verbose,
                                             uint64_t *totalsize) {
  *totalsize = 0;
  if (numbers == NULL)
    return NULL;
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
    if (runoptimize)
      roaring_bitmap_run_optimize(answer[i]);
    savedmem += roaring_bitmap_shrink_to_fit(answer[i]);
    *totalsize += roaring_bitmap_portable_size_in_bytes(answer[i]);
  }
#ifdef RECORD_MALLOCS
  size_t aft = malloced_memory_usage;
  totalmalloced += aft - bef;
  if (verbose)
    printf("total malloc: %zu vs. reported %llu (%f %%) \n", totalmalloced,
           (unsigned long long)*totalsize,
           (totalmalloced - *totalsize) * 100.0 / *totalsize);
  *totalsize = totalmalloced;
#endif
  if (verbose)
    printf("saved bytes by shrinking : %zu \n", savedmem);
  return answer;
}

const char *modes =
    "intersection, intersectioncount, union, unioncount, difference, "
    "differencecount, "
    "symdifference, symdifferencecount, wideunion, wideunionheap, access";

static void printusage(char *command) {
  printf(" Try %s -m intersection directory \n where directory could be "
         "benchmarks/realdata/census1881\n",
         command);
  ;
  printf("the -m flag can be followed by one of %s \n", modes);

  printf("the -r flag turns on run optimization\n");
  printf("the -c flag turns on copy-on-write\n");
  printf("the -v flag turns on verbose mode\n");
}

int main(int argc, char **argv) {
  int c;
  bool runoptimize = false;
  bool verbose = false;
  bool copyonwrite = false;
  char *extension = ".txt";
  char *mode = "none specified";
  uint64_t cycles_start = 0, cycles_final = 0;

  RDTSC_START(cycles_start);

  while ((c = getopt(argc, argv, "cvrm:e:h")) != -1)
    switch (c) {
    case 'e':
      extension = optarg;
      break;
    case 'm':
      mode = optarg;
      if(verbose) printf("setting mode: %s \n", mode);
      break;
    case 'v':
      verbose = true;
      break;
    case 'r':
      runoptimize = true;
      if (verbose)
        printf("enabling run optimization\n");
      break;
    case 'c':
      copyonwrite = true;
      if (verbose)
        printf("enabling copyonwrite\n");
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
    printf("I could not find or load any data file with extension %s in "
           "directory %s.\n",
           extension, dirname);
    return -1;
  }
  uint32_t maxvalue = 0;
  for (size_t i = 0; i < count; i++) {
    if (howmany[i] > 0) {
      if (maxvalue < numbers[i][howmany[i] - 1]) {
        maxvalue = numbers[i][howmany[i] - 1];
      }
    }
  }
  uint64_t totalcard = 0;
  for (size_t i = 0; i < count; i++) {
    totalcard += howmany[i];
  }
  uint64_t successivecard = 0;
  for (size_t i = 1; i < count; i++) {
    successivecard += howmany[i - 1] + howmany[i];
  }
  uint64_t totalsize = 0;
  roaring_bitmap_t **bitmaps = create_all_bitmaps(
      howmany, numbers, count, runoptimize, copyonwrite, verbose, &totalsize);
  if (bitmaps == NULL)
    return -1;
  if (verbose)
    printf("Loaded %d bitmaps from directory %s \n", (int)count, dirname);
  data[0] = totalsize;
  if (verbose)
    printf("Total size in bytes =  %" PRIu64 " \n", totalsize);
  RDTSC_FINAL(cycles_final);

  const size_t init_cycles = cycles_final - cycles_start;
  if(verbose) printf("init_cycles = %zu \n", init_cycles);
  size_t main_cycles = 0;
  size_t loops = 0;
  size_t bogus = 0;
  while (main_cycles < 100 * init_cycles) {
    RDTSC_START(cycles_start);
    if (strcmp(mode, "intersection") == 0) {
      for (int i = 0; i < (int)count - 1; ++i) {
        roaring_bitmap_t *tempand =
            roaring_bitmap_and(bitmaps[i], bitmaps[i + 1]);
        bogus += roaring_bitmap_get_cardinality(tempand);
        roaring_bitmap_free(tempand);
      }
    } else if (strcmp(mode, "intersectioncount") == 0) {
      for (int i = 0; i < (int)count - 1; ++i) {
        bogus += roaring_bitmap_and_cardinality(bitmaps[i], bitmaps[i + 1]);
      }

    } else if (strcmp(mode, "union") == 0) {
      for (int i = 0; i < (int)count - 1; ++i) {
        roaring_bitmap_t *tempor =
            roaring_bitmap_or(bitmaps[i], bitmaps[i + 1]);
        bogus += roaring_bitmap_get_cardinality(tempor);
        roaring_bitmap_free(tempor);
      }
    } else if (strcmp(mode, "unioncount") == 0) {
      for (int i = 0; i < (int)count - 1; ++i) {
        bogus += roaring_bitmap_or_cardinality(bitmaps[i], bitmaps[i + 1]);
      }
    } else if (strcmp(mode, "wideunion") == 0) {
      roaring_bitmap_t *totalorbitmap =
          roaring_bitmap_or_many(count, (const roaring_bitmap_t **)bitmaps);
      bogus += roaring_bitmap_get_cardinality(totalorbitmap);
      roaring_bitmap_free(totalorbitmap);
    } else if (strcmp(mode, "wideunionheap") == 0) {
      roaring_bitmap_t *totalorbitmapheap = roaring_bitmap_or_many_heap(
          count, (const roaring_bitmap_t **)bitmaps);
      bogus += roaring_bitmap_get_cardinality(totalorbitmapheap);
      roaring_bitmap_free(totalorbitmapheap);
    } else if (strcmp(mode, "access") == 0) {
      for (size_t i = 0; i < count; ++i) {
        bogus += roaring_bitmap_contains(bitmaps[i], maxvalue / 4);
        bogus += roaring_bitmap_contains(bitmaps[i], maxvalue / 2);
        bogus += roaring_bitmap_contains(bitmaps[i], 3 * maxvalue / 4);
      }
    } else if (strcmp(mode, "difference") == 0) {
      for (int i = 0; i < (int)count - 1; ++i) {
        roaring_bitmap_t *tempandnot =
            roaring_bitmap_andnot(bitmaps[i], bitmaps[i + 1]);
        bogus += roaring_bitmap_get_cardinality(tempandnot);
        roaring_bitmap_free(tempandnot);
      }
    } else if (strcmp(mode, "differencecount") == 0) {
      for (int i = 0; i < (int)count - 1; ++i) {
        bogus += roaring_bitmap_andnot_cardinality(bitmaps[i], bitmaps[i + 1]);
      }
    } else if (strcmp(mode, "symdifference") == 0) {
      for (int i = 0; i < (int)count - 1; ++i) {
        roaring_bitmap_t *tempxor =
            roaring_bitmap_xor(bitmaps[i], bitmaps[i + 1]);
        bogus += roaring_bitmap_get_cardinality(tempxor);
        roaring_bitmap_free(tempxor);
      }
    } else if (strcmp(mode, "symdifferencecount") == 0) {
      for (int i = 0; i < (int)count - 1; ++i) {
        bogus += roaring_bitmap_xor_cardinality(bitmaps[i], bitmaps[i + 1]);
      }
    } else if (strcmp(mode, "iterate") == 0) {
      for (size_t i = 0; i < count; ++i) {
        roaring_bitmap_t *ra = bitmaps[i];
        roaring_iterate(ra, roaring_iterator_increment, &bogus);
      }
    } else /* default: */
    {
      printf("mode: %s, try one of '%s' following the -m flag\n", mode, modes);
      break;
    }
    loops++;

    RDTSC_FINAL(cycles_final);
    main_cycles += cycles_final - cycles_start;

  }

  if(verbose) printf("repeated %zu times \n", loops);
  for (int i = 0; i < (int)count; ++i) {
    free(numbers[i]);
    numbers[i] = NULL; // paranoid
    roaring_bitmap_free(bitmaps[i]);
    bitmaps[i] = NULL; // paranoid
  }
  free(bitmaps);
  free(howmany);
  free(numbers);

  return (int)bogus;
}
