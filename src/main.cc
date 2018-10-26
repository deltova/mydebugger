#define _GNU_SOURCE 1
#include <string>
#include <iostream>
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
#include "cli-parser.h"
#include "define.h"
#include "memory_mapping.h"

static void init(int pid)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
}

static int exec_and_trace(int argc, char **argv)
{
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

    if (argc == 2)
        return execvp(argv[1], nullptr);
    else
        return execvp(argv[1], argv + 2);
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
        if (exec_and_trace(argc, argv) == -1)
        {
            perror("Could not exec the program");
            return 1;
        }
    }
    else
    {
        mem_mapping_t info = {.beg_addr = 0, .end_addr = 0};
        int status;
        if (waitpid(pid, &status, 0))
        {
            if (WIFEXITED(status))
                return 1;

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
            std::string input;
            std::cout << "mygdb> ";
            std::getline(std::cin, input);
            parse_and_update(&global_status, input);
        }
    }
    return 0;
}
