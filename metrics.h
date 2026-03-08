#ifndef METRICS_H
#define METRICS_H

#include <atomic>

class Metrics {
public:
    std::atomic<int> committed_txns;
    std::atomic<int> aborted_txns;

    Metrics() {
        committed_txns = 0;
        aborted_txns = 0;
    }
};

#endif