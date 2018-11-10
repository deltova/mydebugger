#pragma once
#include <vector>
#include <iostream>
#include <cstdint>
#include <optional>
#include <utility>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <capstone/capstone.h>
#include "debugger.h"
#include "libelfin/elf/elf++.hh"
#include "libelfin/dwarf/dwarf++.hh"

class DebuggerDwarf: public Debugger
{
public:
    DebuggerDwarf(int pid, std::string program_name);

    std::optional<std::pair<std::string, size_t>>
    find_source_file(const std::string &symbol);

protected:
    std::pair<std::string, size_t>
    get_file_name_and_line(const dwarf::die &die);
    //status
    dwarf::dwarf _dw;
    std::vector<std::string> _source_files;
    
};
