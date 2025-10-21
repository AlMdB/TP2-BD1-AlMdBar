#include "FileReader.hpp"


FileReader* FileReader::reader = nullptr;

FileReader*FileReader::getReader(const std::string& file_path){
    if(reader == nullptr){
        reader = new FileReader(file_path);
    }
    return reader;
}

std::string* FileReader::getFilePath(){
    return &file_path;
}

int FileReader::getCurrentLine() const {
    return cur_lines;
}

std::vector<std::string> FileReader::line_processed(){
    std::vector<std::string> tokens;
    std::string token;

    while(std::getline(file_stream,token,';')){
        tokens.push_back(token);
    }
    cur_lines++;
    return tokens;
}

bool FileReader::hasNextLine(){
    return file_stream.peek()!=EOF;
}


void FileReader::reset(){
    file_stream.clear();
    file_stream.seekg(0);
    cur_lines = 0;
}


FileReader::~FileReader(){
    if(file_stream.is_open()){
        file_stream.close();
    }
}