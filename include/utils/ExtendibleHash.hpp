#ifndef EXTENDIBLE_HASH_V2_HPP
#define EXTENDIBLE_HASH_V2_HPP

#include <vector>
#include <memory>
#include <sstream>
#include <map>
#include "Logger.hpp"
#include "Record.hpp"

/**
 * HashBucket: Now uses actual Record objects
 * Respects block boundaries and accurately calculates occupancy
 */
struct HashBucket
{
    int bucket_id;
    int local_depth;
    int block_id;
    int max_block_size;

    std::vector<Record> records;
    int used_space;

    HashBucket(int id, int depth = 0, int blk_id = 0, int blk_size = 4096)
        : bucket_id(id), local_depth(depth), block_id(blk_id),
          max_block_size(blk_size), used_space(0) {}

    /**
     * Try to insert record into bucket
     * Returns true only if there's actual space in block
     */
    bool insertRecord(const Record &rec)
    {
        // Check if record fits in remaining block space
        if (used_space + rec.getTotalSize() > max_block_size)
        {
            return false; // Block full
        }
        records.push_back(rec);
        used_space += rec.getTotalSize();
        return true;
    }

    bool isFull() const
    {
        // Conservative: leave 5% margin for metadata
        return used_space > (max_block_size * 0.95);
    }

    double getOccupancy() const
    {
        return static_cast<double>(used_space) / max_block_size;
    }

    const Record *findRecord(int id) const
    {
        for (const auto &rec : records)
        {
            if (rec.getId() == id)
            {
                return &rec;
            }
        }
        return nullptr;
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << "Bucket[ID=" << bucket_id
            << " LocalDepth=" << local_depth
            << " Used=" << used_space << "/" << max_block_size
            << " Occupancy=" << (getOccupancy() * 100) << "%"
            << " Records=" << records.size() << "]";
        return oss.str();
    }
};

/**
 * ExtendibleHash: Updated to work with proper Record structure
 */
class ExtendibleHash
{
private:
    Logger &logger;
    std::vector<std::shared_ptr<HashBucket>> directory;
    int global_depth;
    int block_size;
    double max_load_factor;

    // Statistics
    int blocks_read;
    int blocks_written;
    int total_buckets;
    int splits_performed;

    int getHashValue(int key, int depth) const;
    void splitBucket(int bucket_index);
    void doubleDirectory();
    void logBucketState(const std::string &op, const HashBucket &bucket);

public:
    ExtendibleHash(Logger &log, int initial_buckets = 16,
                   int blk_size = 4096, double load_factor = 0.7);

    bool insert(const Record &rec);
    const Record *search(int id);
    void printStructure() const;
    void printStatistics() const;

    // Getters
    int getGlobalDepth() const { return global_depth; }
    int getDirectorySize() const { return directory.size(); }
    int getTotalBuckets() const { return total_buckets; }
    int getBlocksRead() const { return blocks_read; }
    int getBlocksWritten() const { return blocks_written; }
    int getSplitsPerformed() const { return splits_performed; }
};

#endif // EXTENDIBLE_HASH_V2_HPP

// ============== IMPLEMENTATION ==============

ExtendibleHash::ExtendibleHash(Logger &log, int initial_buckets,
                               int blk_size, double load_factor)
    : logger(log), global_depth(0), block_size(blk_size),
      max_load_factor(load_factor), blocks_read(0), blocks_written(0),
      total_buckets(0), splits_performed(0)
{

    // Calculate initial depth
    int temp = initial_buckets;
    while ((1 << global_depth) < initial_buckets)
    {
        global_depth++;
    }

    int num_buckets = 1 << global_depth;

    // Create buckets
    for (int i = 0; i < num_buckets; ++i)
    {
        auto bucket = std::make_shared<HashBucket>(i, global_depth, i, block_size);
        directory.push_back(bucket);
    }

    total_buckets = num_buckets;

    std::ostringstream oss;
    oss << "ExtendibleHash initialized - Global Depth: " << global_depth
        << " Buckets: " << num_buckets << " Block Size: " << block_size
        << " bytes Load Factor: " << load_factor;
    LOG_INFO(&logger, oss.str());
}

int ExtendibleHash::getHashValue(int key, int depth) const
{
    if (depth == 0)
        return 0;
    int mask = (1 << depth) - 1;
    return key & mask;
}

