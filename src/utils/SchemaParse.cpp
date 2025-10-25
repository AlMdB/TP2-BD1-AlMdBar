void SchemaParser::trim(std::string &str)
{
    str.erase(0, str.find_first_not_of(" \t\n\r"));
    str.erase(str.find_last_not_of(" \t\n\r") + 1);
}

std::string SchemaParser::toUpperCase(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

int SchemaParser::parseSize(const std::string &size_str)
{
    std::regex size_pattern(R"(\((\d+)(?:,(\d+))?\))");
    std::smatch match;

    if (std::regex_search(size_str, match, size_pattern))
    {
        return std::stoi(match[1]);
    }
    return 0;
}

void SchemaParser::parseFieldDefinition(const std::string &field_def)
{
    std::istringstream iss(field_def);
    std::string name, type, size_part;

    iss >> name >> type;
    trim(name);
    trim(type);

    std::string remainder;
    std::getline(iss, remainder);
    trim(remainder);

    int size = 0, min_size = 0, max_size = 0;

    std::string upper_type = toUpperCase(type);

    if (upper_type == "INT" || upper_type == "INTE")
    {
        size = 4;
    }
    else if (upper_type == "DATAH")
    {
        size = 8;
    }
    else if (upper_type.find("ALFA") != std::string::npos)
    {
        size = parseSize(field_def);
    }
    else if (upper_type.find("VARALFA") != std::string::npos)
    {
        std::regex var_pattern(R"(VARALFA\s*\(\s*(\d+)\s*,\s*(\d+)\s*\))");
        std::smatch match;

        if (std::regex_search(field_def, match, var_pattern))
        {
            min_size = std::stoi(match[1]);
            max_size = std::stoi(match[2]);
            size = max_size;
        }
    }

    fields.emplace_back(name, type, size, min_size, max_size);

    LOG_DEBUG(&logger, "Parsed field: " + name + " (" + type + ") - Size: " +
                           std::to_string(size) + " bytes");
}

bool SchemaParser::parseSchema(const std::string &schema_file)
{
    std::ifstream file(schema_file);
    if (!file.is_open())
    {
        LOG_ERROR(&logger, "Could not open schema file: " + schema_file);
        return false;
    }

    std::string line, full_content;
    while (std::getline(file, line))
    {
        full_content += " " + line;
    }
    file.close();

    std::regex table_pattern(R"(CRIA\s+TABELA\s+(\w+)\s*\((.*)\))");
    std::smatch match;

    if (!std::regex_search(full_content, match, table_pattern))
    {
        LOG_ERROR(&logger, "Invalid schema format");
        return false;
    }

    table_name = match[1];
    std::string fields_str = match[2];

    std::regex field_pattern(R"((\w+\s+(?:INT|INTE|ALFA|VARALFA|DATAH)[^,]*))");
    std::sregex_iterator iter(fields_str.begin(), fields_str.end(), field_pattern);
    std::sregex_iterator end;

    while (iter != end)
    {
        std::string field_def = iter->str();
        trim(field_def);
        if (!field_def.empty())
        {
            parseFieldDefinition(field_def);
        }
        ++iter;
    }

    LOG_INFO(&logger, "Schema parsed successfully. Table: " + table_name +
                          " Fields: " + std::to_string(fields.size()));

    return true;
}

int SchemaParser::calculateRecordSize() const
{
    int total = 0;
    for (const auto &field : fields)
    {
        total += field.size;
    }
    return total;
}

void SchemaParser::printSchema() const
{
    std::ostringstream oss;
    oss << "\n=== SCHEMA INFORMATION ===\n";
    oss << "Table: " << table_name << "\n";
    oss << "Fields:\n";

    for (const auto &field : fields)
    {
        oss << "  - " << field.name << " (" << field.type << ") Size: "
            << field.size << " bytes\n";
    }
    oss << "Total Record Size: " << calculateRecordSize() << " bytes\n";

    LOG_INFO(&logger, oss.str());
}

void DBManager::calculateBlockCapacity()
{
    config.max_records_per_block = config.block_size / config.record_size;
    if (config.max_records_per_block == 0)
    {
        config.max_records_per_block = 1;
    }
}

void DBManager::calculateOptimalLoadFactor()
{
    config.load_factor = 0.7;
    LOG_INFO(&logger, "Load factor set to: " + std::to_string(config.load_factor));
}

void DBManager::initializeFromSchema(const std::vector<FieldSpec> &schema_fields,
                                     const std::string &table_name)
{
    fields = schema_fields;
    config.table_name = table_name;

    config.record_size = 0;
    for (const auto &field : fields)
    {
        config.record_size += field.size;
    }

    calculateBlockCapacity();
    calculateOptimalLoadFactor();

    config.initial_buckets = 16;

    std::ostringstream oss;
    oss << "Initialized buckets: " << config.initial_buckets;
    LOG_DEBUG(&logger, oss.str());
}

void DBManager::setBlockSize(int size)
{
    config.block_size = size;
    calculateBlockCapacity();
}

void DBManager::setPageSize(int size)
{
    config.page_size = size;
}

void DBManager::printConfig() const
{
    std::ostringstream oss;
    oss << "\n=== DATABASE CONFIG ===\n"
        << "Table: " << config.table_name << "\n"
        << "Block Size: " << config.block_size << " bytes\n"
        << "Page Size: " << config.page_size << " bytes\n"
        << "Record Size: " << config.record_size << " bytes\n"
        << "Max Records per Block: " << config.max_records_per_block << "\n"
        << "Load Factor: " << config.load_factor << "\n"
        << "Initial Hash Buckets: " << config.initial_buckets << "\n";

    LOG_INFO(&logger, oss.str());
}