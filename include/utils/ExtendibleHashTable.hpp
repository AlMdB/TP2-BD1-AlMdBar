#ifndef EXTENDIBLE_HASH_TABLE_H
#define EXTENDIBLE_HASH_TABLE_H

#include "Buffer.hpp"
#include "Record.hpp"
#include "Logger.hpp"
#include "Chronometer.hpp"
#include <fstream>
#include <vector>
#include <cstring>
#include <unordered_map>

struct HashBucket{
    size_t local_depth;
    size_t block_offset;
    size_t entry_count;
    std::vector<std::pair<size_t,size_t>> entries; // keyhash, offsetRecord
};

class ExtendibleHashTable{
private:
    Logger* logger;
    Buffer* ram_buffer;
    
    // file path and storage variables
    std::string sec_mem_filepath;
    std::fstream sec_storage;

    size_t global_depth;
    size_t bucket_capacity;
    size_t total_records;


    std::unordered_map<size_t,HashBucket> bucket_directory;
    std::unordered_map<size_t,size_t> hash_to_bucket;

    size_t hashFunction(const char* key);
    size_t getBucketIndex(size_t hash);
    void doubleDirectory();
    void splitBucket(size_t bucket_id);
    void loadBucketFromDisk(size_t bucket_id);
    void saveBucketToDisk(size_t bucket_id);
    void saveMetadata();
    void loadMetadata();

public:
    ExtensibleHashTable(const std::string& _file_path, size_t _bucket_cap = 4, Logger* _logger);
    ~ExtensibleHashTable();
    
    int insert(const char* key,const std::byte* record_data,size_t record_size);
    int search(const char* key, std::byte*& record_data, size_t& record_size);
    int remove(const char* key);

    size_t getRecordCount() const {return total_records; }
    size_t getGlobalDepth() const {return global_depth; }

    void printStatistics();
    void reorganize();

}


#endif // EXTENDIBLE_HASH_TABLE_H