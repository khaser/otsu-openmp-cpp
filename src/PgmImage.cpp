#include "PgmImage.h"

#include <iostream>

std::ostream& operator << (std::ostream& os, const PgmImage& p) {
    os << p.magic_bits << '\n' << p.width << ' ' << p.height << "\n255\n";
    for (const auto& row : p.pixels) {
        for (const uint8_t& pixel : row) {
            os << pixel;
        }
    }
    return os;
}

PgmImage::PgmImage(std::istream& is): hist(UINT8_MAX + 1, 0) {
    std::string magic;
    is >> magic;
    assert(magic == magic_bits && "unexpected image file magic bits");

    is >> width;
    is >> height;
    pixels.resize(height, std::vector<uint8_t> (width));

    int max_color;
    is >> max_color;
    assert(max_color == 255 && "unsupported color-space");
    is.ignore(2, '\n');

    for (size_t x = 0; x < height; ++x) {
        for (size_t y = 0; y < width; ++y) {
            uint8_t c;
            is.read((char*)&c, 1);
            hist[c]++;
            pixels[x][y] = c;
        }
    }

    assert(!is.bad() && "an error occurred while reading the file");
}

void PgmImage::transform(std::function<uint8_t(uint8_t)> transformer) {
    for (auto& row : pixels) {
        for (uint8_t& pixel : row) {
            pixel = transformer(pixel);
        }
    }
}

