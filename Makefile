# minimalist makefile
######################
# To add a competitive technique, simply add your code in the src subdirectory (follow the README.md instructions) and
# add your executable file name to the EXECUTABLES variable below.
# along with a target to build it.
#######################
.SUFFIXES:
#
.SUFFIXES: .cpp .o .c .h

.PHONY: clean
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
OSFLAGS= -Wl,--no-as-needed
endif

ifeq ($(DEBUG),1)
CFLAGS = -fuse-ld=gold -fPIC  -std=c99 -ggdb -mavx2 -mbmi2 -march=native -Wall -Wextra -Wshadow -fsanitize=undefined  -fno-omit-frame-pointer -fsanitize=address  $(OSFLAGS) -ldl
CXXFLAGS = -fuse-ld=gold -fPIC  -std=c++11 -ggdb -mavx2 -mbmi2 -march=native -Wall -Wextra -Wshadow -fsanitize=undefined  -fno-omit-frame-pointer -fsanitize=address -Wno-deprecated-register  $(OSFLAGS) -ldl
ROARFLAGS = -DCMAKE_BUILD_TYPE=Debug -DSANITIZE=ON
else
CFLAGS = -fPIC -std=c99 -O3 -mavx2 -mbmi2 -march=native -Wall -Wextra -Wshadow   $(OSFLAGS) -ldl
CXXFLAGS = -fPIC -std=c++11 -O3 -mavx2 -mbmi2  -march=native -Wall -Wextra -Wshadow -Wno-deprecated-register  $(OSFLAGS) -ldl
ROARFLAGS = -DCMAKE_BUILD_TYPE=Release
endif # debug

UNAME := $(shell uname)


EXECUTABLES=wah32_benchmarks concise_benchmarks roaring_benchmarks slow_roaring_benchmarks  bitmagic_benchmarks ewah32_benchmarks ewah64_benchmarks stl_vector_benchmarks stl_hashset_benchmarks stl_vector_benchmarks_memtracked stl_hashset_benchmarks_memtracked bitset_benchmarks malloced_roaring_benchmarks hot_roaring_benchmarks hot_slow_roaring_benchmarks

all: $(EXECUTABLES)

test:
	./scripts/all.sh


src/roaring.c :
	(cd src && exec ../CRoaring/amalgamation.sh && rm almagamation_demo.c && rm almagamation_demo.cpp)

roaring_benchmarks : src/roaring.c src/roaring_benchmarks.c
	$(CC) $(CFLAGS) -o roaring_benchmarks src/roaring_benchmarks.c


hot_roaring_benchmarks : src/roaring.c src/hot_roaring_benchmarks.c
	$(CC) $(CFLAGS)  -ggdb -o hot_roaring_benchmarks src/hot_roaring_benchmarks.c

malloced_roaring_benchmarks : src/roaring.c src/roaring_benchmarks.c
	$(CC) $(CFLAGS) -o malloced_roaring_benchmarks src/roaring_benchmarks.c -DRECORD_MALLOCS


slow_roaring_benchmarks : src/roaring.c src/roaring_benchmarks.c
	$(CC) $(CFLAGS) -DDISABLE_X64 -o slow_roaring_benchmarks src/roaring_benchmarks.c

hot_slow_roaring_benchmarks : src/roaring.c src/hot_roaring_benchmarks.c
	$(CC) $(CFLAGS)   -ggdb  -DDISABLE_X64 -o hot_slow_roaring_benchmarks src/hot_roaring_benchmarks.c


bitmagic_benchmarks: src/bitmagic_benchmarks.cpp
	$(CXX) $(CXXFLAGS) -o bitmagic_benchmarks src/bitmagic_benchmarks.cpp -IBitMagic/src

ewah32_benchmarks: src/ewah32_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o ewah32_benchmarks ./src/ewah32_benchmarks.cpp -IEWAHBoolArray/headers

wah32_benchmarks: src/wah32_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o wah32_benchmarks ./src/wah32_benchmarks.cpp -IConcise/include

concise_benchmarks: src/concise_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o concise_benchmarks ./src/concise_benchmarks.cpp -IConcise/include

ewah64_benchmarks: src/ewah64_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o ewah64_benchmarks ./src/ewah64_benchmarks.cpp -IEWAHBoolArray/headers

stl_vector_benchmarks: src/stl_vector_benchmarks.cpp src/memtrackingallocator.h
	$(CXX) $(CXXFLAGS)  -o stl_vector_benchmarks ./src/stl_vector_benchmarks.cpp

stl_hashset_benchmarks: src/stl_hashset_benchmarks.cpp src/memtrackingallocator.h
	$(CXX) $(CXXFLAGS)  -o stl_hashset_benchmarks ./src/stl_hashset_benchmarks.cpp


stl_vector_benchmarks_memtracked: src/stl_vector_benchmarks.cpp src/memtrackingallocator.h
	$(CXX) $(CXXFLAGS)  -o stl_vector_benchmarks_memtracked ./src/stl_vector_benchmarks.cpp -DMEMTRACKED

stl_hashset_benchmarks_memtracked: src/stl_hashset_benchmarks.cpp src/memtrackingallocator.h
	$(CXX) $(CXXFLAGS)  -o stl_hashset_benchmarks_memtracked ./src/stl_hashset_benchmarks.cpp -DMEMTRACKED

bitset_benchmarks: src/bitset_benchmarks.c cbitset/include/bitset.h cbitset/src/bitset.c
	$(CC) $(CFLAGS)  -o bitset_benchmarks ./src/bitset_benchmarks.c cbitset/src/bitset.c   -Icbitset/include

clean:
	rm -r -f   $(EXECUTABLES) src/roaring.c src/roaring.h src/roaring.hh
