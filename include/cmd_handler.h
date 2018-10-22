#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H
#include "define.h"
#include <stdio.h>
#include <sys/user.h>
#include <sys/ptrace.h>

void bp_handler(char*, debugger_status_t*);
void print_reg_handler(char*, debugger_status_t*);
void continue_handler(char*, debugger_status_t*);
void next_handler(char*, debugger_status_t*);
void help_handler(char *input, debugger_status_t *global_stat);

#endif
