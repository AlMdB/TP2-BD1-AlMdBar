#ifndef FILE_READER_H
#define FILE_READER_H
#pragma once
#include "Logger.hpp"
#include "Chronometer.hpp"
#include <fstream>
#include <string>
#include <vector>


class FileReader{
    std::string file_path;
    int cur_lines;
    std::ifstream file_stream;
    
public: 

    std::vector<std::string> line_processed();

    bool hasNextLine();
    void reset();


    FileReader(FileReader& _reader) = delete;
    void operator=(const FileReader &) = delete;

    static FileReader* getReader(const std::string& file_path);

    std::string* getFilePath();

    int getCurrentLine() const;

    ~FileReader();
    

protected:
    FileReader(const std::string _file_path):file_path(_file_path),cur_lines(0){
            file_stream.open(_file_path);
    };
    static FileReader* reader;
};

#endif