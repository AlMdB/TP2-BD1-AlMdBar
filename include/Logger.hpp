#ifndef LOGGER_H
#define LOGGER_H
#include <mutex>
#include <string>
class Logger{
    enum class logsTypes{
        LOG,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        VOID // extra para passar tamanho do log para colorcode
    };
    const std::string color_code[static_cast<size_t>(logsTypes::VOID)] = {
        "\x1b[37m", // Default log color: white. 
        "\x1b[31m", // Debug log color: cyan
        "\x1b[32m", // Info log Color: green
        "\x1b[38;5;208m", // warning log color: Orange
        "\x1b[33m" // Critical error log: reds
    };

    public:
        void log(const std::string& message, logsTypes level);

        Logger(Logger& _logger) = delete;
        void operator=(const Logger &) = delete;

        static Logger* getLogger();

        virtual ~Logger(){};    
        
    protected:
        static Logger* logger;
        std::mutex logger_mutex;
        std::string logTypeString(logsTypes log_type);

};

    #define LOG(logger,level,message) (logger).log(message,level)
    #define LOG_DEBUG() (logger).log(message,logsTypes::DEBUG)
    #define LOG_INFO() (logger).log(message,logsTypes::INFO)
    #define LOG_WARN() (logger).log(message,logsTypes::WARN)
    #define LOG_ERROR() (logger).log(message,logsTypes::ERROR)

#endif // LOGGER_H