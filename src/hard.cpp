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

const std::vector<int> gray_scale = {0, 84, 170, 255};

struct PgmPicture {
    const std::string magic_bits = "P5";

    unsigned int height, width;
    std::vector<unsigned int> hist;
    std::vector<std::vector<uint8_t>> pixels;

    PgmPicture(std::istream& is) : hist(UINT8_MAX, 0) {
        std::string magic;
        is >> magic;
        assert(magic == magic_bits && "unexpected image file magic bits");

        is >> width;
        is >> height;
        pixels.resize(height, std::vector<uint8_t> (width));

        int max_color;
        is >> max_color;
        assert(max_color == 255 && "unsupported color-space");
        is.ignore(2);

        for (size_t x = 0; x < height; ++x) {
            for (size_t y = 0; y < width; ++y) {
                uint8_t c;
                is.read((char*)&c, 1);
                hist[c]++;
                pixels[x][y] = c;
            }
        }

        assert(!is.bad() && "an error occurred while reading the file");
        assert(is.eof() && "file contains more pixels, then specified in size");
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

struct OtsuPartition {
    const int M = 4; 
    std::vector<uint8_t> part;
private:
    const unsigned int N;
    double part_disp = 0;
    std::vector<uint8_t> cur_part;
    std::vector<unsigned int> q;
    std::vector<double> mu;
    std::vector<unsigned int> hist;
    std::vector<unsigned long long> pref_sums;

public:
    OtsuPartition(const std::vector<unsigned int>& hist) :
        N(std::accumulate(hist.begin(), hist.end(), 0)),
        cur_part(M - 1), q(M, 0), mu(M, 0), hist(hist),
        pref_sums(hist.size() + 1) { 

        for (size_t i = 1; i < hist.size(); ++i) {
            pref_sums[i] = pref_sums[i - 1] + hist[i - 1] * i;
        }
        get_partition(0, 0);
    }

    void get_partition(int tl, int cur_th) {
        if (cur_th == M - 1) {
            q[cur_th] = std::accumulate(hist.begin() + tl, hist.end(), 0);
            mu[cur_th] = 1.0 * get_sum(tl, hist.size()) / q[cur_th];
            double mu_total = 1.0 * get_sum(0, hist.size())  / N;
            double cur_disp = 0;
            for (size_t i = 0; i < q.size(); ++i) {
                int t = (mu[i] - mu_total);
                cur_disp += q[i] * t * t;
            }
            if (cur_disp > part_disp) {
                part = cur_part;
                part_disp = cur_disp;
            }
            return; 
        }

        q[cur_th] = 0;
        for (size_t i = tl; i < hist.size(); ++i) {
            q[cur_th] += hist[i];
            mu[cur_th] = 1.0 * get_sum(tl, i) / q[cur_th];
            cur_part[cur_th] = i;
            get_partition(i + 1, cur_th + 1);
        }
    }

private:
    // [tl, tr)
    unsigned long long get_sum(int tl, int tr) {
        return pref_sums[tr] - pref_sums[tl];
    }

};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Synopsis: ./omp4 <treads> <input file> <output file>\n";
        return 1;
    }

    std::ifstream fin(argv[2], std::ios::binary);
    if (!fin) {
        std::cout << "Can not read from file with name " << argv[2] << '\n';
        return 1;
    }
    PgmPicture picture(fin);

    std::cerr << "Image meta\n";
    std::cerr << picture.height << ' ' << picture.width << '\n';
    for (int i : picture.hist) {
        std::cerr << i << ' ';
    }
    std::cerr << '\n';

    std::vector<uint8_t> partition = OtsuPartition(picture.hist).part;

    std::cerr << "Otsu partition\n";
    for (uint8_t i : partition) std::cerr << (int) i << ' ';
    std::cerr << '\n';

    std::function<uint8_t(uint8_t)> trans = [&partition](uint8_t x) {
        uint8_t gray_lvl = std::find_if(partition.begin(), partition.end(),
                [&x] (uint8_t threshold) {
                return x <= threshold; 
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