bool ExtendibleHash::insert(const Record &rec)
{
    int hash_val = getHashValue(rec.getId(), global_depth);

    if (hash_val >= static_cast<int>(directory.size()))
    {
        LOG_ERROR(&logger, "Hash value out of bounds");
        return false;
    }

    auto bucket = directory[hash_val];
    blocks_read++;

    // Try to insert into bucket
    if (bucket->insertRecord(rec))
    {
        blocks_written++;

        logBucketState("INSERT", *bucket);

        // Check if we should split
        double global_load = 0;
        for (const auto &b : directory)
        {
            global_load += b->getOccupancy();
        }
        global_load /= directory.size();

        if (global_load >= max_load_factor)
        {
            LOG_WARN(&logger, "Global load factor exceeded. Triggering split.");
            splitBucket(hash_val);
        }

        return true;
    }
    else
    {
        // Bucket full - must split and retry
        LOG_WARN(&logger, "Bucket " + std::to_string(hash_val) + " full. Splitting.");
        splitBucket(hash_val);

        // Retry insertion with new bucket arrangement
        hash_val = getHashValue(rec.getId(), global_depth);
        if (hash_val < static_cast<int>(directory.size()))
        {
            auto new_bucket = directory[hash_val];
            if (new_bucket->insertRecord(rec))
            {
                blocks_written++;
                logBucketState("INSERT_AFTER_SPLIT", *new_bucket);
                return true;
            }
        }

        return false;
    }
}

const Record *ExtendibleHash::search(int id)
{
    int hash_val = getHashValue(id, global_depth);
    blocks_read++;

    if (hash_val >= static_cast<int>(directory.size()))
    {
        LOG_ERROR(&logger, "Hash value out of bounds");
        return nullptr;
    }

    return directory[hash_val]->findRecord(id);
}

void ExtendibleHash::splitBucket(int bucket_index)
{
    auto bucket = directory[bucket_index];

    if (bucket->local_depth == global_depth)
    {
        doubleDirectory();
    }

    int new_depth = bucket->local_depth + 1;
    int split_index = bucket_index + (1 << bucket->local_depth);

    // Create new bucket
    auto new_bucket = std::make_shared<HashBucket>(
        split_index, new_depth, split_index, block_size);

    // Redistribute records
    std::vector<Record> records_to_keep;

    for (const auto &rec : bucket->records)
    {
        int hash_val = getHashValue(rec.getId(), new_depth);
        if (hash_val >= (1 << (new_depth - 1)))
        {
            new_bucket->records.push_back(rec);
            new_bucket->used_space += rec.getTotalSize();
        }
        else
        {
            records_to_keep.push_back(rec);
        }
    }

    bucket->records = records_to_keep;
    bucket->used_space = 0;
    for (const auto &rec : bucket->records)
    {
        bucket->used_space += rec.getTotalSize();
    }
    bucket->local_depth = new_depth;

    directory[split_index] = new_bucket;
    blocks_written += 2;
    total_buckets++;
    splits_performed++;

    logBucketState("SPLIT_OLD", *bucket);
    logBucketState("SPLIT_NEW", *new_bucket);
}

void ExtendibleHash::doubleDirectory()
{
    int old_size = directory.size();
    std::vector<std::shared_ptr<HashBucket>> new_directory = directory;

    for (int i = 0; i < old_size; ++i)
    {
        new_directory.push_back(directory[i]);
    }

    directory = new_directory;
    global_depth++;

    std::ostringstream oss;
    oss << "Directory doubled - New Global Depth: " << global_depth
        << " New Size: " << directory.size();
    LOG_INFO(&logger, oss.str());
}

void ExtendibleHash::logBucketState(const std::string &op,
                                    const HashBucket &bucket)
{
    std::ostringstream oss;
    oss << "BucketOp[" << op << "] " << bucket.toString();
    LOG_DEBUG(&logger, oss.str());
}

void ExtendibleHash::printStructure() const
{
    std::ostringstream oss;
    oss << "\n=== EXTENDIBLE HASH STRUCTURE V2 ===\n"
        << "Global Depth: " << global_depth << "\n"
        << "Directory Size: " << directory.size() << "\n"
        << "Total Buckets: " << total_buckets << "\n"
        << "Block Size: " << block_size << " bytes\n\n";

    double total_occupancy = 0;
    for (size_t i = 0; i < directory.size(); ++i)
    {
        if (directory[i])
        {
            oss << directory[i]->toString() << "\n";
            total_occupancy += directory[i]->getOccupancy();
        }
    }

    oss << "\nAverage Occupancy: "
        << (total_occupancy / directory.size() * 100) << "%\n";

    LOG_INFO(&logger, oss.str());
}

void ExtendibleHash::printStatistics() const
{
    std::ostringstream oss;
    oss << "\n=== HASH STATISTICS ===\n"
        << "Blocks Read: " << blocks_read << "\n"
        << "Blocks Written: " << blocks_written << "\n"
        << "Total Buckets: " << total_buckets << "\n"
        << "Splits Performed: " << splits_performed << "\n"
        << "Global Depth: " << global_depth << "\n";

    LOG_INFO(&logger, oss.str());
}