#include "cli-parser.h"


void change_mode(debugger_status_t *global_stat, status_e new_status)
{
    global_stat->status = new_status;
}

void add_breakpoint(debugger_status_t *global_stat, unsigned long bp_addr)
{
    global_stat->breakpoint_list.push_back(bp_addr);
}


void parse_and_update(debugger_status_t *global_stat, std::string input)
{
    int cmd = input[0] - 'a';
    input_handlers[cmd](input, global_stat);
}
