#include "cli-parser.h"

void parse_and_update(debugger_status_t *global_stat, std::string input)
{
    auto beg = input.begin();
    auto handler_it = input_handlers.find(std::string(beg, beg + 1));
    if (handler_it == input_handlers.end())
        default_handler(input, global_stat);
    else
    {
        auto handler = handler_it->second;
        handler(input, global_stat);
    }
}
