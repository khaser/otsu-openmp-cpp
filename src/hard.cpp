#include <cstdint>
#include <omp.h>

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <cassert>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <functional>

#include "otsu_singlethread.h"
#include "otsu_multithread.h"

const std::vector<int> gray_scale = {0, 84, 170, 255};

struct PgmPicture {
    const std::string magic_bits = "P5";

    unsigned int height, width;
    std::vector<unsigned int> hist;
    std::vector<std::vector<uint8_t>> pixels;

    PgmPicture(std::istream& is) : hist(UINT8_MAX + 1, 0) {
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

    void transform(std::function<uint8_t(uint8_t)> transformer) {
        for (auto& row : pixels) {
            for (uint8_t& pixel : row) {
                pixel = transformer(pixel);
            }
        }
    }

};

std::ostream& operator << (std::ostream& os, const PgmPicture& p) {
    os << p.magic_bits << '\n' << p.width << ' ' << p.height << "\n255\n";
    for (const auto& row : p.pixels) {
        for (const uint8_t& pixel : row) {
            os << pixel;
        }
    }
    return os;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Synopsis: ./omp4 <treads> <input file> <output file>\n";
        return 1;
    }

    int threads = std::atoi(argv[1]);
    
    std::ifstream fin(argv[2], std::ios::binary);
    if (!fin) {
        std::cout << "Can not read from file with name " << argv[2] << '\n';
        return 1;
    }
    PgmPicture picture(fin);

    std::vector<uint8_t> partition;
    double start_time = omp_get_wtime();
    if (threads == -1) {
        partition = OtsuPartition(picture.hist).part;
    } else {
        if (threads != 0) omp_set_num_threads(threads);
        partition = Otsu(picture.hist);
    }
    std::cout <<  "Time (" << threads << " thread(s)): " << \
        omp_get_wtime() - start_time << " ms\n";

    std::function<uint8_t(uint8_t)> trans = [&partition](uint8_t x) {
        uint8_t gray_lvl = std::find_if(partition.begin(), partition.end(),
                [&x] (uint8_t threshold) {
                return x < threshold; 
            }
        ) - partition.begin();
        return gray_scale[gray_lvl];
    };
    picture.transform(trans);

    std::ofstream fout(argv[3], std::ios::binary); 
    if (!fout) {
        std::cout << "Can not write to file with name" << argv[3] << '\n';
        return 1;
    }
    fout << picture;

    return 0;
}
