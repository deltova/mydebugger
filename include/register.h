#pragma once
#include "define.h"
#include <sys/ptrace.h>
#include <sys/user.h>

uintptr_t get_specific_register(std::string reg_name,
                                debugger_status_t *global_stat);

void set_specific_register(std::string reg_name,
                                debugger_status_t *global_stat, uintptr_t val);
