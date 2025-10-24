#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <string>
#include "Logger.hpp"
#include "HashTableManager.hpp"
#include "BTreePManager.hpp"

class DatabaseManager
{
private:
    std::string datas_dir_path;
    std::string data_file_path;
    std::fstream data_stream;
    Logger *logger;

    HashtableManager hashtable_man;
    BTreePManager indexes;

    size_t total_blocks;
    size_t current_offset;
    std::vector<std::pair<long, uint16_t>> data_meta;

    static const size_t BLOCK_SIZE;
}
#endif // DB_MANAGER_H