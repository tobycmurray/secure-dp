#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t bucket;
  const int *data;
  size_t ndata;
  size_t binsize;
} dobucket_data_t;

int choose_ct(int c, int a, int b){
  // XXX: this would be faster with shifts perhaps
  return ((c * a) + ((1 - c) * b));
}


void *dobucket(void *data){
  const dobucket_data_t * const d = (dobucket_data_t *)data;
  size_t count = 0;
  
  for (size_t i=0; i<d->ndata; i++){
    int dat = d->data[i];
    size_t b = dat/d->binsize;
    count = choose_ct(b == d->bucket,count+1,count);
  }
  return (void *)count;
}

void histogram_ct(size_t *buckets, const size_t nbuckets,
                  const int * const data, const size_t ndata,
                  const size_t binsize){
  pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t)*nbuckets);
  dobucket_data_t *tdata = (dobucket_data_t *)malloc(sizeof(dobucket_data_t)*nbuckets);
  assert(threads);
  assert(tdata);
  for (size_t i=0; i<nbuckets; i++){
    tdata[i].bucket = i;
    tdata[i].data = data;
    tdata[i].ndata = ndata;
    tdata[i].binsize = binsize;
    if (pthread_create(&(threads[i]),NULL,&dobucket,(void *)&(tdata[i])) != 0){
      // horribly hard cleanup. ignore it for now
      assert(0 && "Didn't expect to have pthread_create fail, but it did!");
      return;
    }
  }
  // wait for them all to finish, populate the histogram with the counts
  for (size_t i=0; i<nbuckets; i++){
    size_t answer;
    pthread_join(threads[i],(void **)&answer);
    buckets[i] = answer;
  }
  free(tdata);
  free(threads);
}

void histogram(size_t *buckets, const size_t nbuckets,
               const int * const data, const size_t ndata,
               const size_t binsize){
  memset(buckets,0,sizeof(size_t)*nbuckets);
  for (size_t i=0; i<ndata; i++){
    int dat = data[i];
    size_t b = dat / binsize;
    buckets[b] = buckets[b] + 1;
  }
}

