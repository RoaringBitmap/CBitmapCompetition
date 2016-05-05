#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <cstdint>

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

#include "allocator.h"

void initializeMemUsageCounter()  {
    memory_usage = 0;
}

uint64_t getMemUsageInBytes()  {
    return memory_usage;
}



typedef std::vector<uint32_t,MemoryCountingAllocator<uint32_t> >  vector;


static vector  fast_logicalor(size_t n, const vector **inputs) {
	  class StdVectorPtr {

	  public:
	    StdVectorPtr(const vector *p, bool o) : ptr(p), own(o) {}
	    const vector *ptr;
	    bool own; // whether to clean

	    bool operator<(const StdVectorPtr &o) const {
	      return o.ptr->size() < ptr->size(); // backward on purpose
	    }
	  };

	  if (n == 0) {
		return vector();
	  }
	  if (n == 1) {
	    return vector(*inputs[0]);
	  }
	  std::priority_queue<StdVectorPtr> pq;
	  for (size_t i = 0; i < n; i++) {
	    // could use emplace
	    pq.push(StdVectorPtr(inputs[i], false));
	  }
	  while (pq.size() > 2) {

	    StdVectorPtr x1 = pq.top();
	    pq.pop();

	    StdVectorPtr x2 = pq.top();
	    pq.pop();
	    vector * buffer = new vector();
      std::set_union(x1.ptr->begin(), x1.ptr->end(),x2.ptr->begin(), x2.ptr->end(),std::back_inserter(*buffer));
	    if (x1.own) {
	      delete x1.ptr;
	    }
	    if (x2.own) {
	      delete x2.ptr;
	    }
	    pq.push(StdVectorPtr(buffer, true));
	  }
	  StdVectorPtr x1 = pq.top();
	  pq.pop();

	  StdVectorPtr x2 = pq.top();
	  pq.pop();

	  vector  container;
    std::set_union(x1.ptr->begin(), x1.ptr->end(),x2.ptr->begin(), x2.ptr->end(),std::back_inserter(container));

	  if (x1.own) {
	    delete x1.ptr;
	  }
	  if (x2.own) {
	    delete x2.ptr;
	  }
	  return container;
	}



/**
 * Once you have collected all the integers, build the bitmaps.
 */
static std::vector<vector > create_all_bitmaps(size_t *howmany,
        uint32_t **numbers, size_t count) {
    if (numbers == NULL) return std::vector<vector >();
    std::vector<vector > answer(count);

    for (size_t i = 0; i < count; i++) {
        vector & bm = answer[i];
        uint32_t * mynumbers = numbers[i];
        for(size_t j = 0; j < howmany[i] ; ++j) {
            bm.push_back(mynumbers[j]);
        }
        bm.shrink_to_fit();
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
    initializeMemUsageCounter();
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
    std::vector<vector> bitmaps = create_all_bitmaps(howmany, numbers, count);
    RDTSC_FINAL(cycles_final);
    if (bitmaps.empty()) return -1;
    if(verbose) printf("Loaded %d bitmaps from directory %s \n", (int)count, dirname);
    uint64_t totalsize = getMemUsageInBytes();
    data[0] = totalsize;

    if(verbose) printf("Total size in bytes =  %" PRIu64 " \n", totalsize);

    uint64_t successive_and = 0;
    uint64_t successive_or = 0;
    uint64_t total_or = 0;

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        vector v;
        std::set_intersection(bitmaps[i].begin(), bitmaps[i].end(),bitmaps[i+1].begin(), bitmaps[i+1].end(),std::back_inserter(v));
        successive_and += v.size();
    }
    RDTSC_FINAL(cycles_final);
    data[1] = cycles_final - cycles_start;
    if(verbose) printf("Successive intersections on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        vector v;
        std::set_union(bitmaps[i].begin(), bitmaps[i].end(),bitmaps[i+1].begin(), bitmaps[i+1].end(),std::back_inserter(v));
        successive_or += v.size();
    }
    RDTSC_FINAL(cycles_final);
    data[2] = cycles_final - cycles_start;
    if(verbose) printf("Successive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    if(count>1) {
        vector v;
        std::set_union(bitmaps[0].begin(), bitmaps[0].end(),bitmaps[1].begin(), bitmaps[1].end(),std::back_inserter(v));
        for (int i = 2; i < (int)count ; ++i) {
            vector newv;
            std::set_union(v.begin(), v.end(),bitmaps[i].begin(), bitmaps[i].end(),std::back_inserter(newv));
            v.swap(newv);
        }
        total_or = v.size();
    }
    RDTSC_FINAL(cycles_final);
    data[3] = cycles_final - cycles_start;
    if(verbose) printf("Total naive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);
    RDTSC_START(cycles_start);
    if(count>1) {
        const vector  ** allofthem = new const vector* [count];
        for(int i = 0 ; i < (int) count; ++i) allofthem[i] = & bitmaps[i];
        vector totalorbitmap = fast_logicalor(count, allofthem);
        total_or = totalorbitmap.size();
        delete[] allofthem;
    }
    RDTSC_FINAL(cycles_final);
    data[4] = cycles_final - cycles_start;
    if(verbose) printf("Total heap unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);
    if(verbose) printf("Collected stats  %" PRIu64 "  %" PRIu64 "  %" PRIu64 "\n",successive_and,successive_or,total_or);
    printf(" %20.2f %20.2f %20.2f %20.2f %20.2f \n",
      data[0]*8.0/totalcard,
      data[1]*1.0/successivecard,
      data[2]*1.0/successivecard,
      data[3]*1.0/totalcard,
      data[4]*1.0/totalcard);

    for (int i = 0; i < (int)count; ++i) {
        free(numbers[i]);
        numbers[i] = NULL;  // paranoid
    }
    free(howmany);
    free(numbers);

    return 0;
}
