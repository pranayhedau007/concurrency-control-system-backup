#include "transaction_manager.h"

TransactionManager::TransactionManager(StorageEngine* storage) {
    this->storage = storage;
}

bool TransactionManager::commit(Transaction& txn) {

    std::lock_guard<std::mutex> lock(validation_mutex);

    // VALIDATION (check only recent commits)
    int start = committed_txns.size() > 50 ? committed_txns.size() - 50 : 0;

    for (size_t i = start; i < committed_txns.size(); i++) {

        auto& committed = committed_txns[i];

        for (auto& key : txn.read_set) {

            if (committed.write_set.count(key)) {
                return false;
            }
        }
    }

    // WRITE PHASE
    for (auto& pair : txn.write_buffer) {
        storage->write(pair.first, pair.second);
    }

    // RECORD COMMIT
    OCCRecord record;
    record.txn_id = txn.txn_id;
    record.write_set = txn.write_set;

    committed_txns.push_back(record);

    return true;
}