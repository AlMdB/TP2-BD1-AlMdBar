#ifndef RECORD_H
#define RECORD_H
#pragma once
#include "Logger.hpp"
#include "vector"
#include <cstddef>

enum class DBFieldType
{
    INT,
    ALFA,
    DATEHOUR,
    VARALFA,
    UNKNOWN // useful for some error detection
};

struct FieldGeneral
{
    std::string name;
    DBFieldType type;
    uint32_t baseSize;
    bool isPrimary;
    bool isIndexed;
};

typedef struct FieldSpecific
{
    uint32_t max_var_size;
    uint32_t curr_var_size;
}
{
}
rec_metadata;

class RecordHead
{
    typedef struct DBColumnInfo
    {
        std::string columnName;
        DBType type;
        size_t size;
    } DBCol;

    std::vector<DBColumnInfo> *Record_TInfo;
};

class Record
{
public:
    std::byte record_data;
    size_t record_size;
    Record(std::byte _data, size_t _size) : record_data(_data), record_size(_size) {};
}

#endif