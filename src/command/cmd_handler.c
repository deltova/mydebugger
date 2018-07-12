#include "cmd_handler.h"
#include <sys/wait.h>

typedef unsigned long long int ulli;
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
static ulli get_specific_register(char *reg_name, debugger_status_t *global_stat)
{
    struct user_regs_struct registers = get_regs(global_stat);
    int i = 0;
    while(strcmp(regs[i], reg_name) && i < 26)
        ++i;
    return *(ulli*)(((char*)&registers + sizeof(ulli) * i));
}

void print_reg_handler(char *input, debugger_status_t *global_stat)
{
    ulli reg_val = get_specific_register(input + 2, global_stat);
    printf("%lld\n", reg_val);
}

void next_handler(char *input, debugger_status_t *global_stat)
{
    ptrace(PTRACE_SINGLESTEP, global_stat->pid, NULL, NULL);
    int status;
    waitpid(global_stat->pid, &status, 0);
}


static unsigned long resolve_addr(char *value, debugger_status_t *global_stat)
{
    if (value[0] == '0' && value[1] == 'x')
    {
        return (unsigned long)strtol(value, NULL, 0);
    }
    else
    {
        //add offset to the begining  of the programm in memory
        return global_stat->mapping.beg_addr
               + addr_from_name(global_stat->program_name, value);
    }

}

void bp_handler(char *input, debugger_status_t *global_stat)
{
    unsigned long addr = resolve_addr(input + 2, global_stat);
    list_push(global_stat->breakpoint_list, addr);
    void* addr_bp = (int*)addr;
    long ret;
    ret = ptrace(PTRACE_PEEKTEXT, global_stat->pid, addr_bp, NULL);
    if (ret < 0)
        printf("ERROR peektext\n");
    else
        printf("%x\n", ret);
    printf("Added breakpoint at Address: 0x%lx\n", addr);
}

static need_break(debugger_status_t global_stat, ulli rip_val)
{
    ulli elem;
    list_each(global_stat.breakpoint_list, elem)
    {
        if (elem == rip_val)
            return 1;
    }
    return 0;
}

void continue_handler(char *input, debugger_status_t *global_stat)
{
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
