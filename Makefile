# minimalist makefile
######################
# To add a competitive technique, simply add your code in the src subdirectory (follow the README.md instructions) and
# add your executable file name to the EXECUTABLES variable below.
# along with a target to build it.
#######################
.SUFFIXES:
#
.SUFFIXES: .cpp .o .c .h

.PHONY: clean libroaring simplelibroaring
ifeq ($(DEBUG),1)
CFLAGS = -fPIC  -std=c99 -ggdb -mavx2 -march=native -Wall -Wextra -Wshadow -fsanitize=undefined  -fno-omit-frame-pointer -fsanitize=address
CXXFLAGS = -fPIC  -std=c++11 -ggdb -mavx2 -march=native -Wall -Wextra -Wshadow -fsanitize=undefined  -fno-omit-frame-pointer -fsanitize=address -Wno-deprecated-register
ROARFLAGS = -DCMAKE_BUILD_TYPE=Debug -DSANITIZE=ON
else
CFLAGS = -fPIC -std=c99 -O3 -mavx2  -march=native -Wall -Wextra -Wshadow 
CXXFLAGS = -fPIC -std=c++11 -O3 -mavx2  -march=native -Wall -Wextra -Wshadow -Wno-deprecated-register 
ROARFLAGS = -DCMAKE_BUILD_TYPE=Release
endif # debug

UNAME := $(shell uname)


ifeq ($(UNAME), Darwin)
  CROARINGLIBNAME=roaringlib/libroaring.dylib
  CSIMPLEROARINGLIBNAME=simpleroaringlib/libroaring.dylib
else
  CROARINGLIBNAME=roaringlib/libroaring.so
  CSIMPLEROARINGLIBNAME=simpleroaringlib/libroaring.so
endif

EXECUTABLES=wah32_benchmarks concise_benchmarks roaring_benchmarks simpleroaring_benchmarks bitmagic_benchmarks ewah32_benchmarks ewah64_benchmarks stl_vector_benchmarks stl_hashset_benchmarks bitset_benchmarks

all: $(EXECUTABLES)

test:
	./scripts/all.sh

roaring_benchmarks : libroaring src/roaring_benchmarks.c
	@$(CC) $(CFLAGS) -o roaring_benchmarks src/roaring_benchmarks.c -ICRoaring/include -ICRoaring/benchmarks -Lroaringlib -lroaring

simpleroaring_benchmarks : simplelibroaring src/roaring_benchmarks.c
	@$(CC) $(CFLAGS) -o simpleroaring_benchmarks src/roaring_benchmarks.c -ICSimpleRoaring/include -ICSimpleRoaring/benchmarks -Lsimpleroaringlib -lroaring


bitmagic_benchmarks: src/bitmagic_benchmarks.cpp
	$(CXX) $(CXXFLAGS) -o bitmagic_benchmarks src/bitmagic_benchmarks.cpp -IBitMagic/src  -ICRoaring/include -ICRoaring/benchmarks

ewah32_benchmarks: src/ewah32_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o ewah32_benchmarks ./src/ewah32_benchmarks.cpp -IEWAHBoolArray/headers  -ICRoaring/include -ICRoaring/benchmarks

wah32_benchmarks: src/wah32_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o wah32_benchmarks ./src/wah32_benchmarks.cpp -IConcise/include  -ICRoaring/include -ICRoaring/benchmarks

concise_benchmarks: src/concise_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o concise_benchmarks ./src/concise_benchmarks.cpp -IConcise/include  -ICRoaring/include -ICRoaring/benchmarks


ewah64_benchmarks: src/ewah64_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o ewah64_benchmarks ./src/ewah64_benchmarks.cpp -IEWAHBoolArray/headers  -ICRoaring/include -ICRoaring/benchmarks

stl_vector_benchmarks: src/stl_vector_benchmarks.cpp src/allocator.h
	$(CXX) $(CXXFLAGS)  -o stl_vector_benchmarks ./src/stl_vector_benchmarks.cpp   -ICRoaring/include -ICRoaring/benchmarks

stl_hashset_benchmarks: src/stl_hashset_benchmarks.cpp src/allocator.h
	$(CXX) $(CXXFLAGS)  -o stl_hashset_benchmarks ./src/stl_hashset_benchmarks.cpp   -ICRoaring/include -ICRoaring/benchmarks

bitset_benchmarks: src/bitset_benchmarks.c cbitset/include/bitset.h cbitset/src/bitset.c
	$(CXX) $(CXXFLAGS)  -o bitset_benchmarks ./src/bitset_benchmarks.c cbitset/src/bitset.c   -ICRoaring/include -ICRoaring/benchmarks -Icbitset/include

libroaring:
	@(mkdir -p roaringlib && cd roaringlib && cmake $(ROARFLAGS) ../CRoaring >/dev/null && make >/dev/null)

simplelibroaring:
	@(mkdir -p simpleroaringlib && cd simpleroaringlib && cmake $(ROARFLAGS) ../CSimpleRoaring >/dev/null && make >/dev/null)

clean:
	rm -r -f simpleroaringlib roaringlib  $(EXECUTABLES) CRoaring/CMakeCache.txt CRoaring/CMakeFiles  CSimpleRoaring/CMakeCache.txt CSimpleRoaring/CMakeFiles
