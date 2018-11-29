#include "register.h"
#include <array>
#include <err.h>
#include <iostream>

#define REGS_SIZE 27

static constexpr std::array<const char *, REGS_SIZE> regs = {
  "r15",    "r14", "r13", "r12",     "rbp",     "rbx", "r11",      "r10", "r9",
  "r8",     "rax", "rcx", "rdx",     "rsi",     "rdi", "orig_rax", "rip", "cs",
  "eflags", "rsp", "ss",  "fs_base", "gs_base", "ds",  "es",       "fs",  "gs"};

static struct user_regs_struct get_regs(int pid)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    return regs;
}

uintptr_t get_specific_register(const std::string& reg_name, int pid)
{
    struct user_regs_struct registers = get_regs(pid);
    int i = 0;
    while (regs[i] != reg_name && i < REGS_SIZE - 1)
        ++i;
    return *(uintptr_t*)(((char*)&registers + sizeof(ulli) * i));
}

void set_specific_register(const std::string& reg_name, int pid, uintptr_t val)
{
    struct user_regs_struct registers = get_regs(pid);
    int i = 0;
    while (regs[i] != reg_name && i < REGS_SIZE - 1)
        ++i;
    *(uintptr_t*)((char*)&registers + sizeof(ulli) * i) = val;
    if (ptrace(PTRACE_SETREGS, pid, NULL, &registers) < 0)
        perror("error seting reg");
}

void print_register(int pid, const std::string& name)
{
    const auto rip_val = get_specific_register(name, pid);
    std::cout << "rip after handling bp" << std::hex << rip_val << std::dec
              << '\n';
}
