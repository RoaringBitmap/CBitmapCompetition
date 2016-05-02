# minimalist makefile
.SUFFIXES:
#
.SUFFIXES: .cpp .o .c .h

.PHONY: clean libroaring
ifeq ($(DEBUG),1)
CFLAGS = -fPIC  -std=c99 -ggdb -mavx2 -march=native -Wall -Wextra -Wshadow -fsanitize=undefined  -fno-omit-frame-pointer -fsanitize=address
CXXFLAGS = -fPIC  -std=c11 -ggdb -mavx2 -march=native -Wall -Wextra -Wshadow -fsanitize=undefined  -fno-omit-frame-pointer -fsanitize=address -Wno-deprecated-register
else
CFLAGS = -fPIC -std=c99 -O3 -mavx2  -march=native -Wall -Wextra -Wshadow
CXXFLAGS = -fPIC -std=c++11 -O3 -mavx2  -march=native -Wall -Wextra -Wshadow -Wno-deprecated-register
endif # debug

UNAME := $(shell uname)


ifeq ($(UNAME), Darwin)
  CROARINGLIBNAME=roaringlib/libroaring.dylib
else
  CROARINGLIBNAME=roaringlib/libroaring.so
endif

EXECUTABLES=roaring_benchmarks bitmagic_benchmarks ewah32_benchmarks ewah64_benchmarks stl_vector_benchmarks

all: $(EXECUTABLES)

test:
	./scripts/all.sh

roaring_benchmarks : libroaring src/roaring_benchmarks.c
	@$(CC) $(CFLAGS) -o roaring_benchmarks src/roaring_benchmarks.c -ICRoaring/include -ICRoaring/benchmarks -Lroaringlib -lroaring

bitmagic_benchmarks: src/bitmagic_benchmarks.cpp
	$(CXX) $(CXXFLAGS) -o bitmagic_benchmarks src/bitmagic_benchmarks.cpp -IBitMagic/src -ICRoaring/benchmarks

ewah32_benchmarks: src/ewah32_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o ewah32_benchmarks ./src/ewah32_benchmarks.cpp -IEWAHBoolArray/headers -ICRoaring/benchmarks

ewah64_benchmarks: src/ewah64_benchmarks.cpp
	$(CXX) $(CXXFLAGS)  -o ewah64_benchmarks ./src/ewah64_benchmarks.cpp -IEWAHBoolArray/headers -ICRoaring/benchmarks

stl_vector_benchmarks: src/stl_vector_benchmarks.cpp src/allocator.h
	$(CXX) $(CXXFLAGS)  -o stl_vector_benchmarks ./src/stl_vector_benchmarks.cpp  -ICRoaring/benchmarks

libroaring:
	@(mkdir -p roaringlib && cd roaringlib && cmake ../CRoaring >/dev/null && make >/dev/null)


clean:
	rm -r -f roaringlib  $(EXECUTABLES)
