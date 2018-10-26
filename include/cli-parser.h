#pragma once

#include <stdio.h>
#include <stddef.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <string>
#include <iostream>
#include <vector>
#include "cmd_handler.h"

static void default_handler(std::string input, debugger_status_t *global_stat)
{
    std::cerr << "ERRROR: this cmd " << input << "not handle\n";
    (void)global_stat;
}

static std::vector<handler> input_handlers{
    default_handler,
    bp_handler,
    continue_handler,
    default_handler,
    default_handler,
    default_handler,
    default_handler,
    help_handler,
    default_handler,
    default_handler,
    default_handler,
    default_handler,
    default_handler,
    next_handler,
    default_handler,
    print_reg_handler,
    default_handler,
    default_handler,
    step_handler,
    default_handler,
    default_handler,
    default_handler,
    default_handler,
    default_handler,
    default_handler,
    default_handler
};

void change_mode(debugger_status_t*, status_e);
void add_breakpoint(debugger_status_t*, unsigned long);
void parse_and_update(debugger_status_t*, std::string);
void print_register(debugger_status_t *, char *);
