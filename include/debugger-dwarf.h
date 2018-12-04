#pragma once
#include "debugger.h"
#include "file_cacher.hh"
#include "libelfin/dwarf/dwarf++.hh"
#include "libelfin/elf/elf++.hh"
#include <capstone/capstone.h>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <optional>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <utility>
#include <vector>

class DebuggerDwarf : public Debugger
{
  public:
    DebuggerDwarf(int pid, std::string program_name);

    std::optional<std::pair<std::string, size_t>>
    source_from_name(const std::string& symbol);

    std::optional<std::pair<std::string, size_t>>
    source_from_pc(const uintptr_t& pc_val);

    void get_current_code(std::string input) override;

    void call_correct(const std::string& input) override;

  protected:
    std::pair<std::string, size_t>
    get_file_name_and_line(const dwarf::die& die);
    std::optional<std::pair<std::string, size_t>> find_line(const uintptr_t pc);
    // status
    dwarf::dwarf _dw;
    struct CompileUnit
    {
        const uintptr_t low_pc;
        const uintptr_t high_pc;
        const std::string filename;
        CompileUnit(uintptr_t low, uintptr_t high, std::string str)
          : low_pc(low)
          , high_pc(high)
          , filename(str)
        {
        }
    };
    std::vector<CompileUnit> _source_files;
    std::map<std::string, decltype(&DebuggerDwarf::get_current_code)>
      _input_handlers_dwarf;
    FileCacher _cache;
};
