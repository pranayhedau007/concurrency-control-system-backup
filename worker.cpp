#include "worker.h"
#include "transaction_manager.h"
#include <random>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>

// helper: extract balance from JSON-like value
int extract_balance(const std::string& value) {
    size_t pos = value.find("balance:");
    if (pos == std::string::npos) return 0;

    pos += 8;
    while (pos < value.size() && (value[pos] == ' ' || value[pos] == '"'))
        pos++;

    size_t end = value.find_first_of("}", pos);
    return std::stoi(value.substr(pos, end - pos));
}

// helper: update balance in value string
std::string update_balance(const std::string& value, int new_balance) {
    size_t pos = value.find("balance:");
    if (pos == std::string::npos) return value;

    size_t start = pos + 8;
    while (start < value.size() && value[start] == ' ')
        start++;

    size_t end = value.find_first_of("}", start);

    return value.substr(0, start) + std::to_string(new_balance) + value.substr(end);
}

Worker::Worker(int id,
               StorageEngine* storage,
               Metrics* metrics,
               LockManager* lock_manager,
               int txn_count,
               const std::vector<std::string>& keys,
               Protocol protocol)
{
    this->worker_id = id;
    this->storage = storage;
    this->metrics = metrics;
    this->lock_manager = lock_manager;
    this->txn_count = txn_count;
    this->keys = keys;
    this->protocol = protocol;
}

void Worker::run() {

    TransactionManager txn_manager(storage);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, keys.size() - 1);

    std::cout << "Worker " << worker_id << " started\n";

    for (int i = 0; i < txn_count; i++) {

        bool committed = false;

        while (!committed) {

            Transaction txn(i);

            // choose accounts
            std::string from_key = keys[dist(gen)];
            std::string to_key   = keys[dist(gen)];

            if (from_key == to_key)
                continue;

            // --- C2PL locks ---
            if (protocol == Protocol::C2PL) {
                lock_manager->lock_exclusive(from_key);
                lock_manager->lock_exclusive(to_key);
            }

            // --- READ ---
            std::string from_val = txn.read(from_key, storage);
            std::string to_val   = txn.read(to_key, storage);

            int from_balance = extract_balance(from_val);
            int to_balance   = extract_balance(to_val);

            // --- MODIFY ---
            from_balance -= 1;
            to_balance += 1;

            std::string new_from = update_balance(from_val, from_balance);
            std::string new_to   = update_balance(to_val, to_balance);

            // --- WRITE ---
            txn.write(from_key, new_from);
            txn.write(to_key, new_to);

            // --- COMMIT ---
            committed = txn_manager.commit(txn);

            // --- UNLOCK ---
            if (protocol == Protocol::C2PL) {
                lock_manager->unlock_exclusive(from_key);
                lock_manager->unlock_exclusive(to_key);
            }

            if (!committed) {
                metrics->aborted_txns++;
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        }

        metrics->committed_txns++;
    }

    std::cout << "Worker " << worker_id << " finished\n";
}