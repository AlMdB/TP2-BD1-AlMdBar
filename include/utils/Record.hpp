#ifndef RECORD_HPP
#define RECORD_HPP

#include <string>
#include <vector>
#include <cstring>
#include <memory>
#include <sstream>

/**
 * RecordField: Represents a single field in a record
 * Stores field name, type, and dynamically sized data
 */
struct RecordField
{
    std::string field_name;
    std::string field_type;
    std::vector<char> field_data;
    int field_size;

    RecordField() : field_size(0) {}

    RecordField(const std::string &name, const std::string &type,
                const char *data, int size)
        : field_name(name), field_type(type), field_size(size)
    {
        if (data && size > 0)
        {
            field_data.assign(data, data + size);
        }
    }

    const char *getData() const
    {
        return field_data.empty() ? nullptr : field_data.data();
    }

    std::string getAsString() const
    {
        if (field_data.empty())
            return "";
        return std::string(field_data.begin(), field_data.end());
    }
};

/**
 * Record: Represents a complete database record
 *
 * Structure in memory:
 * [ID: 4 bytes][Num Fields: 4 bytes][Total Size: 4 bytes]
 * [Field 1 Size: 4][Field 1 Data: n bytes]
 * [Field 2 Size: 4][Field 2 Data: m bytes]
 * ...
 *
 * This way:
 * - Small records don't waste space
 * - Record size accurately reflects actual data
 * - Block calculations are correct
 * - Serialization is straightforward
 */
class Record
{
private:
    int record_id;
    std::vector<RecordField> fields;
    int total_size;

    void calculateSize()
    {
        total_size = 4 + 4 + 4; // ID (4) + NumFields (4) + TotalSize (4)
        for (const auto &field : fields)
        {
            total_size += 4 + field.field_size; // FieldSize (4) + Data
        }
    }

public:
    Record() : record_id(0), total_size(12) {}

    explicit Record(int id) : record_id(id), total_size(12) {}

    /**
     * Add a field to the record
     * @param name Field name
     * @param type Field type (for reference)
     * @param data Pointer to field data
     * @param size Size of field data
     */
    void addField(const std::string &name, const std::string &type,
                  const char *data, int size)
    {
        fields.emplace_back(name, type, data, size);
        calculateSize();
    }

    /**
     * Serialize record to binary format
     * Format: [ID][NumFields][TotalSize][Field1Size][Field1Data]...
     */
    std::vector<char> serialize() const
    {
        std::vector<char> buffer;

        // Write ID
        int id = record_id;
        buffer.insert(buffer.end(), (char *)&id, (char *)&id + 4);

        // Write number of fields
        int num_fields = fields.size();
        buffer.insert(buffer.end(), (char *)&num_fields, (char *)&num_fields + 4);

        // Write total size
        int size = total_size;
        buffer.insert(buffer.end(), (char *)&size, (char *)&size + 4);

        // Write each field
        for (const auto &field : fields)
        {
            int field_sz = field.field_size;
            buffer.insert(buffer.end(), (char *)&field_sz, (char *)&field_sz + 4);

            if (field_sz > 0 && field.getData())
            {
                buffer.insert(buffer.end(), field.getData(),
                              field.getData() + field_sz);
            }
        }

        return buffer;
    }

    /**
     * Deserialize record from binary format
     */
    static Record deserialize(const char *buffer, int buffer_size)
    {
        Record rec;
        int offset = 0;

        if (buffer_size < 12)
            return rec; // Invalid

        // Read ID
        std::memcpy(&rec.record_id, buffer + offset, 4);
        offset += 4;

        // Read number of fields
        int num_fields = 0;
        std::memcpy(&num_fields, buffer + offset, 4);
        offset += 4;

        // Read total size
        std::memcpy(&rec.total_size, buffer + offset, 4);
        offset += 4;

        // Read each field
        for (int i = 0; i < num_fields && offset < buffer_size; ++i)
        {
            int field_sz = 0;
            std::memcpy(&field_sz, buffer + offset, 4);
            offset += 4;

            if (field_sz > 0 && offset + field_sz <= buffer_size)
            {
                RecordField field;
                field.field_size = field_sz;
                field.field_data.assign(buffer + offset, buffer + offset + field_sz);
                rec.fields.push_back(field);
                offset += field_sz;
            }
        }

        return rec;
    }

    // Getters
    int getId() const { return record_id; }
    int getTotalSize() const { return total_size; }
    int getNumFields() const { return fields.size(); }
    const std::vector<RecordField> &getFields() const { return fields; }

    /**
     * Get a specific field by index
     */
    const RecordField *getField(int index) const
    {
        if (index < 0 || index >= static_cast<int>(fields.size()))
        {
            return nullptr;
        }
        return &fields[index];
    }

    /**
     * Get field value as string
     */
    std::string getFieldAsString(int index) const
    {
        const auto *field = getField(index);
        return field ? field->getAsString() : "";
    }

    /**
     * Print record in human-readable format
     */
    std::string toString() const
    {
        std::ostringstream oss;
        oss << "Record[ID=" << record_id << " Size=" << total_size << " bytes]\n";
        for (size_t i = 0; i < fields.size(); ++i)
        {
            oss << "  Field[" << i << "]: " << fields[i].field_name
                << " (" << fields[i].field_type << ") = ";

            if (fields[i].field_size <= 100)
            {
                oss << fields[i].getAsString();
            }
            else
            {
                oss << "[" << fields[i].field_size << " bytes]";
            }
            oss << "\n";
        }
        return oss.str();
    }
};

/**
 * RecordBlock: Represents a physical block in secondary storage
 * Contains multiple records with proper size tracking
 */
class RecordBlock
{
private:
    int block_id;
    int block_size;
    std::vector<Record> records;
    int used_space;
    int free_space;

public:
    RecordBlock(int id, int size)
        : block_id(id), block_size(size), used_space(0), free_space(size) {}

    /**
     * Try to add record to block
     * Returns true if successful, false if no space
     */
    bool addRecord(const Record &rec)
    {
        if (rec.getTotalSize() > free_space)
        {
            return false;
        }
        records.push_back(rec);
        used_space += rec.getTotalSize();
        free_space -= rec.getTotalSize();
        return true;
    }

    // Getters
    int getBlockId() const { return block_id; }
    int getUsedSpace() const { return used_space; }
    int getFreeSpace() const { return free_space; }
    int getRecordCount() const { return records.size(); }
    double getOccupancy() const
    {
        return static_cast<double>(used_space) / block_size;
    }
    const std::vector<Record> &getRecords() const { return records; }

    /**
     * Find record by ID
     */
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
        oss << "Block[ID=" << block_id
            << " Used=" << used_space << "/" << block_size
            << " Occupancy=" << (getOccupancy() * 100) << "%"
            << " Records=" << records.size() << "]\n";
        return oss.str();
    }
};

#endif // RECORD_HPP