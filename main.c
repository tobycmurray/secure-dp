#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#include "mastik/fr.h"
#include "mastik/util.h"
#include "mastik/low.h"

#include "german_credit.c"

#define BINSIZE 25

#define NUMBINS 800


#if (NUMBINS-1) < CREDIT_MAX_VAL/BINSIZE
#error "need more bins"
#endif

int bins[NUMBINS] = {0};

// a delay used in the query loop to allow the spy to interleave
const struct timespec qdelay =  {.tv_sec = 0, .tv_nsec = 500000};
// a delay used in the spy loop to have it run at a similar rate
// to the victim but ensure it can (hopefully) reliably measure
const struct timespec sdelay = {.tv_sec = 0, .tv_nsec = 400000};

// bins[i] is for values in range i*binsize .. (i+1)*binsize-1
// preconditions:
//  binsize > 0, data is all non-negative,
//  bins is a zero-filled array with length at leas (max / binsize)
//   where 'max' is the maximum value in data
void gen_histogram(int *bins, int binsize, int *data, size_t len){
  //printf("Histogram on %d values\n",len);
  for (size_t i=0; i<len; i++){
    int bin = data[i] / binsize;
    bins[bin]++;
    nanosleep(&qdelay,NULL);
  }
}


volatile int done = 0;

void* query_thread_func(void *arg){
  //printf("Query thread running...\n");
#ifdef PRESENT
  gen_histogram(bins, BINSIZE, credit_orig, CREDIT_ORIG_LEN);
#else
  gen_histogram(bins, BINSIZE, credit_upd, CREDIT_UPD_LEN);
#endif
  printf("Query thread done\n");
  done = 1;
  return NULL;
}

int main(const int argc, const char *argv[]){
  pthread_t query_thread;
  
  void *ptr = &(bins[CREDIT_MAX_VAL/BINSIZE]);
  fr_t fr = fr_prepare();
  fr_monitor(fr, ptr);

  uint16_t res[1];
  fr_probe(fr, res);

  int lines=0;
  printf("Creating query thread\n");
  pthread_create(&query_thread, NULL, &query_thread_func, NULL);
  printf("Main thraed continuing\n");
  while (!done) {
    nanosleep(&sdelay, NULL);
    printf(".");
    fr_probe(fr, res);
    if (res[0] < 100)
      printf("%4d: %s", ++lines, "max val touched\n");
    fflush(stdout);
    //delayloop(100);
  }

  return 0;

}
