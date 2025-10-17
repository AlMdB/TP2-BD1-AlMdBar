#ifndef CHRONOMETER_H
#define CHRONOMETER_H
#pragma once
#include <chrono>
#include <iostream>
#include "Logger.hpp"

class Chronometer{
    Logger& logger;
    bool is_running;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    Chronometer(Logger &_logger):logger(_logger){};
    public:
        void start();
        void stop();
        long milliseconds();
        double seconds();
        void print_dif();
        void print_complete();
        ~Chronometer(){};
};


#endif // CHRONOMETER_H