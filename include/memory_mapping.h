#pragma once
#include <stddef.h>
#include <string>

typedef struct {
    unsigned long beg_addr;
    unsigned long end_addr;
} mem_mapping_t;

mem_mapping_t dump_mem(int, std::string);
