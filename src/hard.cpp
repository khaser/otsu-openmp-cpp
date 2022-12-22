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

#include "OtsuSinglethread.h"
#include "OtsuMultithread.h"
#include "PgmImage.h"

const std::vector<int> gray_scale = {0, 84, 170, 255};

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
    PgmImage picture(fin);

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

    for (uint8_t i : partition) std::cout << i - 1 << ' ';
    std::cout << '\n';

    return 0;
}
