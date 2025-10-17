#ifndef FILE_READER_H
#define FILE_READER_H
#pragma once
#include <fstream>
#include <string>
#include <vector>


class FileReader{
    std::string file_path;
    int cur_lines;
    public: 
        std::vector<std::string> line_processed();

        FileReader(FileReader& _reader) = delete;
        void operator=(const FileReader &) = delete;

        static FileReader* getReader();

        FileReader* getFileReader();

        std::string* getFilePath();

        ~FileReader();


    

    protected:
        FileReader(const std::string _file_path):file_path(_file_path),cur_lines(0){};
        FileReader(const std::string _file_path): file_path(_file_path) {};
        static FileReader* reader;
};




#endif