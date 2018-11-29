#pragma once
#include "info_elf.h"
#include "memory_mapping.h"
#include "register.h"
#include <capstone/capstone.h>
#include <cstdint>
#include <iostream>
#include <map>
#include <sys/uio.h>
#include <vector>

typedef struct
{
    uintptr_t addr;
    long old_byte;
} breakpoint_t;

class Debugger : public MemoryMapping
{
  public:
    Debugger(int pid, std::string program_name)
      : MemoryMapping(pid, program_name)
      , _pid(pid)
      , _program_name(program_name)
    {
        if (cs_open(CS_ARCH_X86, CS_MODE_64, &capstone_handle) != CS_ERR_OK)
            std::cerr << "capstone opening failed" << std::endl;
        _input_handlers =
          std::map<std::string, decltype(&Debugger::bp_handler)>(
            {{"b", &Debugger::bp_handler},
             {"d", &Debugger::disas_handler},
             {"c", &Debugger::continue_handler},
             {"h", &Debugger::help_handler},
             {"s", &Debugger::step_handler},
             {"l", &Debugger::get_current_code},
             {"p", &Debugger::print_handler}});
    }
    virtual void call_correct(const std::string& input);
    virtual void get_current_code(std::string input [[maybe_unused]])
    {
        std::cout << "There is no debug info" << std::endl;
    }
    void bp_handler(std::string input);
    void continue_handler(std::string input);
    void disas_handler(std::string input);
    void default_handler(std::string input);
    void help_handler(std::string input);
    void print_handler(std::string input);
    void step_handler(std::string input);

  protected:
    uintptr_t resolve_addr(std::string value);
    int _pid;
    std::string _program_name;
    std::vector<breakpoint_t> _breakpoints;
    csh capstone_handle;
    std::map<std::string, decltype(&Debugger::bp_handler)> _input_handlers;
    // status
};

template <std::size_t N>
inline std::vector<char> get_memory(uintptr_t addr, int pid)
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
