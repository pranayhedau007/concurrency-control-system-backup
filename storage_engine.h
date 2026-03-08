#ifndef STORAGE_ENGINE_H
#define STORAGE_ENGINE_H

#include <string>
#include <rocksdb/db.h>

class StorageEngine {
private:
    rocksdb::DB* db;
    rocksdb::Options options;

public:
    StorageEngine(const std::string& db_path);
    ~StorageEngine();

    bool insert(const std::string& key, const std::string& value);
    bool read(const std::string& key, std::string& value);
    bool write(const std::string& key, const std::string& value);
    bool remove(const std::string& key);
};

#endif