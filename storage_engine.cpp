#include "storage_engine.h"
#include <iostream>

StorageEngine::StorageEngine(const std::string& db_path) {
    options.create_if_missing = true;

    rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
    if (!status.ok()) {
        std::cerr << "Failed to open RocksDB: " << status.ToString() << std::endl;
        exit(1);
    }
}

StorageEngine::~StorageEngine() {
    delete db;
}

bool StorageEngine::insert(const std::string& key, const std::string& value) {
    rocksdb::Status status = db->Put(rocksdb::WriteOptions(), key, value);
    return status.ok();
}

bool StorageEngine::read(const std::string& key, std::string& value) {
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), key, &value);
    return status.ok();
}

bool StorageEngine::write(const std::string& key, const std::string& value) {
    rocksdb::Status status = db->Put(rocksdb::WriteOptions(), key, value);
    return status.ok();
}

bool StorageEngine::remove(const std::string& key) {
    rocksdb::Status status = db->Delete(rocksdb::WriteOptions(), key);
    return status.ok();
}