#include "cli-parser.h"


void change_mode(debugger_status_t *global_stat, status_e new_status)
{
    global_stat->status = new_status;
}

void parse_and_update(debugger_status_t *global_stat, std::string input)
{
    int cmd = input[0] - 'a';
    input_handlers[cmd](input, global_stat);
}
