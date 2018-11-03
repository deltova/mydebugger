#pragma once
#include <memory>
#include <map>
#include "debugger.h"

class Parser
{
public:
    Parser(Debugger debugger)
    {
        _debugger = std::make_shared<Debugger>(debugger);
    }
    void input_loop(void);
    void parse_input(const std::string &input);
private:
    std::shared_ptr<Debugger> _debugger;
};

typedef void (Debugger::*func_ptr)(std::string);

static std::map<std::string, func_ptr> input_handlers{
    {"b", &Debugger::bp_handler},
    {"d", &Debugger::disas_handler},
    {"c", &Debugger::continue_handler},
    {"h", &Debugger::help_handler},
    {"s", &Debugger::step_handler},
    {"p", &Debugger::print_handler}
};
