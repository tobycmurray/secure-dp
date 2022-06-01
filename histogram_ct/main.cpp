extern "C" {
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "histogram.h"
#include "../attack/german_credit.c"
}

#include <limits>


#define MAX_BUCKETS 16

// C++
#include <chrono>

typedef struct result {
  double min;
  double max;
  double avg;
} result_t;

result_t results[MAX_BUCKETS];
result_t results_ct[MAX_BUCKETS];
size_t buckets[MAX_BUCKETS];

#define NUM_TRIALS 10

#define DATA_SET_SIZE (128*1024*1024)

#define MAX_DATA_VAL_SIZE (1024*1024)

int main(const int argc, const char *argv[]){

  int * data = (int *)malloc(sizeof(int)*DATA_SET_SIZE);
  assert (data && "couldn't allocate the data array");
  
  for (size_t i=0; i<DATA_SET_SIZE; i++){
    data[i] = arc4random_uniform(MAX_DATA_VAL_SIZE);
  }

  
  for (size_t nbuckets=1; nbuckets<MAX_BUCKETS; nbuckets++){
    results[nbuckets].min = std::numeric_limits<double>::max();
    results[nbuckets].max = (double)0.0;
    results[nbuckets].avg = (double)0.0;
    
    const size_t binsize = (MAX_DATA_VAL_SIZE / nbuckets) + 1;
    assert(MAX_DATA_VAL_SIZE / binsize < nbuckets);

    for (int trial=0; trial<NUM_TRIALS; trial++){
      auto t_start = std::chrono::high_resolution_clock::now();
      histogram(buckets,nbuckets,data,DATA_SET_SIZE,binsize);
      auto t_end = std::chrono::high_resolution_clock::now();
      double res = std::chrono::duration<double, std::milli>(t_end-t_start).count();
      if (res < results[nbuckets].min){
        results[nbuckets].min = res;
      }
      if (res > results[nbuckets].max){
        results[nbuckets].max = res;
      }
      results[nbuckets].avg += res;
    }
    results[nbuckets].avg = results[nbuckets].avg / (double)NUM_TRIALS;

    printf("%lu,%lf,%lf,%lf\n",nbuckets,results[nbuckets].min,results[nbuckets].max,results[nbuckets].avg);
  }

  
  for (size_t nbuckets=1; nbuckets<MAX_BUCKETS; nbuckets++){
    results_ct[nbuckets].min = std::numeric_limits<double>::max();
    results_ct[nbuckets].max = (double)0.0;
    results_ct[nbuckets].avg = (double)0.0;
    
    const size_t binsize = (MAX_DATA_VAL_SIZE / nbuckets) + 1;
    assert(MAX_DATA_VAL_SIZE / binsize < nbuckets);

    for (int trial=0; trial<NUM_TRIALS; trial++){
      auto t_start = std::chrono::high_resolution_clock::now();
      histogram_ct(buckets,nbuckets,data,DATA_SET_SIZE,binsize);      
      auto t_end = std::chrono::high_resolution_clock::now();
      double res = std::chrono::duration<double, std::milli>(t_end-t_start).count();
      if (res < results_ct[nbuckets].min){
        results_ct[nbuckets].min = res;
      }
      if (res > results_ct[nbuckets].max){
        results_ct[nbuckets].max = res;
      }
      results_ct[nbuckets].avg += res;
    }
    results_ct[nbuckets].avg = results_ct[nbuckets].avg / (double)NUM_TRIALS;
    printf("%lu,%lf,%lf,%lf\n",nbuckets,results_ct[nbuckets].min,results_ct[nbuckets].max,results_ct[nbuckets].avg);
  }

  
    
  return 0;
}
