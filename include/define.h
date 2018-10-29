#pragma once
#include <vector>
#include <string>
#include "memory_mapping.h"

typedef unsigned long long int ulli;

typedef enum {
    CONTINUE,
    INPUT_WAIT,
}   status_e;

typedef struct {
    uintptr_t addr;
    long old_byte;
} breakpoint_t;

typedef struct {
    std::vector<breakpoint_t> breakpoint_list;
    status_e status;
    int pid;
    mem_mapping_t mapping;
    char *program_name;
}   debugger_status_t;

typedef void (*handler)(std::string, debugger_status_t*);
