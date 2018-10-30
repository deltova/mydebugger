#include "debugger.h"
#include "register.h"
#include "info_elf.h"
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <iostream>

constexpr long long MASK_INT3 = 0x000000cc;
constexpr long long MASK_OLD = 0xFFFFFFFFFFFFFFFF00;

static void print_byte_code(std::vector<char> vect)
{
    for (const auto& it: vect)
        std::cout << std::hex << ((uint16_t)it & 0xFF) << " ";
    std::cout << std::endl;
}

uintptr_t Debugger::resolve_addr(std::string value)
{
    if (value[0] == '0' && value[1] == 'x')
    {
        return (uintptr_t)strtol(value.c_str(), NULL, 0);
    }
    else
    {
        //add offset to the begining  of the programm in memory
        return _mapping.beg_addr
            + addr_from_name(_program_name.c_str(), value.c_str());
    }

}

void Debugger::help_handler(std::string input)
{
    (void)input;
    std::cout << "Available command:" << std::endl;
    std::cout << "\tb $addr: set a breakpoint at $addr" << std::endl;
    std::cout << "\tc: Continue to the next breakpoint" << std::endl;
    std::cout << "\th: print the helper of commands" << std::endl;
    std::cout << "\ts: Go to next instruction" << std::endl;
    std::cout << "\tp $register: print the value of the $register" << std::endl;
    std::cout << "\tp 0xaddr: print the content at $addr" << std::endl;
}

void Debugger::default_handler(std::string input)
{
    std::cerr << "ERRROR: this cmd " << input << "not handle\n";
}

void Debugger::bp_handler(std::string input)
{
    auto addr = resolve_addr(std::string(input.begin() + 2, input.end()));
    void* addr_bp = (int*)addr;
    long ret;
    long int3 = 0xcc;
    long oldbyte;
    oldbyte = ptrace(PTRACE_PEEKTEXT, _pid, addr_bp, NULL);
    if (oldbyte == -1)
        perror("ERROR peektext");

    std::cout << std::hex << addr << std::endl;
    int3 |= oldbyte & MASK_OLD;

    ret = ptrace(PTRACE_POKETEXT, _pid, addr_bp, (void*)int3);
    if (ret == -1)
        perror("ERROR poketext");
    breakpoint_t bp = {addr, oldbyte};
    _breakpoints.push_back(bp);
    long debug;
    debug = ptrace(PTRACE_PEEKTEXT, _pid, addr_bp, NULL);
    if (debug == -1)
        perror("ERROR poketext");
}

void Debugger::continue_handler(std::string input)
{
    (void)input;
    int status;
    long ret = ptrace(PTRACE_CONT, _pid, NULL, NULL);
    if (ret == -1)
        perror("ERROR PTRACE_CONT\n");
    waitpid(_pid, &status, 0);
    print_register(_pid, "rip");
    auto rip_val = get_specific_register("rip", _pid);
    breakpoint_t current_bp = {0, 0};
    for (const auto& bp: _breakpoints)
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
    ret = ptrace(PTRACE_POKETEXT, _pid,
                 current_bp.addr, (void*)current_bp.old_byte);
    if (ret == -1)
        perror("ERROR POKETEXT\n");

    //reset the rip above this instruction
    set_specific_register("rip", _pid, rip_val - 1);

    //execute this instruction
    this->step_handler(input);

    // put the int3 back
    long int3 = MASK_INT3;
    int3 |= current_bp.old_byte & MASK_OLD;
    ret = ptrace(PTRACE_POKETEXT, _pid, current_bp.addr, (void*)int3);
    if (ret == -1)
        perror("ERROR poketext");

    if (!WEXITSTATUS(status))
        printf("Programm stopped\n");
}

void Debugger::print_handler(std::string input)
{
    auto command  = std::string(input.begin() + 2, input.end());
    if (command.size() > 2 && command[0] == '0' && command[1] == 'x')
    {
        uintptr_t addr = std::stoul(command, NULL, 16);
        auto vect = get_memory<10>(addr, _pid);
        print_byte_code(vect);
    }
    else
    {
        std::cout << "0x";
        std::cout << std::hex << get_specific_register(command, _pid)
            << std::endl;
    }
}

void Debugger::step_handler(std::string input)
{
    (void)input;
    int status;
    long ret = ptrace(PTRACE_SINGLESTEP, _pid, NULL, NULL);
    if (ret == -1)
        printf("ERROR SINGLESTEP\n");
    waitpid(_pid, &status, 0);
    print_register(_pid, std::string("rip"));
    if (!WEXITSTATUS(status))
        printf("Programm stopped\n");
}
