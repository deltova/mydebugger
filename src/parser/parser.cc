#include "parser.h"
#include <iostream>


void Parser::input_loop(void)
{
    for (;;)
    {
        std::string input;
        std::cout << "mygdb> ";
        std::getline(std::cin, input);
        this->parse_input(input);
    }
}

void Parser::parse_input(const std::string &input)
{
    auto beg = input.begin();
    auto handler_it = input_handlers.find(std::string(beg, beg + 1));
    if (handler_it == input_handlers.end())
        _debugger->default_handler(input);
    else
    {
        auto handler = handler_it->second;
        ((*_debugger).*handler)(input);
    }
}
