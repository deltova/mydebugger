#pragma once
#include <stddef.h>

typedef struct {
    unsigned long beg_addr;
    unsigned long end_addr;
} mem_mapping_t;

mem_mapping_t dump_mem(int, char*);
