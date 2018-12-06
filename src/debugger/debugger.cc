#include "debugger.h"
#include "info_elf.h"
#include "register.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

constexpr long long MASK_INT3 = 0x000000cc;
constexpr long long MASK_OLD = 0xFFFFFFFFFFFFFF00;

static void print_byte_code(const std::vector<char>& vect)
{
    for (const auto& it : vect)
        std::cout << std::hex << ((uint16_t)it & 0xFF) << ' ' << std::dec;
    std::cout << '\n';
}

static std::vector<std::string> tokenize(std::string str)
{
    std::stringstream strstr(str);
    std::istream_iterator<std::string> it(strstr);
    std::istream_iterator<std::string> end;
    std::vector<std::string> results(it, end);
    return results;
}

uintptr_t Debugger::get_static_pc()
{
    return get_specific_register("rip", _pid) - this->_begin_addr;
}
void Debugger::call_correct(const std::string& input)
{
    auto beg = input.begin();
    auto handler_it = _input_handlers.find(std::string(beg, beg + 1));
    if (handler_it == _input_handlers.end())
        default_handler(input);
    else
    {
        auto handler = handler_it->second;
        ((*this).*handler)(input);
    }
}

uintptr_t Debugger::resolve_addr(std::string value)
{
    if (value[0] == '0' && value[1] == 'x')
        return (uintptr_t)strtol(value.c_str(), NULL, 0);
    else
        // add offset to the begining  of the programm in memory
        return this->_begin_addr +
               addr_from_name(_program_name.c_str(), value.c_str());
}

void Debugger::help_handler(std::string input [[maybe_unused]])
{
    std::cout << "Available command:\n"
              << "\tb $addr: set a breakpoint at $addr\n"
              << "\tc: Continue to the next breakpoint\n"
              << "\td: Disas from rip value\n"
              << "\td $0xaddr: Disas from 0xaddr\n"
              << "\th: print the helper of commands\n"
              << "\tn: go to the next line of code\n"
              << "\ts: Go to next instruction\n"
              << "\tp $register: print the value of the $register\n"
              << "\tp 0xaddr: print the content at $addr\n"
              << "\tl [n]: get [n]  line of the current source"
              << " code\n";
}

void Debugger::disas_handler(std::string input)
{
    auto tokens = tokenize(input);
    uintptr_t addr_disas = 0;
    if (tokens.size() >= 2)
    {
        auto is_hexa = std::string(tokens[1].begin(), tokens[1].begin() + 2);
        if (is_hexa == std::string("0x"))
            addr_disas = strtol(tokens[1].c_str(), NULL, 0);
    }
    if (addr_disas == 0)
        addr_disas = get_specific_register("rip", _pid);
    auto data = get_memory<20>(addr_disas, _pid);
    cs_insn* insn;
    auto raw_data = reinterpret_cast<const unsigned char*>(data.data());
    auto count = cs_disasm(capstone_handle, raw_data, sizeof(raw_data) - 1,
                           0x1000, 0, &insn);
    for (std::size_t i = 0; i < count; ++i)
        std::cout << insn[i].mnemonic << ' ' << insn[i].op_str << '\n';
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

    std::cout << std::hex << addr << std::dec << '\n';
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

void Debugger::continue_handler(std::string input [[maybe_unused]])
{
    int status;
    long ret = ptrace(PTRACE_CONT, _pid, NULL, NULL);
    if (ret == -1)
        perror("ERROR PTRACE_CONT\n");
    waitpid(_pid, &status, 0);
    auto rip_val = get_specific_register("rip", _pid);
    breakpoint_t current_bp = {0, 0};
    for (const auto& bp : _breakpoints)
    {
        if (bp.addr + 1 == rip_val)
            current_bp = bp;
    }
    if (current_bp.addr == 0)
    {
        std::cout << "did not stop on a breakpoint\n";
        return;
    }

    // reset the instruction that was their before the int3
    ret = ptrace(PTRACE_POKETEXT, _pid, current_bp.addr,
                 (void*)current_bp.old_byte);
    if (ret == -1)
        perror("ERROR POKETEXT\n");

    // reset the rip above this instruction
    set_specific_register("rip", _pid, rip_val - 1);

    // execute this instruction
    this->step_handler(input);

    // put the int3 back
    long int3 = MASK_INT3;
    int3 |= current_bp.old_byte & MASK_OLD;
    ret = ptrace(PTRACE_POKETEXT, _pid, current_bp.addr, (void*)int3);
    if (ret == -1)
        perror("ERROR poketext");

    if (!WEXITSTATUS(status))
        std::cerr << "Programm stopped\n";
}

void Debugger::print_handler(std::string input)
{
    if (input.size() < 2)
    {
        std::cout << "print takes an arguement\n";
    }
    auto command = std::string(input.begin() + 2, input.end());
    if (command.size() > 2 && command[0] == '0' && command[1] == 'x')
    {
        uintptr_t addr = std::stoul(command, NULL, 16);
        auto vect = get_memory<10>(addr, _pid);
        print_byte_code(vect);
    }
    else
        std::cout << "0x" << std::hex << get_specific_register(command, _pid)
                  << std::dec << '\n';
}

void Debugger::stepper(void)
{
    int status;
    long ret = ptrace(PTRACE_SINGLESTEP, _pid, NULL, NULL);
    if (ret == -1)
        std::cerr << "ERROR SINGLESTEP\n";
    waitpid(_pid, &status, 0);
    if (!WEXITSTATUS(status))
        std::cerr << "Programm stopped\n";
}

void Debugger::step_handler(std::string input [[maybe_unused]])
{
    stepper();
    print_register(_pid, std::string("rip"));
}
