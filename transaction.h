#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <unordered_map>
#include <unordered_set>
#include <string>

class Transaction {
public:
    int txn_id;

    std::unordered_set<std::string> read_set;
    std::unordered_set<std::string> write_set;

    std::unordered_map<std::string, std::string> write_buffer;

    Transaction(int id);

    std::string read(const std::string& key, class StorageEngine* storage);

    void write(const std::string& key, const std::string& value);
};

#endif