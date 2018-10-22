#ifndef CLI_PARSER_H
#define CLI_PARSER_H
#include <stdio.h>
#include <stddef.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include "cmd_handler.h"

void default_handler(char *input, debugger_status_t *global_stat)
{
    fprintf(stderr, "ERRROR: this cmd \"%s\"not handle\n", input);
    global_stat++;
    global_stat--;
}

handler input_handlers[] =
{
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
    default_handler,
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
void parse_and_update(debugger_status_t*, char *);
void print_register(debugger_status_t *, char *);
#endif
