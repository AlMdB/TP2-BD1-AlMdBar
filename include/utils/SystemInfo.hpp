#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include "Logger.hpp"

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#include <winioctl.h>
#define LINUX false
#define WINDOWS true
#else
#include <sys/statvfs.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>
#define WINDOWS false
#define LINUX true
#endif

class SystemInfo
{
private:
    long page_size;
    long block_size;
    Logger *logger;

public:
    static bool detectDocker()
    {
#ifdef __linux__
        std::ifstream cgroup("/proc/1/cgroup");
        if (cgroup.is_open())
        {
            std::string line;
            while (std::getline(cgroup, line))
            {
                if (line.find("docker") != std::string::npos)
                {
                    return true;
                }
            }
            cgroup.close();
        }
        std::ifstream dockerenv("/.dockerenv");
        if (dockerenv.good())
            return true;
#endif
        return false;
    }

    static std::string getStorageType(const std::string &path)
    {
        std::string output;
        std::osstream oss;
#ifdef __linux__
        output = getStorageTypeLinux(path);
#elif _WIN32
        output = getStorageTypeWindows(path);
#else
        LOG_ERROR(logger, "MAC system identified");
        return "";
#endif
        oss << "File on Path: " << path << "\n Stored in: " << output << std::endl;
        LOG_INFO(logger, oss.str());
        return output;
    }

    static long getBlockSize(const std::string &path)
    {
        std::osstream oss;
        std::string output;
#ifdef __linux__
        struct statvfs stat;
        if (statvfs(path.c_str(), &stat) == 0)
        {
            output = stats.f_bsize;
        }
#elif _WIN32
        output = getBlockSizeWindows(path);
#else
        LOG_ERROR(logger, "Unknow system cant retrieve block size");
        return -1;
#endif
        return output;
    }

private:
    static std::string getStorageTypeLinux(const std::string &path)
    {
        struct stat statbuf;
        if (stat(path.c_str(), &statbuf) != 0)
        {
            return "Error getting stat";
        }

        FILE *df = popen(("df " + path + " | tail -1").c_str(), "r");
        if (!df)
            return "Error "
    }
}

#endif // SYSTEM_INFO_H