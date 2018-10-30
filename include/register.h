#pragma once
#include "define.h"
#include <sys/ptrace.h>
#include <sys/user.h>

uintptr_t get_specific_register(const std::string& reg_name, int pid);

void set_specific_register(const std::string& reg_name, int pid, uintptr_t val);

void print_register(int pid, const std::string& reg_name);
