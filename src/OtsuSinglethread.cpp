#include "OtsuSinglethread.h"

#include <numeric>
#include <tuple>
#include <vector>

using std::vector;

OtsuPartition::OtsuPartition(const vector<unsigned int>& hist) :
    N(std::accumulate(hist.begin(), hist.end(), 0)),
    cur_part(M - 1), q(M, 0), mu(M, 0),
    hist_prefix(MAXP + 1, 0), w_hist_prefix(MAXP + 1, 0) { 

    for (size_t i = 1; i <= MAXP; ++i) {
        hist_prefix[i] = hist_prefix[i - 1] + hist[i - 1];
        w_hist_prefix[i] = w_hist_prefix[i - 1] + 1ll * hist[i - 1] * (i - 1);
    }

    get_partition(1, 0);
}

void OtsuPartition::get_partition(int tl, int cur_th) {
    if (cur_th == M - 1) {
        relax_ans();
        return; 
    }
    for (size_t i = tl; i < MAXP; ++i) {
        cur_part[cur_th] = i;
        get_partition(i + 1, cur_th + 1);
    }
}

void OtsuPartition::relax_ans() {
    std::tie(q[0], mu[0]) = get_sums_for_part(0, cur_part.front());
    for (size_t i = 1; i < cur_part.size(); ++i) {
        std::tie(q[i], mu[i]) = get_sums_for_part(cur_part[i - 1], cur_part[i]);
    }
    std::tie(q.back(), mu.back()) = get_sums_for_part(cur_part.back(), MAXP);

    double cur_disp = 0;
    for (int i = 0; i < M; ++i) {
        cur_disp += q[i] * mu[i] * mu[i];
    }
    if (cur_disp > part_disp) {
        part = cur_part;
        part_disp = cur_disp;
    }
}

// [tl, tr)
std::pair<unsigned long long, double> OtsuPartition::get_sums_for_part(int tl, int tr) {
    unsigned long long q = get_sum(hist_prefix, tl, tr);
    double mu = 1.0 * get_sum(w_hist_prefix, tl, tr) / q;
    return {q, mu};
}

// [tl, tr)
unsigned long long OtsuPartition::get_sum(const vector<unsigned long long>& pref_sums,
        int tl, int tr) {
    return pref_sums[tr] - pref_sums[tl];
}

