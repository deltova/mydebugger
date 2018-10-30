#include "memory_mapping.h"
#include <cassert>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

MemoryMapping::MemoryMapping(const int& pid, const std::string& exec_name)
{
    std::ifstream proc;
    std::string path_to_proc = "/proc/" + std::to_string(pid) + "/maps";
    proc.open(path_to_proc);
    std::string full_path = fs::canonical(exec_name);
    std::string line = "";
    for (; std::getline(proc, line);)
    {
        if (line.find(full_path) != std::string::npos)
            break;
    }
    assert(line != "");
    char flags[4];
    unsigned long from, to;
    unsigned int pgoff, major, minor;
    unsigned long ino;
    sscanf(line.c_str(), "%lx-%lx %4c %x %x:%x %lu ", &from, &to,
           flags, &pgoff, &major, &minor, &ino);    
    _begin_addr = from;
    _end_addr = to;
}
