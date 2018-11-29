#include "debugger-dwarf.h"

static bool contains_pc(uintptr_t low, uintptr_t high, uintptr_t pc)
{
    return low <= pc && pc <= low + high;
}

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
<<<<<<< HEAD
=======
    }
    return save;
}

std::optional<std::pair<std::string, size_t>>
DebuggerDwarf::source_from_name(const std::string& symbol)
{
    auto lambda = [](const auto& node, const auto& symbol) {
        for (const auto& attr : node.attributes())
        {
            if (auto attribute = to_string(attr.first);
                attribute == "DW_AT_name")
                if (auto value = to_string(attr.second); value == symbol)
                    return true;
        }
        return false;
    };
    for (const auto& cu : _dw.compilation_units())
    {
        auto res = search_tree(cu.root(), symbol, 0, lambda);
        if (res != std::nullopt)
            return get_file_name_and_line(res.value());
>>>>>>> cleanup: clean code and fix warnings
    }
    return save;
}

size_t DebuggerDwarf::find_line(const size_t index_cu, const uintptr_t pc)
{
    const auto& cu = _dw.compilation_units()[index_cu];
    const auto& lines = cu.get_line_table();
    auto prev = lines.begin();
    for (auto line = lines.begin(); line != lines.end(); line++)
    {
        if (pc == line->address)
            return line->line;
        else if (pc > prev->address && pc < line->address)
            return prev->line;
        prev = line;
    }
    return 0;
}

std::optional<std::pair<std::string, size_t>>
DebuggerDwarf::source_from_pc(const uintptr_t& pc_val)
{
    auto lambda = [](const auto& node, const auto& pc) {
        uintptr_t low = 0;
        uintptr_t high = 0;
        for (const auto& attr : node.attributes())
        {
            const auto attribute = to_string(attr.first);
            if (attribute == "DW_AT_low_pc")
                low = strtol(to_string(attr.second).c_str(), NULL, 0);
            else if (attribute == "DW_AT_high_pc")
                high = strtol(to_string(attr.second).c_str(), NULL, 0);
        }
        return contains_pc(low, high, pc);
    };
    const auto& cus = _dw.compilation_units();
    for (size_t i = 0; i < cus.size(); ++i)
    {
        auto low = _source_files[i].low_pc;
        auto high = _source_files[i].high_pc;
        if (!contains_pc(low, high, pc_val))
            continue;
        auto res = search_tree(cus[i].root(), pc_val, 0, lambda);

        if (res != std::nullopt)
            return std::make_pair(_source_files[i].filename,
                                  find_line(i, pc_val));
    }
    return {};
}
