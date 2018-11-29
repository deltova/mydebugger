#include "file_cacher.hh"
#include <fstream>

std::string FileCacher::get_line(const std::string& file_path,
                                 const size_t line)
{
    auto element = _container.find(file_path);
    if (element == _container.end())
    {
        load_file(file_path);
        element = _container.find(file_path);
    }
    return element->second[line];
}

void FileCacher::load_file(const std::string& file_path)
{
    std::ifstream file;
    file.open(file_path);
    _container[file_path] = {};
    std::string line;
    for (; std::getline(file, line);)
        _container[file_path].push_back(line);
}
