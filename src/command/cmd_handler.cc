#include "cmd_handler.h"
#include "info_elf.h"
#include <sys/wait.h>
#include <string>
#include <iostream>

typedef unsigned long long int ulli;

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

static ulli get_specific_register(std::string reg_name, debugger_status_t *global_stat)
{
    struct user_regs_struct registers = get_regs(global_stat);
    int i = 0;
    while(regs[i] != reg_name && i < 26)
        ++i;
    return *(ulli*)(((char*)&registers + sizeof(ulli) * i));
}

static int need_break(debugger_status_t global_stat, ulli rip_val)
{
    for (const auto& elem: global_stat.breakpoint_list)
    {
        if (elem == rip_val)
            return 1;
    }
    return 0;
}

static unsigned long resolve_addr(std::string value, debugger_status_t *global_stat)
{
    if (value[0] == '0' && value[1] == 'x')
    {
        return (unsigned long)strtol(value.c_str(), NULL, 0);
    }
    else
    {
        //add offset to the begining  of the programm in memory
        return global_stat->mapping.beg_addr
               + addr_from_name(global_stat->program_name, value.c_str());
    }

}

void help_handler(std::string input, debugger_status_t *global_stat)
{
    (void)input;
    (void)global_stat;
    printf("Available command:\n");
    printf("\tb $addr: set a breakpoint at $addr\n");
    printf("\tc: Continue to the next breakpoint\n");
    printf("\th: print the helper of commands\n");
    printf("\tn: Go to next instruction\n");
    printf("\tp $register: print the value of the $register\n");
}


void print_reg_handler(std::string input, debugger_status_t *global_stat)
{
    ulli reg_val = get_specific_register(
                   std::string(input.begin() + 2, input.end()), global_stat);
    printf("%lld\n", reg_val);
}

void next_handler(std::string input, debugger_status_t *global_stat)
{
    (void)input;
    ptrace(PTRACE_SINGLESTEP, global_stat->pid, NULL, NULL);
    int status;
    waitpid(global_stat->pid, &status, 0);
}

void bp_handler(std::string input, debugger_status_t *global_stat)
{
    unsigned long addr = resolve_addr(
                    std::string(input.begin() + 2, input.end()), global_stat);
    global_stat->breakpoint_list.push_back(addr);
    void* addr_bp = (int*)addr;
    long ret;
    uint8_t c = 0xcc;
    printf("%x\n", addr);
    ret = ptrace(PTRACE_POKETEXT, global_stat->pid, addr_bp, &c);
    if (ret < 0)
        printf("ERROR peektext\n");
    else
        printf("%ld\n", ret);
}


void continue_handler(std::string input, debugger_status_t *global_stat)
{
    (void)input;
    int status;
    global_stat->status = CONTINUE;
    do {
        int res = ptrace(PTRACE_SINGLESTEP, global_stat->pid, NULL, NULL);
        if (res < 0)
            printf("ERROR\n");
        waitpid(global_stat->pid, &status, 0);
        ulli rip_val = get_specific_register("rip\0", global_stat);
        if (need_break(*global_stat, rip_val))
        {
            printf("Stopped by Breakpoint\n");
            break;
        }
    } while(WEXITSTATUS(status));
    if (!WEXITSTATUS(status))
        printf("Programm stopped\n");
}
