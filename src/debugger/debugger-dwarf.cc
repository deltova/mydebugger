#include "debugger-dwarf.h"

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
        for (const auto &attr: cu.root().attributes())
        {
            if (to_string(attr.first) == "DW_AT_name")
                filename = to_string(attr.second);
            else if (to_string(attr.first) == "DW_AT_comp_dir")
                path = to_string(attr.second);
        }
        _source_files.push_back(path + "/" + filename);
    }
}

std::pair<std::string, size_t>
DebuggerDwarf::get_file_name_and_line(const dwarf::die &die)
{
    size_t compile_unit;
    size_t line;
    for (auto &attr: die.attributes())
    {
        if (auto attribute = to_string(attr.first);
                attribute == "DW_AT_decl_file")
        {
            compile_unit = 1 - strtol(to_string(attr.second).c_str(), NULL, 0);
        }
        
        if (auto attribute = to_string(attr.first);
                attribute == "DW_AT_decl_line")
            line = strtol(to_string(attr.second).c_str(), NULL, 0);
    }
    return std::make_pair(_source_files[compile_unit], line);

}

static std::optional<dwarf::die>
search_tree(const dwarf::die &node, const std::string &symbol, int depth = 0)
{
    for (auto &attr: node.attributes())
    {
        if (auto attribute = to_string(attr.first); attribute == "DW_AT_name")
            if (auto value = to_string(attr.second); value == symbol)
                return std::make_optional(node);
                
    }
    for (const auto &child : node)
    {
        auto res = search_tree(child, symbol, depth + 1);
        if (res != std::nullopt)
            return res;
    }
    return {};
}

std::optional<std::pair<std::string, size_t>>
DebuggerDwarf::find_source_file(const std::string &symbol)
{
    for (const auto& cu : _dw.compilation_units())
    {
        auto res = search_tree(cu.root(), symbol);
        if (res != std::nullopt)
            return std::make_optional(get_file_name_and_line(res.value()));
    }
    return {};
}
