#pragma once
#include "define.h"
#include <memory>
#include <sys/user.h>
#include <err.h>
#include <sys/uio.h>
#include <sys/ptrace.h>

void bp_handler(std::string, debugger_status_t*);
void print_handler(std::string, debugger_status_t*);
void continue_handler(std::string, debugger_status_t*);
void next_handler(std::string, debugger_status_t*);
void help_handler(std::string, debugger_status_t *global_stat);
void step_handler(std::string input, debugger_status_t *global_stat);

template <std::size_t N>
inline std::vector<char>
get_memory(uintptr_t addr, debugger_status_t *global_stat)
{
    char buffer[N];
    iovec local{&buffer, N};
    iovec remote{reinterpret_cast<void*>(addr), N};
    unsigned int n = 1;
    unsigned int flag = 0;
    if (process_vm_readv(global_stat->pid, &local, n, &remote, n, flag) < 0)
        perror("procees_vm_readv_failed");
    return std::vector<char>(std::begin(buffer), std::end(buffer));
}
