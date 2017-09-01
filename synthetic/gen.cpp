#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 10000
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <iostream>

#include "anh_moffat_clustered.h"
void printusage(const char *command) {
  printf(" %s -N number -M maxval \n", command);
}
int main(int argc, char ** argv) {

  int c;
  uint32_t N = 0;
  uint32_t M = 0;

  while ((c = getopt(argc, argv, "hN:M:")) != -1)
    switch (c) {
    case 'h':
      printusage(argv[0]);
      return EXIT_SUCCESS;
    case 'N':
      N = atoi(optarg);
      break;
    case 'M':
      M = atoi(optarg);
      break;
    default:
      abort();
    }
  if(N > M) {
      printf("N = %u M = %u \n",N,M);
      printf(" N must be smaller than M! \n");
      return EXIT_FAILURE;
  }
  if(N == 0) {
      printf("N = %u M = %u \n",N,M);
      printf(" N must be greater than zero! \n");
      return EXIT_FAILURE;
  }
  ClusteredDataGenerator cdg;

  vector<uint32_t> v = cdg.generate(N, M);
  for(size_t i = 0; i < v.size(); i++) {
    std::cout << v[i];
    if(i + 1 < v.size()) std::cout<<",";
    std::cout.flush();
  }
  return EXIT_SUCCESS;
}
