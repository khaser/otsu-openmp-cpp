#ifndef OTSU_SINGLETHREAD_GUARD
#define OTSU_SINGLETHREAD_GUARD

#include <vector>
#include <inttypes.h>

using std::size_t;
using std::vector;

std::vector<uint8_t> OtsuSinglethread(const std::vector<unsigned int>& hist);

#endif
