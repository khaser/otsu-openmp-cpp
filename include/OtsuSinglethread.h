#ifndef OTSU_SINGLETHREAD_GUARD
#define OTSU_SINGLETHREAD_GUARD

#include <vector>
#include <inttypes.h>

using std::size_t;
using std::vector;

struct OtsuPartition {
    const int M = 4; 
    const size_t MAXP = UINT8_MAX + 1;
    vector<uint8_t> part;
private:
    const unsigned int N;
    double part_disp = 0;
    vector<uint8_t> cur_part;
    vector<unsigned long long> q;
    vector<double> mu;
    vector<unsigned long long> hist_prefix, w_hist_prefix;

public:
    OtsuPartition(const vector<unsigned int>& hist);

    void get_partition(int tl, int cur_th); 

    void relax_ans(); 

    // [tl, tr)
    std::pair<unsigned long long, double> get_sums_for_part(int tl, int tr); 

private:
    // [tl, tr)
    unsigned long long get_sum(const vector<unsigned long long>& pref_sums,
            int tl, int tr);

};

#endif
