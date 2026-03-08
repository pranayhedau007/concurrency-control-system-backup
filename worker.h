#ifndef WORKER_H
#define WORKER_H

#include "lock_manager.h"
#include "storage_engine.h"
#include "protocol.h"
#include "transaction.h"
#include "metrics.h"
#include <vector>


class Worker {
private:
    StorageEngine* storage;
    Metrics* metrics;
    LockManager* lock_manager;

    int worker_id;
    int txn_count;
    std::vector<std::string> keys;
    Protocol protocol;

public:
    Worker(int id,
           StorageEngine* storage,
           Metrics* metrics,
           LockManager* lock_manager,
           int txn_count,
           const std::vector<std::string>& keys,
           Protocol protocol);

    void run();
};

#endif