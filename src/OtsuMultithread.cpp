#include "OtsuMultithread.h"

#include <vector>
#include <tuple>
#include <algorithm>
#include <numeric>

using std::vector;

namespace {

const int M = 4; 
const size_t MAXP = UINT8_MAX + 1;
vector<uint8_t> part;

unsigned int N = 0;
double part_disp = 0;
vector<uint8_t> cur_part;
vector<unsigned long long> hist_prefix, w_hist_prefix;

#pragma omp threadprivate(cur_part)

// [tl, tr)
unsigned long long get_sum(const vector<unsigned long long>& pref_sums,
        int tl, int tr) {
    return pref_sums[tr] - pref_sums[tl];
}

// [tl, tr)
std::pair<unsigned long long, double> get_sums_for_part(int tl, int tr) {
    unsigned long long q = get_sum(hist_prefix, tl, tr);
    double mu = 1.0 * get_sum(w_hist_prefix, tl, tr) / q;
    return {q, mu};
}

void relax_ans() {
    double cur_disp = 0;
    auto [q, mu] = get_sums_for_part(0, cur_part.front());
    cur_disp += q * mu * mu;
    for (size_t i = 1; i < cur_part.size(); ++i) {
        std::tie(q, mu) = get_sums_for_part(cur_part[i - 1], cur_part[i]);
        cur_disp += q * mu * mu;
    }
    std::tie(q, mu) = get_sums_for_part(cur_part.back(), MAXP);
    cur_disp += q * mu * mu;

    if (cur_disp > part_disp) {
#pragma omp critical
        {
            part = cur_part;
            part_disp = cur_disp;
        }
    }
}

void get_partition(int tl, int cur_th) {
    if (cur_th == M - 1) {
        relax_ans();
    } else if (cur_th == 0) {
#pragma omp for schedule(dynamic, 1)
        for (size_t i = tl; i < MAXP; ++i) {
            cur_part[cur_th] = i;
            get_partition(i + 1, cur_th + 1);
        }
    } else {
        for (size_t i = tl; i < MAXP; ++i) {
            cur_part[cur_th] = i;
            get_partition(i + 1, cur_th + 1);
        }
    }
}

} //end anonymous namespace


vector<uint8_t> Otsu(const vector<unsigned int>& hist) {
    N = std::accumulate(hist.begin(), hist.end(), 0);
    hist_prefix.resize(MAXP + 1, 0);
    w_hist_prefix.resize(MAXP + 1, 0);

    for (size_t i = 1; i <= MAXP; ++i) {
        hist_prefix[i] = hist_prefix[i - 1] + hist[i - 1];
        w_hist_prefix[i] = w_hist_prefix[i - 1] + 1ll * hist[i - 1] * (i - 1);
    }

#pragma omp parallel 
    {
        cur_part.resize(M - 1);
        get_partition(1, 0);
    }
    return part;
}
