#pragma once

#include <stdio.h>
#include <stddef.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "cmd_handler.h"

static void default_handler(std::string input, debugger_status_t *global_stat)
{
    std::cerr << "ERRROR: this cmd " << input << "not handle\n";
    (void)global_stat;
}

static std::map<std::string, handler> input_handlers{
    {"b", bp_handler},
    {"c", continue_handler},
    {"h", help_handler},
    {"s", step_handler},
};

void change_mode(debugger_status_t*, status_e);
void add_breakpoint(debugger_status_t*, unsigned long);
void parse_and_update(debugger_status_t*, std::string);
void print_register(debugger_status_t *, char *);
