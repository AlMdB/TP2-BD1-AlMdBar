#ifndef LOGGER_H
#define LOGGER_H
#include "Chronometer.hpp"
#include <mutex>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
class Logger{
public:
    enum class logsTypes{
        LOG,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        VOID // extra para passar tamanho do log para colorcode
    };
    void log(const std::string& message, logsTypes level);


    static Logger* getLogger();
    static std::string logTypeString(logsTypes log_type);


    virtual ~Logger(){};    
protected:

    Logger() = default;    
    static Logger* logger;
    std::mutex logger_mutex;
    Logger(const Logger& ) = delete;
    Logger& operator=(const Logger&) = delete;
};

    #define LOG(logger,level,message) (logger)->log(message,level)
    #define LOG_LOG(logger,message)   LOG(logger,Logger::logsTypes::LOG,message);
    #define LOG_DEBUG(logger,message) LOG(logger,Logger::logsTypes::DEBUG,message)
    #define LOG_INFO(logger,message)  LOG(logger,Logger::logsTypes::INFO,message)
    #define LOG_WARN(logger,message)  LOG(logger,Logger::logsTypes::WARN,message)
    #define LOG_ERROR(logger,message) LOG(logger,Logger::logsTypes::ERROR,message)

#endif // LOGGER_H