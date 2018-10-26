#include "register.h"
#include <err.h>
#include <vector>

static std::vector<std::string> regs = {"r15", "r14", "r13", "r12", "rbp", "rbx",
"r11", "r10", "r9", "r8", "rax", "rcx", "rdx", "rsi", "rdi",
"orig_rax", "rip", "cs", "eflags", "rsp", "ss", "fs_base",
"gs_base", "ds", "es", "fs", "gs"};

static struct user_regs_struct get_regs(debugger_status_t* global_stat)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, global_stat->pid, NULL, &regs);
    return regs;
}

uintptr_t get_specific_register(std::string reg_name,
                                debugger_status_t *global_stat)
{
    struct user_regs_struct registers = get_regs(global_stat);
    int i = 0;
    while(regs[i] != reg_name && i < 26)
        ++i;
    return *(uintptr_t*)(((char*)&registers + sizeof(ulli) * i));
}

void set_specific_register(std::string reg_name,
                                debugger_status_t *global_stat, uintptr_t val)
{
    struct user_regs_struct registers = get_regs(global_stat);
    int i = 0;
    while(regs[i] != reg_name && i < 26)
        ++i;
    *(uintptr_t*)((char*)&registers + sizeof(ulli) * i) = val;
    if (ptrace(PTRACE_SETREGS, global_stat->pid, NULL, &registers) < 0)
        perror("error seting reg");
}
