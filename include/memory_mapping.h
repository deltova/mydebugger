#pragma once
#include <stddef.h>
#include <string>

typedef struct {
    uintptr_t beg_addr;
    uintptr_t end_addr;
} mem_mapping_t;

mem_mapping_t dump_mem(int, std::string);
