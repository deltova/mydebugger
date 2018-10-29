#define _GNU_SOURCE 1
#include "memory_mapping.h"
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <filesystem>

static mem_mapping_t fill(std::string line, std::string exec_name)
{
    mem_mapping_t res = {0, 0};
    unsigned long from, to;
    unsigned int pgoff, major, minor;
    unsigned long ino;
    char flags[4];
    sscanf(line.c_str(), "%lx-%lx %4c %x %x:%x %lu ", &from, &to, flags, &pgoff, &major,
            &minor, &ino);
    auto get_name = line.find("/");
    if (get_name != std::string::npos)
    {
        line = line.substr(get_name);
    }
    std::filesystem::path file_map(line);
    std::filesystem::path exec_path(line);
    auto tamere = std::filesystem::canonical(file_map);
    auto tonpere = std::filesystem::canonical(exec_name);
    if (tamere == tonpere)
    {
        res.beg_addr = from;
        res.end_addr = to;
    }
    return res;
}

static mem_mapping_t parse_and_fill(FILE *file, std::string exec_name)
{
    char **line = reinterpret_cast<char**>(malloc(sizeof(char*)));
    *line = NULL;
    size_t *size = reinterpret_cast<size_t*>(malloc(sizeof(size_t)));
    *size = 0;
    mem_mapping_t res;
    do {
        int len = getline(line, size, file);
        if (len < 0)
            break;
        (*line)[len - 1] = 0;
        printf("%s\n", *line);
        res = fill(*line, exec_name);
        if (res.beg_addr != 0 && res.end_addr != 0)
            break;
    }
    while (1);
    free(size);
    free(line);
    return res;
}

static std::string inject_full_path(std::string exec)
{
    auto dir_name = std::string(get_current_dir_name());
    return dir_name + "/" + exec;
}

mem_mapping_t dump_mem(int pid, std::string executable_name)
{
    executable_name = inject_full_path(executable_name);
    std::string value = "/proc/" + std::to_string(pid) + "/maps";
    FILE *file = fopen(value.c_str(), "r");
    mem_mapping_t res = parse_and_fill(file, executable_name);
    fclose(file);
    return res;
}
