#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <queue>

#ifdef __cplusplus
extern "C" {
#endif
#include "benchmark.h"
#include "numbersfromtextfiles.h"
#ifdef __cplusplus
}
#endif

#define BMSSE2OPT // for good measure we enable SSE2
#define BMSSE42OPT // to enable SSE4 optimizations

#include "bm.h" /* bit magic */
#include "bmserial.h" /* bit magic, serialization routines */

typedef bm::bvector<> bvect;

/**
 * Once you have collected all the integers, build the bitmaps.
 */
static std::vector<bvect > create_all_bitmaps(size_t *howmany,
        uint32_t **numbers, size_t count, bool memorysavingmode) {
    if (numbers == NULL) return std::vector<bvect >();
    // we work hard to create the bitsets in memory-saving mode
    std::vector<bvect > answer(count);
    for (size_t i = 0; i < count; i++) {
        bvect & bm = answer[i];
        if(memorysavingmode) {
          bm.set_new_blocks_strat(bm::BM_GAP);
        }
        uint32_t * mynumbers = numbers[i];
        for(size_t j = 0; j < howmany[i] ; ++j) {
          bm.set(mynumbers[j]);
        }
        if(memorysavingmode) {
          bm.optimize();// this might be useless, redundant...
        }
    }
    return answer;
}

// This function has unresolved memory leaks. We don't care since we focus on performance.
static bvect  fast_logicalor(size_t n, bvect **inputs) {
	  class BMVectorWrapper {
	  public:
	    BMVectorWrapper(bvect * p, bool o) : ptr(p), own(o) {}
	    bvect * ptr;
            bool own;

	    bool operator<(const BMVectorWrapper & o) const {
	      return o.ptr->size() < ptr->size(); // backward on purpose
	    }
	  };

	  if (n == 0) {
		return bvect();
	  }
	  if (n == 1) {
	    return bvect(*inputs[0]);
	  }
	  std::priority_queue<BMVectorWrapper> pq;
	  for (size_t i = 0; i < n; i++) {
	    // could use emplace
	    pq.push(BMVectorWrapper(inputs[i], false));
	  }
	  while (pq.size() > 1) {

	    BMVectorWrapper x1 = pq.top();
	    pq.pop();

	    BMVectorWrapper x2 = pq.top();
	    pq.pop();
      if(x1.own) {
        x1.ptr->bit_or(*x2.ptr);
       if(x2.own) delete x2.ptr;
        pq.push(x1);
      } else if (x2.own) {
        x2.ptr->bit_or(*x1.ptr);
        pq.push(x2);
      } else {
        bvect ans = *x1.ptr | *x2.ptr;
        bvect * buffer = new bvect();
        buffer->swap(ans);
	      pq.push(BMVectorWrapper(buffer, true));
      }
	  }
	  BMVectorWrapper x1 = pq.top();
	  pq.pop();

	  return *x1.ptr;
	}



static void printusage(char *command) {
    printf(
        " Try %s directory \n where directory could be "
        "benchmarks/realdata/census1881\n",
        command);
    ;
    printf("the -v flag turns on verbose mode");
    printf("the -r flag turns on memory-saving mode");


}

int main(int argc, char **argv) {
    int c;
    const char *extension = ".txt";
    bool verbose = false;
    bool memorysavingmode = false;
    uint64_t data[6];
    while ((c = getopt(argc, argv, "rve:h")) != -1) switch (c) {
        case 'e':
            extension = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        case 'r':
            memorysavingmode = true;
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
    if(verbose) printf("memorysavingmode=%d\n",memorysavingmode);
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
    std::vector<bvect > bitmaps = create_all_bitmaps(howmany, numbers, count, memorysavingmode);
    RDTSC_FINAL(cycles_final);
    if (bitmaps.empty()) return -1;
    if(verbose) printf("Loaded %d bitmaps from directory %s \n", (int)count, dirname);
    uint64_t totalsize = 0;

    for (int i = 0; i < (int) count; ++i) {
        bvect & bv = bitmaps[i];
        bvect::statistics st;
        bv.calc_stat(&st);
        totalsize += st.memory_used;
    }
    data[0] = totalsize;

    if(verbose) printf("Total size in bytes =  %" PRIu64 " \n", totalsize);

    uint64_t successive_and = 0;
    uint64_t successive_or = 0;
    uint64_t total_or = 0;

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        bvect tempand = bitmaps[i] & bitmaps[i + 1];
        successive_and += tempand.count();
    }
    RDTSC_FINAL(cycles_final);
    data[1] = cycles_final - cycles_start;
    if(verbose) printf("Successive intersections on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        bvect tempor = bitmaps[i] | bitmaps[i + 1];
        successive_or += tempor.count();
    }
    RDTSC_FINAL(cycles_final);
    data[2] = cycles_final - cycles_start;
    if(verbose) printf("Successive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    if(count>1) {
        bvect totalorbitmap = bitmaps[0] | bitmaps[1];
        for (int i = 2; i < (int)count ; ++i) {
            totalorbitmap |= bitmaps[i];
        }
        total_or = totalorbitmap.count();
    }
    RDTSC_FINAL(cycles_final);
    data[3] = cycles_final - cycles_start;
    if(verbose) printf("Total naive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);
    RDTSC_START(cycles_start);
    if(count>1) {
        bvect  ** allofthem = new bvect* [count];
        for(int i = 0 ; i < (int) count; ++i) allofthem[i] = & bitmaps[i];
        bvect totalorbitmap = fast_logicalor(count, allofthem);
        total_or = totalorbitmap.count();
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
      quartcount += bitmaps[i].get_bit(maxvalue/4);
      quartcount += bitmaps[i].get_bit(maxvalue/2);
      quartcount += bitmaps[i].get_bit(3*maxvalue/4);
    }
    ENDBEST(data[5])

    if(verbose) printf("Quartile queries on %zu bitmaps took %" PRIu64 " cycles\n", count,
           data[5]);

    if(verbose) printf("Collected stats  %" PRIu64 "  %" PRIu64 "  %" PRIu64 " %" PRIu64 "\n",successive_and,successive_or,total_or,quartcount);

    printf(" %20.2f %20.2f %20.2f %20.2f %20.2f %20.2f \n",
      data[0]*8.0/totalcard,
      data[1]*1.0/successivecard,
      data[2]*1.0/successivecard,
      data[3]*1.0/totalcard,
      data[4]*1.0/totalcard,
      data[5]*1.0/(3*count)
    );


    for (int i = 0; i < (int)count; ++i) {
        free(numbers[i]);
        numbers[i] = NULL;  // paranoid
    }
    free(howmany);
    free(numbers);

    return 0;
}
