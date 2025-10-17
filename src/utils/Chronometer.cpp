#include "Chronometer.hpp"


class Chronometer{
    Logger& logger;
    bool is_running;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    Chronometer(Logger &_logger, bool _is_running):logger(_logger),is_running(_is_running){};
    public:
        void start();
        void stop();
        long milliseconds();
        double seconds();
        void print_dif();
        void print_complete();
        ~Chronometer(){};
};

