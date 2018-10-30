#pragma once
#include <sys/uio.h>
#include "define.h"
#include "memory_mapping.h"

class Debugger
{
public:
    Debugger(int pid, std::string program_name, mem_mapping_t mapping)
        : _pid(pid),
          _program_name(program_name),
          _mapping(mapping)
    {}

    void bp_handler(std::string input);
    void continue_handler(std::string input);
    void default_handler(std::string input);
    void help_handler(std::string input);
    void print_handler(std::string input);
    void step_handler(std::string input);
private:
    uintptr_t resolve_addr(std::string value);
    int _pid;
    std::string _program_name;
    mem_mapping_t _mapping;
    std::vector<breakpoint_t> _breakpoints;
    //status
};

template <std::size_t N>
inline std::vector<char>
get_memory(uintptr_t addr, int pid)
{
    char buffer[N];
    iovec local{&buffer, N};
    iovec remote{reinterpret_cast<void*>(addr), N};
    unsigned int n = 1;
    unsigned int flag = 0;
    if (process_vm_readv(pid, &local, n, &remote, n, flag) < 0)
        perror("procees_vm_readv_failed");
    return std::vector<char>(std::begin(buffer), std::end(buffer));
}
