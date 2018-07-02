#define _GNU_SOURCE 1
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <syscall.h>
#include <stdint.h>
#include "info_elf.h"
#include "define.h"
#include "memory_mapping.h"

/*typedef unsigned long addr_type;

static void do_shit(int pid, unsigned long beginning, unsigned long sym)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    addr_type diff = regs.rip ;
    printf("toto: %llx, %lx\n", regs.rip, beginning + sym);
    if (diff == beginning + sym)
    {
        printf("tamere\n");
        exit(0);
    }
    printf("pid: %d\n", pid);
    do
    {
        scanf("%c", &t);
    } while (t != 'n');
    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
}*/

static void init(int pid)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
}

static int exec_and_trace(int argc, char **argv)
{
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    void *str = NULL;
    if (argc == 2)
        execvp(argv[1], str);
    else
        execvp(argv[1], argv + 2);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "too few args");
        return 1;
    }
    int pid;
    if (!(pid = fork()))
    {
        exec_and_trace(argc, argv);
    }
    else
    {
        mem_mapping_t info = {.beg_addr = 0, .end_addr = 0};
        int status;
        if (waitpid(pid, &status, 0))
        {
            init(pid);
            info = dump_mem(pid, argv[1]);
        }
        debugger_status_t global_status = {
            .status = INPUT_WAIT,
            .pid = pid,
            .mapping = info,
            .program_name = argv[1]
        };
        for (;;)
        {
            char *line = readline("mygdb>");
            parse_and_update(&global_status, line);
        }
    }
    return 0;
}
