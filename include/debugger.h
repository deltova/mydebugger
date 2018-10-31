#pragma once
#include <vector>
#include <cstdint>
#include <sys/uio.h>
#include "memory_mapping.h"

typedef struct {
    uintptr_t addr;
    long old_byte;
} breakpoint_t;

class Debugger: public MemoryMapping
{
public:
    Debugger(int pid, std::string program_name)
        : MemoryMapping(pid, program_name),
          _pid(pid),
          _program_name(program_name)
    {
    }
    void bp_handler(std::string input);
    void continue_handler(std::string input);
    void default_handler(std::string input);
    void help_handler(std::string input);
    void print_handler(std::string input);
    void step_handler(std::string input);
protected:
    uintptr_t resolve_addr(std::string value);
    int _pid;
    std::string _program_name;
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
