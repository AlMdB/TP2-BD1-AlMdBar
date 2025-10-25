#include "SchemaParser.hpp"
#include "ExtendibleHash.hpp"
#include "Chronometer.hpp"
#include <iostream>
#include <fstream>
#include <random>

void generateTestCSV(const std::string &csv_file, int num_records)
{
    std::ofstream csv(csv_file);
    csv << "ID,Titulo,Ano,Autores,Citacoes,Atualizacao,Snippet\n";

    std::mt19937 gen(42);
    std::uniform_int_distribution<> year_dist(2015, 2024);
    std::uniform_int_distribution<> citations_dist(0, 1000);

    const std::vector<std::string> titles = {
        "Machine Learning Fundamentals",
        "Deep Learning Applications",
        "Natural Language Processing",
        "Computer Vision Advances",
        "Quantum Computing Basics",
        "Blockchain Technology",
        "Internet of Things",
        "Cloud Computing",
        "Edge Computing Systems",
        "Distributed Algorithms"};

    const std::vector<std::string> authors = {
        "Smith, J. and Johnson, M.",
        "Williams, A. and Brown, B.",
        "Jones, C. and Davis, D.",
        "Miller, E. and Wilson, F.",
        "Moore, G. and Taylor, H."};

    const std::vector<std::string> snippets = {
        "This paper presents a novel approach to machine learning optimization",
        "We propose an innovative framework for data analysis and processing",
        "Our work demonstrates significant improvements in computational efficiency",
        "This research advances the state-of-the-art in modern computing",
        "We introduce new techniques for solving complex problems efficiently"};

    for (int i = 1; i <= num_records; ++i)
    {
        int idx_title = (i - 1) % titles.size();
        int idx_authors = (i - 1) % authors.size();
        int idx_snippet = (i - 1) % snippets.size();

        csv << i << ","
            << titles[idx_title] << " #" << i << ","
            << year_dist(gen) << ","
            << authors[idx_authors] << ","
            << citations_dist(gen) << ","
            << "2024-10-24 10:30:00" << ","
            << snippets[idx_snippet] << " for record " << i << "\n";
    }

    csv.close();
}

int main(int argc, char *argv[])
{
    Logger *logger = Logger::getLogger();

    std::string schema_file = "schema.almdb";
    std::string csv_file = "test_data.csv";
    int num_records = 100;

    if (argc > 1)
    {
        csv_file = argv[1];
    }
    if (argc > 2)
    {
        num_records = std::stoi(argv[2]);
    }

    LOG_INFO(logger, "=== DATABASE UPLOAD TEST PROGRAM ===");

    // Generate test CSV if needed
    LOG_INFO(logger, "Generating test CSV with " + std::to_string(num_records) + " records");
    generateTestCSV(csv_file, num_records);

    Chronometer total_time(*logger);
    total_time.start();

    // Parse schema
    LOG_INFO(logger, "Step 1: Parsing schema from " + schema_file);
    SchemaParser parser(*logger);

    if (!parser.parseSchema(schema_file))
    {
        LOG_ERROR(logger, "Failed to parse schema");
        return 1;
    }

    parser.printSchema();

    // Initialize DBManager
    LOG_INFO(logger, "Step 2: Initializing Database Manager");
    DBManager db_manager(*logger);
    db_manager.initializeFromSchema(parser.getFields(), parser.getTableName());
    db_manager.printConfig();

    const auto &config = db_manager.getConfig();

    // Initialize ExtendibleHash
    LOG_INFO(logger, "Step 3: Initializing Extendible Hash Structure");
    ExtendibleHash hash(*logger, 16, config.block_size,
                        config.record_size, config.load_factor);

    // Read and insert records
    LOG_INFO(logger, "Step 4: Reading CSV and inserting records into hash");

    std::ifstream csv_stream(csv_file);
    std::string line;
    int inserted_count = 0;
    int skipped_count = 0;

    // Skip header
    std::getline(csv_stream, line);

    while (std::getline(csv_stream, line) && inserted_count < num_records)
    {
        // Parse CSV line (simple comma-separated)
        int id = 0;
        try
        {
            size_t first_comma = line.find(',');
            id = std::stoi(line.substr(0, first_comma));

            // Use entire line as data for simplicity
            if (hash.insert(id, line.c_str(), line.length()))
            {
                inserted_count++;
            }
            else
            {
                skipped_count++;
            }

            if (inserted_count % 10 == 0)
            {
                std::ostringstream oss;
                oss << "Progress: " << inserted_count << "/" << num_records;
                LOG_DEBUG(logger, oss.str());
            }
        }
        catch (const std::exception &e)
        {
            LOG_WARN(logger, "Failed to parse record: " + std::string(e.what()));
            skipped_count++;
        }
    }

    csv_stream.close();

    // Print results
    LOG_INFO(logger, "Step 5: Hash structure finalized");
    hash.printStructure();

    int blocks_read, blocks_written, total_blocks;
    hash.getStats(blocks_read, blocks_written, total_blocks);

    total_time.stop();

    // Final report
    std::ostringstream report;
    report << "\n=== UPLOAD TEST REPORT ===\n"
           << "Total Records Inserted: " << inserted_count << "\n"
           << "Records Skipped: " << skipped_count << "\n"
           << "Blocks Read: " << blocks_read << "\n"
           << "Blocks Written: " << blocks_written << "\n"
           << "Total Blocks: " << total_blocks << "\n"
           << "Total Execution Time: " << total_time.milliseconds() << " ms\n"
           << "Average Time per Record: "
           << (total_time.milliseconds() / std::max(inserted_count, 1)) << " ms\n";

    LOG_INFO(logger, report.str());

    std::cout << report.str();

    return 0;
}