#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <cstdint>

#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_set>
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



typedef std::unordered_set<uint32_t,std::hash<uint32_t>,std::equal_to<uint32_t>,MemoryCountingAllocator<uint32_t> >  hashset;


/**
 * Once you have collected all the integers, build the bitmaps.
 */
static std::vector<hashset > create_all_bitmaps(size_t *howmany,
        uint32_t **numbers, size_t count) {
    if (numbers == NULL) return std::vector<hashset >();
    std::vector<hashset> answer(count);
    for (size_t i = 0; i < count; i++) {
        hashset & bm = answer[i];
        uint32_t * mynumbers = numbers[i];
        for(size_t j = 0; j < howmany[i] ; ++j) {
            bm.insert(mynumbers[j]);
        }
    }
    return answer;
}


static void intersection(hashset& h1, hashset& h2, hashset& answer) {
  if(h1.size() > h2.size()) {
    intersection(h2,h1,answer);
    return;
  }
  answer.clear();
  for(hashset::iterator i = h1.begin(); i != h1.end(); i++) {
    if(h2.find(*i) != h2.end())
      answer.insert(*i);
  }
}

static void hashunion(hashset& h1, hashset& h2, hashset& answer) {
  answer.clear();
  for(hashset::iterator i = h2.begin(); i != h2.end(); i++) {
    answer.insert(*i);
  }
  for(hashset::iterator i = h1.begin(); i != h1.end(); i++) {
    answer.insert(*i);
  }
}

static void inplace_union(hashset& h1, hashset& h2) {
  for(hashset::iterator i = h2.begin(); i != h2.end(); i++) {
    h1.insert(*i);
  }
}

static void printusage(char *command) {
    printf(
        " Try %s directory \n where directory could be "
        "benchmarks/realdata/census1881\n",
        command);
    ;
    printf("the -v flag turns on verbose mode");

}



static hashset  fast_logicalor(size_t n, hashset **inputs) {
	  class StdHashSetPtr {

	  public:
	    StdHashSetPtr(hashset *p, bool o) : ptr(p), own(o) {}
	    hashset *ptr;
	    bool own; // whether to clean

	    bool operator<(const StdHashSetPtr &o) const {
	      return o.ptr->size() < ptr->size(); // backward on purpose
	    }
	  };

	  if (n == 0) {
		return hashset();
	  }
	  if (n == 1) {
	    return hashset(*inputs[0]);
	  }
	  std::priority_queue<StdHashSetPtr> pq;
	  for (size_t i = 0; i < n; i++) {
	    // could use emplace
	    pq.push(StdHashSetPtr(inputs[i], false));
	  }
	  while (pq.size() > 1) {

	    StdHashSetPtr x1 = pq.top();
	    pq.pop();

	    StdHashSetPtr x2 = pq.top();
	    pq.pop();
      if(x1.own) {
      }
      if (x1.own && x2.own) {
        if(x1.ptr->size() > x2.ptr->size()) {
          inplace_union(*(x1.ptr),*(x2.ptr));
	        pq.push(x1);
          delete x2.ptr;
        } else {
          inplace_union(*(x2.ptr),*(x1.ptr));
	        pq.push(x2);
          delete x1.ptr;
        }
      } else  if (x1.own) {
        inplace_union(*(x1.ptr),*(x2.ptr));
	      pq.push(x1);
	    } else if (x2.own) {
        inplace_union(*(x2.ptr),*(x2.ptr));
	      pq.push(x2);
	    } else {
	      hashset * buffer = new hashset();
        hashunion(*(x1.ptr),*(x2.ptr),*buffer);
	      pq.push(StdHashSetPtr(buffer, true));
      }
	  }
	  StdHashSetPtr x1 = pq.top();
	  pq.pop();
	  return *x1.ptr;
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
    std::vector<hashset > bitmaps = create_all_bitmaps(howmany, numbers, count);
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
        hashset v;
        intersection(bitmaps[i], bitmaps[i + 1], v);
        successive_and += v.size();
    }
    RDTSC_FINAL(cycles_final);
    data[1] = cycles_final - cycles_start;
    if(verbose) printf("Successive intersections on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    for (int i = 0; i < (int)count - 1; ++i) {
        hashset v (bitmaps[i]);
        inplace_union(v, bitmaps[i + 1]);
        successive_or += v.size();
    }
    RDTSC_FINAL(cycles_final);
    data[2] = cycles_final - cycles_start;
    if(verbose) printf("Successive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);

    RDTSC_START(cycles_start);
    if(count>1) {
        hashset v (bitmaps[0]);
        inplace_union(v, bitmaps[1]);
        for (int i = 2; i < (int)count ; ++i) {
            inplace_union(v, bitmaps[i]);
        }
        total_or = v.size();
    }
    RDTSC_FINAL(cycles_final);
    data[3] = cycles_final - cycles_start;
    if(verbose) printf("Total naive unions on %zu bitmaps took %" PRIu64 " cycles\n", count,
                           cycles_final - cycles_start);
    RDTSC_START(cycles_start);
    if(count>1) {
        hashset  ** allofthem = new  hashset* [count];
        for(int i = 0 ; i < (int) count; ++i) allofthem[i] = & bitmaps[i];
        hashset totalorbitmap = fast_logicalor(count, allofthem);
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
