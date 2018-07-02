#ifndef DEFINE_H
#define DEFINE_H
#include "memory_mapping.h"
#include "list.h"

typedef enum {
    CONTINUE,
    INPUT_WAIT,
}   status_e;

typedef struct {
    list(unsigned long, breakpoint_list);
    status_e status;
    int pid;
    mem_mapping_t mapping;
    char *program_name;
}   debugger_status_t;

typedef void (*handler)(char *, debugger_status_t*);
#endif
