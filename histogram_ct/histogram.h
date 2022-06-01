#ifndef _HISTOGRAM_H
#define _HISTOGRAM_H

void histogram_ct(size_t *buckets, const size_t nbuckets,
                  const int * const data, const size_t ndata,
                  const size_t binsize);


void histogram(size_t *buckets, const size_t nbuckets,
               const int * const data, const size_t ndata,
               const size_t binsize);


#endif // #ifndef _HISTOGRAM_H
