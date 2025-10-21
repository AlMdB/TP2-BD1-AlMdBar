#ifndef EXTENDIBLE_HASH_TABLE_H
#define EXTENDIBLE_HASH_TABLE_H

#include "Buffer.hpp"
#include "Record.hpp"
#include "Logger.hpp"
#include "Chronometer.hpp"
#include <fstream>
#include <vector>
#include <cstring>

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
    std::fsteam sec_storage;

    size_t global_depth;
    size_t bucket_capacity;
    size_t total_records;
    st
    

    int insertRecord();
    int searchRecord();

}


#endif // EXTENDIBLE_HASH_TABLE_H