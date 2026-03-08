#include "transaction.h"
#include "storage_engine.h"

Transaction::Transaction(int id) {
    txn_id = id;
}

std::string Transaction::read(const std::string& key, StorageEngine* storage) {

    if (write_buffer.find(key) != write_buffer.end()) {
        return write_buffer[key];
    }

    std::string value;
    storage->read(key, value);

    read_set.insert(key);

    return value;
}

void Transaction::write(const std::string& key, const std::string& value) {
    write_buffer[key] = value;
    write_set.insert(key);
}