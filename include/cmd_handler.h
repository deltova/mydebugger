#pragma once
#include "define.h"
#include <stdio.h>
#include <sys/user.h>
#include <sys/ptrace.h>

void bp_handler(std::string, debugger_status_t*);
void print_reg_handler(std::string, debugger_status_t*);
void continue_handler(std::string, debugger_status_t*);
void next_handler(std::string, debugger_status_t*);
void help_handler(std::string, debugger_status_t *global_stat);
