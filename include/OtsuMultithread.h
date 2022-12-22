#ifndef OTSU_MULTITHREAD_GUARD
#define OTSU_MULTITHREAD_GUARD

#include <cstdint>
#include <vector>

std::vector<uint8_t> Otsu(const std::vector<unsigned int>& hist);

#endif

