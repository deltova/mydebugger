#include "cmd_handler.h"

static char *regs[] = {"r15\0", "r14\0", "r13\0", "r12\0", "rbp\0", "rbx\0",
"r11\0", "r10\0", "r9\0", "r8\0", "rax\0", "rcx\0", "rdx\0", "rsi\0", "rdi\0",
"orig_rax\0", "rip\0", "cs\0", "eflags\0", "rsp\0", "ss\0", "fs_base\0",
"gs_base\0", "ds\0", "es\0", "fs\0", "gs\0"};

static struct user_regs_struct get_regs(debugger_status_t* global_stat)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, global_stat->pid, NULL, &regs);
    return regs;
}

void print_reg_handler(char *input, debugger_status_t *global_stat)
{
    char *reg_name = input + 2;
    struct user_regs_struct registers = get_regs(global_stat);
    int i = 0;
    while(strcmp(regs[i], reg_name) && i < 26)
        ++i;
    typedef unsigned long long int ulli;
    ulli reg = *(ulli*)(((char*)&registers + sizeof(ulli) * i));
    printf("%lld\n", reg);
}

void next_handler(char *input, debugger_status_t *global_stat)
{
    ptrace(PTRACE_SINGLESTEP, global_stat->pid, NULL, NULL);
    int status;
    waitpid(global_stat->pid, &status, 0);
}

void bp_handler(char *input, debugger_status_t *global_stat)
{
}

void continue_handler(char *input, debugger_status_t *global_stat)
{
}

