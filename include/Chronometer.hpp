#ifndef CHRONOMETER_H
#define CHRONOMETER_H
#include <chrono>
#include <iostream>
#include <mutex>
#include <iomanip>
#include <sstream>
#include "Logger.hpp"

class Logger;
class Chronometer{
    Logger& logger;
    bool is_running;
    std::mutex time_mutex;
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
public:
    explicit Chronometer(Logger &_logger):logger(_logger), is_running(false) {}
    void start();
    void stop();
    long milliseconds();
    double seconds();
    void print(const std::string& program);
    static std::chrono::time_point<std::chrono::high_resolution_clock> get_current_time();
    ~Chronometer() = default;
};


#endif // CHRONOMETER_H