#ifndef RECORD_H
#define RECORD_H
#pragma once
#include "Logger.hpp"
#include "vector"
#include <cstddef>

class RecordHead{
    enum class DBType{
        INT,
        ALFA,
        DATEHOUR,
        VARALFA,
    };

    typedef struct DBColumnInfo{
        std::string columnName;
        DBType type;
        size_t size;
    }DBCol;


    std::vector<DBColumnInfo>* Record_TInfo;
};

class Record{
public:
    std::byte record_data;
    size_t record_size;
    Record(std::byte _data,size_t _size) : record_data(_data),record_size(_size){};
}

#endif