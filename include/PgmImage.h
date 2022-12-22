#ifndef PGM_IMAGE_GUARD
#define PGM_IMAGE_GUARD

#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <functional>

struct PgmImage {
    const std::string magic_bits = "P5";

    unsigned int height, width;
    std::vector<unsigned int> hist;
    std::vector<std::vector<uint8_t>> pixels;

    PgmImage(std::istream& is);

    void transform(std::function<uint8_t(uint8_t)> transformer); 

};

std::ostream& operator << (std::ostream& os, const PgmImage& p);


#endif
