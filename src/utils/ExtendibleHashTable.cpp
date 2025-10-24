#include "ExtendibleHashTable.hpp"

ExtendibleHashTable::ExtendibleHashTable(const std::string &_file_path, size_t _bucket_cap, Logger *_logger) : logger(_logger),
                                                                                                               secondary_file_path(file_path),
                                                                                                               bucket_capacity(bucket_cap),
                                                                                                               lobal_depth(1),
                                                                                                               total_records(0)
{
    if (logger == nullptr)
    {
        logger = Logger::getLogger();
    }
    buffer = Buffer::getBuffer();

    sec_storage.open(sec_mem_filepath + data_file_suffix, std::ios::in | std::ios::out | std::ios::binary);

    if (!sec_storage.is_open())
    {
        sec_storage.open(sec_mem_filepath + data_file_suffix, std::ios::in | std::ios::out | std::ios::binary);

        HashBucket first_bucket;
        first_bucket.local_depth = 1;
        first_bucket.block_offset = 0;
        first_bucket.entry_count = 0;
        first_bucket.is_full = false;

        bucket_directory[0] = first_bucket;
        hash_to_bucket[0] = 0;

        LOG_INFO(logger, "Extendible Hash Table Initialized");
    }
    else
    {
        LOG_DEBUG(logger, "Hash table already exists, loading metadata related");
        loadMetadata();
    }

    ExtendibleHashTable::~ExtendibleHashTable()
    {
        LOG_INFO(logger, "Hash table destructor called. Saving remaining data");
        saveMetadata();
        if (sec_storage.is_open())
        {
            sec_storage.close();
        }
    }

    size_t ExtendibleHashTable::hashFunction(const char *key)
    {
        std::hash<char *> hasher;
        return hasher(key);
    }

    size_t ExtendibleHashTable::getBucketIndex(size_t hash)
    {
        size_t mask = (1 << global_depth) - 1;
        return hash & mask;
    }

    int ExtendibleHashTable::insert(const char *key, const std::byte *record_data, size_t record_size)
    {
        std::lock_guard<std::mutex> lock(buffer->buffer_mutex);

        size_t hash = hashFunction(key);
        size_t bucket_idx = getBucketIndex(hash);

        // insert data and key on the bucket with hash function. If not found generate new
        if (bucker_directory.find(bucket_idx) == bucket_directory.end())
        {
            HashBucket new_bucket;
            new_bucket.local_depth = global_depth;
            new_bucket.block_offset = sec_storage.tellp();
            new_bucket.entry_count = 0;
            bucket_directory[bucket_idx] = new_bucket;
        }

        HashBucket &bucket = bucket_directory[bucket_idx];

        // check if overflows occurs
        if (bucket.entry_count >= bucket_capacity)
        {
            if (bucket.local_depth == global_depth)
            {
                doubleDirectory();
            }
            splitBucket(bucket_idx);
            return insert(key, record_data, record_size);
        }
        // all clear to insert in sec storage

        sec_storage.seekp(0, std::ios::end);
        size_t record_offset = sec_storage.tellp();

        sec_storage.write(reinterpret_cast<const char *>(record_data), record_size);
        sec_storage.flush();

        bucket.entries.pushback({hash, record_offset});
        bucket.entry_count++;
        total_records++;

        std::osstream oss;
        oss << "Total Records saved in the Hash Table: " << total_records << std::endl;
        LOG_DEBUG(logger, oss.str());

        saveBucketToDisk(bucket_idx);
        return true;
    }

    int ExtendibleHashTable::search(const std::string &key, std::byte *&record_data, size_t &record_size)
    {
        std::lock_guard<std::mutex> lock(buffer->buffer_mutex);

        size_t hash = hashFunction(key);
        size_t bucket_idx = getBucketIndex(hash);

        if (bucket_directory.find(bucket_idx) == bucket_directory.end())
        {
            return 0;
        }

        HashBucket &bucket = bucket_directory[bucket_idx];

        for (const auto & : bucket.entries)
        {
            if (entry.first == hash)
            {
            }
        }
    }
}