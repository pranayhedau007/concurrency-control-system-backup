#ifndef OCC_RECORD_H
#define OCC_RECORD_H

#include <unordered_set>
#include <string>

struct OCCRecord {
    int txn_id;
    std::unordered_set<std::string> write_set;
};

#endif