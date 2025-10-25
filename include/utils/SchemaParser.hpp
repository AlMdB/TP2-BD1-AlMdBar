#ifndef SCHEMA_PARSER_HPP
#define SCHEMA_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <algorithm>
#include "Logger.hpp"

struct FieldSpec
{
    std::string name;
    std::string type;
    int size;
    int min_size;
    int max_size;

    FieldSpec(const std::string &n, const std::string &t, int s = 0, int min_s = 0, int max_s = 0)
        : name(n), type(t), size(s), min_size(min_s), max_size(max_s) {}
};

class SchemaParser
{
private:
    Logger &logger;
    std::vector<FieldSpec> fields;
    std::string table_name;

    void trim(std::string &str);
    std::string toUpperCase(const std::string &str);
    void parseFieldDefinition(const std::string &field_def);
    int parseSize(const std::string &size_str);

public:
    SchemaParser(Logger &log) : logger(log) {}

    bool parseSchema(const std::string &schema_file);
    const std::vector<FieldSpec> &getFields() const { return fields; }
    const std::string &getTableName() const { return table_name; }
    int calculateRecordSize() const;
    void printSchema() const;
};

class DBManager
{
public:
    struct DBConfig
    {
        int block_size;
        int page_size;
        int max_records_per_block;
        int record_size;
        double load_factor;
        int initial_buckets;
        std::string table_name;
    };

private:
    Logger &logger;
    std::vector<FieldSpec> fields;
    DBConfig config;

    void calculateBlockCapacity();
    void calculateOptimalLoadFactor();

public:
    DBManager(Logger &log) : logger(log)
    {
        config.block_size = 4096;
        config.page_size = 4096;
        config.load_factor = 0.7;
    }

    void initializeFromSchema(const std::vector<FieldSpec> &schema_fields,
                              const std::string &table_name);
    const DBConfig &getConfig() const { return config; }
    void printConfig() const;
    void setBlockSize(int size);
    void setPageSize(int size);
};