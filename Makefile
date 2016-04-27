# minimalist makefile
.SUFFIXES:
#
.SUFFIXES: .cpp .o .c .h

.PHONY: clean libroaring
ifeq ($(DEBUG),1)
CFLAGS = -fPIC  -std=c99 -ggdb -mavx2 -march=native -Wall -Wextra -Wshadow -fsanitize=undefined  -fno-omit-frame-pointer -fsanitize=address
CXXFLAGS = -fPIC  -std=c11 -ggdb -mavx2 -march=native -Wall -Wextra -Wshadow -fsanitize=undefined  -fno-omit-frame-pointer -fsanitize=address
else
CFLAGS = -fPIC -std=c99 -O3 -mavx2  -march=native -Wall -Wextra -Wshadow
CXXFLAGS = -fPIC -std=c++11 -O3 -mavx2  -march=native -Wall -Wextra -Wshadow
endif # debug

UNAME := $(shell uname)


ifeq ($(UNAME), Darwin)
  CROARINGLIBNAME=roaringlib/libroaring.dylib
else
  CROARINGLIBNAME=roaringlib/libroaring.so
endif

EXECUTABLES=roaring_benchmarks bitmagic_benchmarks

all: $(EXECUTABLES)

test: 
	./scripts/all.sh

roaring_benchmarks : libroaring src/roaring_benchmarks.c
	@$(CC) $(CFLAGS) -o roaring_benchmarks src/roaring_benchmarks.c -ICRoaring/include -ICRoaring/benchmarks -Lroaringlib -lroaring

bitmagic_benchmarks: src/bitmagic_benchmarks.cpp
	$(CXX) $(CXXFLAGS) -O3 -o bitmagic_benchmarks src/bitmagic_benchmarks.cpp -IBitMagic/src -ICRoaring/benchmarks

libroaring:
	@(mkdir -p roaringlib && cd roaringlib && cmake ../CRoaring >/dev/null && make >/dev/null)


clean:
	rm -r -f roaringlib  $(EXECUTABLES)
