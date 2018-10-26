#include "cmd_handler.h"
#include "register.h"
#include "info_elf.h"
#include <sys/wait.h>
#include <string>
#include <iostream>

#define MASK_INT3 0x000000cc
#define MASK_OLD 0xFFFFFFFFFFFFFFFF00

static void print_byte_code(std::vector<char> vect)
{
    for (const auto& it: vect)
        std::cout << std::hex << ((uint16_t)it & 0xFF) << " ";
    std::cout << std::endl;
}

static uintptr_t resolve_addr(std::string value, debugger_status_t *global_stat)
{
    if (value[0] == '0' && value[1] == 'x')
    {
        return (uintptr_t)strtol(value.c_str(), NULL, 0);
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
    printf("\tp 0xaddr: print the content at $addr\n");
}


void print_handler(std::string input, debugger_status_t *global_stat)
{
    auto command  = std::string(input.begin() + 2, input.end());
    if (command.size() > 2 && command[0] == '0' && command[1] == 'x')
    {
        uintptr_t addr = std::stoul(command, NULL, 16);
        auto vect = get_memory<10>(addr, global_stat);
        print_byte_code(vect);
    }
    else
    {
        std::cout << "0x";
        std::cout << std::hex << get_specific_register(command, global_stat)
            << std::endl;
    }
}

void next_handler(std::string input, debugger_status_t *global_stat)
{
    (void)input;
    if (ptrace(PTRACE_SINGLESTEP, global_stat->pid, NULL, NULL) < 0)
        perror("ERROR singlestepping");
    int status;
    waitpid(global_stat->pid, &status, 0);
}

void bp_handler(std::string input, debugger_status_t *global_stat)
{
    auto addr = resolve_addr(
                    std::string(input.begin() + 2, input.end()), global_stat);
    void* addr_bp = (int*)addr;
    long ret;
    long int3 = 0xcc;
    long oldbyte;
    oldbyte = ptrace(PTRACE_PEEKTEXT, global_stat->pid, addr_bp, NULL);
    if (oldbyte == -1)
        perror("ERROR peektext");

    std::cout << std::hex << addr << std::endl;
    int3 |= oldbyte & MASK_OLD;

    ret = ptrace(PTRACE_POKETEXT, global_stat->pid, addr_bp, (void*)int3);
    if (ret == -1)
        perror("ERROR poketext");
    breakpoint_t bp = {addr, oldbyte};
    global_stat->breakpoint_list.push_back(bp);
    long debug;
    debug = ptrace(PTRACE_PEEKTEXT, global_stat->pid, addr_bp, NULL);
    if (debug == -1)
        perror("ERROR poketext");
}


void step_handler(std::string input, debugger_status_t *global_stat)
{
    (void)input;
    int status;
    long ret = ptrace(PTRACE_SINGLESTEP, global_stat->pid, NULL, NULL);
    if (ret == -1)
        printf("ERROR SINGLESTEP\n");
    waitpid(global_stat->pid, &status, 0);
    print_rip(global_stat);
    if (!WEXITSTATUS(status))
        printf("Programm stopped\n");
}

void continue_handler(std::string input, debugger_status_t *global_stat)
{
    (void)input;
    int status;
    global_stat->status = CONTINUE;
    long ret = ptrace(PTRACE_CONT, global_stat->pid, NULL, NULL);
    if (ret == -1)
        perror("ERROR PTRACE_CONT\n");
    waitpid(global_stat->pid, &status, 0);
    print_rip(global_stat);
    auto rip_val = get_specific_register("rip", global_stat);
    breakpoint_t current_bp = {0, 0};
    for (const auto& bp: global_stat->breakpoint_list)
    {
        if (bp.addr + 1 == rip_val)
        {
            current_bp = bp;
        }
    }
    if (current_bp.addr == 0)
    {
        std::cout << "did not stop on a breakpoint" << std::endl;
        return;
    }

    //reset the instruction that was their before the int3
    ret = ptrace(PTRACE_POKETEXT, global_stat->pid,
                 current_bp.addr, (void*)current_bp.old_byte);
    if (ret == -1)
        perror("ERROR POKETEXT\n");

    //reset the rip above this instruction
    set_specific_register("rip", global_stat, rip_val - 1);

    //execute this instruction
    step_handler(input, global_stat);

    // put the int3 back
    long int3 = MASK_INT3;
    int3 |= current_bp.old_byte & MASK_OLD;
    ret = ptrace(PTRACE_POKETEXT, global_stat->pid, current_bp.addr, (void*)int3);
    if (ret == -1)
        perror("ERROR poketext");

    if (!WEXITSTATUS(status))
        printf("Programm stopped\n");
}
