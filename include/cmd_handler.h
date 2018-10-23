#pragma once
#include "define.h"
#include <memory>
#include <sys/user.h>
#include <err.h>
#include <sys/uio.h>
#include <sys/ptrace.h>

void bp_handler(std::string, debugger_status_t*);
void print_reg_handler(std::string, debugger_status_t*);
void continue_handler(std::string, debugger_status_t*);
void next_handler(std::string, debugger_status_t*);
void help_handler(std::string, debugger_status_t *global_stat);

template <typename T>
inline std::shared_ptr<T>
get_memory(uintptr_t addr, debugger_status_t *global_stat)
{
    T t;
    iovec local{&t, sizeof(T)};
    iovec remote{reinterpret_cast<void*>(addr), sizeof(T)};
    unsigned int n = 1;
    unsigned int flag = 0;
    if (process_vm_readv(global_stat->pid, &local, n, &remote, n, flag) < 0)
        perror("procees_vm_readv_failed");
    return std::make_shared<T>(t);
}

template <typename T>
inline void
set_memory(uintptr_t addr, debugger_status_t *global_stat, T *t)
{
    iovec local{t, sizeof(T)};
    iovec remote{reinterpret_cast<void*>(addr), sizeof(T)};
    unsigned int n = 1;
    unsigned int flag = 0;
    if (process_vm_writev(global_stat->pid, &local, n, &remote, n, flag) < 0)
        perror("procees_vm_writev_failed");
}
