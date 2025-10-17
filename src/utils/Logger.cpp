#include "Logger.hpp"
class Logger{
    static Logger logger;

    public:
        void log(const std::string& message, Logger::logsTypes level);

        Logger(Logger& _logger) = delete;
        void operator=(const Logger &) = delete;

        static Logger* getLogger();

        virtual ~Logger(){};    
    protected:
        static Logger* logger;
        std::mutex logger_mutex;
        std::string logTypeString(logsTypes log_type);

};
