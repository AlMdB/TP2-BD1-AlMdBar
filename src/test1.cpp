#include <Chronometer.hpp>
#include <Logger.hpp>
#include <FileReader.hpp>
#include <thread>

int main(int args, char**argv){
        Logger* logger = Logger::getLogger();
        LOG_INFO(logger,"Starting test1.cpp program...");
        LOG_WARN(logger,"Starting test1.cpp program...");
        LOG_ERROR(logger,"Starting test1.cpp program...");
        LOG_DEBUG(logger,"Starting test1.cpp program...");
        LOG_LOG(logger,"Starting test1.cpp program...");


        Chronometer chrono(*logger);
        chrono.start();


    try{
        std::string path = "data/artigo.csv";
        FileReader* reader = FileReader::getReader(path);

        LOG_INFO(logger,"Opening file: " + *reader->getFilePath());

        int line_count = 0;
        while(reader->hasNextLine()){
            std::vector<std::string> tokens = reader->line_processed();

            std::cout << "Line " << reader->getCurrentLine() << ": ";
            for (size_t i = 0; i < tokens.size(); ++i){
                std::cout << tokens[i];
                if(i < tokens.size() - 1) std::cout << " | ";
            }
            std::cout << std::endl;
            line_count++;
        }

        LOG_INFO(logger,"Finished reading file. Total lines: " + std::to_string(line_count));

        chrono.stop();
        long elapsed_ms = chrono.milliseconds();
        LOG_INFO(logger, "Execution finished in " + std::to_string(elapsed_ms)+ " ms");
        std::cout << "Execution time: " << elapsed_ms << " ms" << "std::endl";
    } catch (...){
        return 1;
    }

    return 0;

}