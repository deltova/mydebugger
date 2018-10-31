#pragma once
#include <cstdint>
#include <string>
#include <sys/ptrace.h>
#include <sys/user.h>

typedef unsigned long long int ulli;

uintptr_t get_specific_register(const std::string& reg_name, int pid);

void set_specific_register(const std::string& reg_name, int pid, uintptr_t val);

void print_register(int pid, const std::string& reg_name);
