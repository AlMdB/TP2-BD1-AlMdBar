#include "Logger.hpp"

const std::string color_code[static_cast<size_t>(Logger::logsTypes::VOID)] = {
    "\x1b[37m", // Default log color: white. 
    "\x1b[36m", // Debug log color: cyan
    "\x1b[32m", // Info log Color: green
    "\x1b[38;5;208m", // warning log color: Orange
    "\x1b[31m" // Critical error log: reds
};
Logger* Logger::logger = nullptr;

Logger* Logger::getLogger(){
    if (logger == nullptr) {
        logger = new Logger();
    }
    return logger;
}


// Get string of the level type to anex to the message
std::string Logger::logTypeString(logsTypes log_type){
    switch (log_type){
        case(logsTypes::LOG):
            return "LOG";
        case(logsTypes::DEBUG):
            return "DEBUG";
        case(logsTypes::ERROR):
            return "ERROR";
        case(logsTypes::INFO):
            return "INFO";
        case(logsTypes::WARN):
            return "WARN";
        default:
            return "UNKNOWN";
    }
}

void Logger::log(const std::string& message, Logger::logsTypes level){
    std::lock_guard <std::mutex> lock(logger_mutex);
    auto now = std::time(nullptr);
    auto tm_now = *std::localtime(&now);


    std::ostringstream timestamp;
    timestamp << std::put_time(&tm_now, "%d-%m-%Y %H:%M:%S");

    std::string color = color_code[static_cast<size_t>(level)];

    const std::string reset = "\x1b[0m";

    std::cout << color 
              << "[" << timestamp.str() << "]"
              << "[" << Logger::logTypeString(level) << "]"
              << message
              << reset
              <<std::endl;

}