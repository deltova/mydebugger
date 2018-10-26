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

static void print_byte_code(std::vector<char> vect)
{
    for (const auto& it: vect)
        std::cout << std::hex << ((uint16_t)it & 0xFF) << " ";
    std::cout << std::endl;
}

static struct user_regs_struct get_regs(debugger_status_t* global_stat)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, global_stat->pid, NULL, &regs);
    return regs;
}

static uintptr_t get_specific_register(std::string reg_name,
                                debugger_status_t *global_stat)
{
    struct user_regs_struct registers = get_regs(global_stat);
    int i = 0;
    while(regs[i] != reg_name && i < 26)
        ++i;
    return *(uintptr_t*)(((char*)&registers + sizeof(ulli) * i));
}

static void print_rip(debugger_status_t* global_stat)
{
    auto rip_val = get_specific_register("rip\0", global_stat);
    std::cout << "rip after handling bp" << std::hex << rip_val << std::endl;
}

static void set_specific_register(std::string reg_name,
                                debugger_status_t *global_stat, uintptr_t val)
{
    struct user_regs_struct registers = get_regs(global_stat);
    int i = 0;
    while(regs[i] != reg_name && i < 26)
        ++i;
    *(uintptr_t*)((char*)&registers + sizeof(ulli) * i) = val;
    if (ptrace(PTRACE_SETREGS, global_stat->pid, NULL, &registers) < 0)
        std::cerr << "error seting reg " << reg_name << std::endl;
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
    auto addr = resolve_addr(
                    std::string(input.begin() + 2, input.end()), global_stat);
    void* addr_bp = (int*)addr;
    long ret;
    long int3 = 0xcc;
    long oldbyte;
    oldbyte = ptrace(PTRACE_PEEKTEXT, global_stat->pid, addr_bp, NULL);
    if (oldbyte < 0)
        std::cerr << "ERROR peektext" << std::endl;

    printf("%x\n", addr);
    int3 |= oldbyte & 0xFFFFFFFF00;
    ret = ptrace(PTRACE_POKETEXT, global_stat->pid, addr_bp, (void*)int3);
    if (ret < 0)
        std::cerr << "ERROR poketext" << std::endl;
    breakpoint_t bp = {addr, oldbyte};
    global_stat->breakpoint_list.push_back(bp);
    long debug;
    debug = ptrace(PTRACE_PEEKTEXT, global_stat->pid, addr_bp, NULL);
    if (debug < 0)
        std::cout << "ERROR poketext" << std::endl;
}


void step_handler(std::string input, debugger_status_t *global_stat)
{
    (void)input;
    int status;
    long ret = ptrace(PTRACE_SINGLESTEP, global_stat->pid, NULL, NULL);
    if (ret < 0)
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
    if (ret < 0)
        printf("ERROR PTRACE_CONT\n");

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
    if (ret < 0)
        printf("ERROR POKETEXT\n");

    //reset the rip above this instruction
    set_specific_register("rip", global_stat, rip_val - 1);

    //execute this instruction
    ret = ptrace(PTRACE_SINGLESTEP, global_stat->pid, NULL, NULL);
    if (ret < 0)
        printf("ERROR SINGLESTEP\n");
    waitpid(global_stat->pid, &status, 0);

    // put the int3 back
    long int3 = 0x000000cc;
    int3 |= current_bp.old_byte & 0xFFFFFFFF00;
    ret = ptrace(PTRACE_POKETEXT, global_stat->pid, current_bp.addr, (void*)int3);
    if (ret < 0)
        perror("ERROR poketext");


    if (!WEXITSTATUS(status))
        printf("Programm stopped\n");
}
