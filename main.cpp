#include "storage_engine.h"
#include "worker.h"
#include "metrics.h"
#include "lock_manager.h"
#include "protocol.h"

#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>

void load_input(const std::string& filename,
                StorageEngine& storage,
                std::vector<std::string>& keys)
{
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {

        if (line.rfind("KEY:", 0) == 0) {

            size_t key_start = line.find("KEY:") + 4;
            size_t comma = line.find(",");
            std::string key = line.substr(key_start, comma - key_start);

            // trim whitespace
            key.erase(0, key.find_first_not_of(" "));
            key.erase(key.find_last_not_of(" ") + 1);

            size_t value_start = line.find("VALUE:") + 6;
            std::string value = line.substr(value_start);

            storage.insert(key, value);
            keys.push_back(key);
        }
    }
}

int main(int argc, char* argv[]) {

    std::cout << "Starting transaction processing system...\n";

    StorageEngine storage("mydb");
    Metrics metrics;
    LockManager lock_manager;

    std::vector<std::string> keys;

    // Load accounts from input file
    load_input("workload1/input1.txt", storage, keys);

    // Configuration
    int num_threads = 4;
    int txns_per_thread = 1000;

    // --- Protocol selection ---
    Protocol protocol = Protocol::OCC;

    if (argc > 1) {
        std::string arg = argv[1];

        if (arg == "2pl") {
            protocol = Protocol::C2PL;
        }
        else if (arg == "occ") {
            protocol = Protocol::OCC;
        }
        else {
            std::cerr << "Unknown protocol '" << arg << "', using OCC\n";
        }
    }

    std::vector<std::thread> threads;
    std::vector<Worker> workers;

    // Create worker objects
    for (int i = 0; i < num_threads; i++) {
        workers.emplace_back(
            i,
            &storage,
            &metrics,
            &lock_manager,
            txns_per_thread,
            keys,
            protocol
        );
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Launching workers...\n";

    // Launch threads
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(&Worker::run, &workers[i]);
    }

    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << "\nExecution finished\n";
    std::cout << "Committed Transactions: "
              << metrics.committed_txns.load() << std::endl;

    std::cout << "Aborted Transactions: "
              << metrics.aborted_txns.load() << std::endl;

    std::cout << "Throughput: "
              << metrics.committed_txns.load() / seconds
              << " txns/sec\n";

    return 0;
}