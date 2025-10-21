#include "Chronometer.hpp"

void Chronometer::start(){
    std::lock_guard<std::mutex> lock(time_mutex);
    start_time = std::chrono::high_resolution_clock::now();
    is_running = true;
    LOG_INFO(&logger,"Chronometer Started");
};

void Chronometer::stop(){
    std::lock_guard<std::mutex> lock(time_mutex);
    end_time = std::chrono::high_resolution_clock::now();
    is_running=false;
    LOG_DEBUG(&logger,"Chronometer stopped");
};

long Chronometer::milliseconds(){
    std::lock_guard<std::mutex> lock(time_mutex);
    auto elapse_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    return elapse_ms;
};

double Chronometer::seconds(){
    std::lock_guard<std::mutex> lock(time_mutex);
    auto elapse_sec = std::chrono::duration_cast<std::chrono::seconds>(end_time -start_time).count() / 1000.0;
    return elapse_sec;
};

void Chronometer::print(const std::string& program){
    std::ostringstream oss;
    oss << "Program: " << program <<  " Finished in: " << milliseconds();
    LOG_INFO(&logger,oss.str());
}

std::chrono::time_point<std::chrono::high_resolution_clock> Chronometer::get_current_time(){
    auto current = std::chrono::high_resolution_clock::now();
    return current;
}
