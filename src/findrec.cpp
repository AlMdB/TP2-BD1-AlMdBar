



int main(int argc, char** argv){
    if (argc != 2){
        LOG_ERROR("No files given. The command should follow the struct findrec ${ID}");
        return 1;
    }
    Logger logger = getLogger();
    Chronometer chrono(logger);
    chrono.start();




    chrono.end();
    return 0;
}