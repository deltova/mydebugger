#pragma once
#include <string>

class MemoryMapping
{
public:
   MemoryMapping(const int& pid, const std::string& exec_name);
protected:
    uintptr_t _begin_addr;
    uintptr_t _end_addr;
};
