#include "debugger-dwarf.h"
#include <iomanip>

static int get_number_line(const std::string& input)
{
    if (input.size() <= 2)
        return 1;
    return strtol(std::string(input.begin() + 2, input.end()).c_str(), NULL, 0);
}

void DebuggerDwarf::get_current_code(std::string input)
{
    const auto number_line = get_number_line(input);
    auto current_pc = get_specific_register("rip", _pid) - this->_begin_addr;
    auto res = source_from_pc(current_pc);
    if (res != std::nullopt && res->second != 0)
    {
        std::cout << res->first << "\n";
        for (auto i = 0; i < number_line; ++i)
        {
            auto line = _cache.get_line(res->first, res->second + i - 1);
            if (line == std::nullopt)
                break;
            std::cout << res->second + i - 1 << ": ";
            std::cout << *line << '\n';
        }
    }
    else
        std::cout << "Their is no current source for the current code\n";
}

DebuggerDwarf::DebuggerDwarf(int pid, std::string program_name)
  : Debugger(pid, program_name)
{
    int fd = open(program_name.c_str(), O_RDONLY);
    elf::elf ef(elf::create_mmap_loader(fd));
    _dw = dwarf::dwarf(dwarf::elf::create_loader(ef));
    for (const auto& cu : _dw.compilation_units())
    {
        std::string filename;
        std::string path;
        uintptr_t high_pc;
        uintptr_t low_pc;
        for (const auto& attr : cu.root().attributes())
        {
            if (to_string(attr.first) == "DW_AT_name")
                filename = to_string(attr.second);
            else if (to_string(attr.first) == "DW_AT_comp_dir")
                path = to_string(attr.second);
            else if (to_string(attr.first) == "DW_AT_low_pc")
                low_pc = strtol(to_string(attr.second).c_str(), NULL, 0);
            else if (to_string(attr.first) == "DW_AT_high_pc")
                high_pc = strtol(to_string(attr.second).c_str(), NULL, 0);
        }
        if (filename[0] != '/')
            filename = path + '/' + filename;
        _source_files.push_back(CompileUnit(low_pc, high_pc, filename));
        _input_handlers_dwarf["l"] = &DebuggerDwarf::get_current_code;
        _input_handlers_dwarf["n"] = &DebuggerDwarf::next_handler;
    }
}

void DebuggerDwarf::call_correct(const std::string& input)
{
    auto beg = input.begin();
    auto handler_it = _input_handlers_dwarf.find(std::string(beg, beg + 1));
    if (handler_it != _input_handlers_dwarf.end())
    {
        auto handler = handler_it->second;
        ((*this).*handler)(input);
    }
    else
        Debugger::call_correct(input);
}

std::pair<std::string, size_t>
DebuggerDwarf::get_file_name_and_line(const dwarf::die& die)
{
    size_t compile_unit;
    size_t line;
    for (const auto& attr : die.attributes())
    {
        const auto attribute = to_string(attr.first);
        if (attribute == "DW_AT_decl_file")
            compile_unit = 1 - strtol(to_string(attr.second).c_str(), NULL, 0);

        else if (attribute == "DW_AT_decl_line")
            line = strtol(to_string(attr.second).c_str(), NULL, 0);
    }
    return std::make_pair(_source_files[compile_unit].filename, line);
}

template <class T, class L>
static std::optional<dwarf::die>
search_tree(const dwarf::die& node, const T& symbol, int depth, L lambda)
{
    std::optional<dwarf::die> save = {};
    for (const auto& child : node)
    {
        const auto res = search_tree(child, symbol, depth + 1, lambda);
        if (res != std::nullopt)
            save = res;
    }
    if (save == std::nullopt)
    {
        if (lambda(node, symbol))
            return node;
        else
            return {};
    }
    return save;
}

std::optional<std::pair<std::string, size_t>>
DebuggerDwarf::find_line(const uintptr_t pc)
{
    for (const auto& cu : _dw.compilation_units())
    {
        const auto& lines = cu.get_line_table();
        auto prev = lines.begin();
        for (auto line = lines.begin(); line != lines.end(); line++)
        {
            if (pc == line->address)
                return std::make_pair(line->file->path, line->line);
            else if (pc > prev->address && pc < line->address)
                return std::make_pair(prev->file->path, prev->line);
            prev = line;
        }
    }
    return {};
}

std::optional<std::pair<std::string, size_t>>
DebuggerDwarf::source_from_pc(const uintptr_t& pc_val)
{
    return find_line(pc_val);
}

void DebuggerDwarf::next_handler([[maybe_unused]] std::string input)
{
    auto file_position = find_line(get_static_pc());
    if (file_position == std::nullopt)
    {
        std::cout << "no debug info in this position\n";
        return;
    }
    decltype(file_position) new_pos;
    do
    {
        this->stepper();
        new_pos = find_line(get_static_pc());
    } while (new_pos == std::nullopt ||
             file_position->first != new_pos->first ||
             file_position->second == new_pos->second);
}
