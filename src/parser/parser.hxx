#include "parser.h"
#include <iostream>

template <typename T>
inline void Parser<T>::input_loop(void)
{
    for (;;)
    {
        std::string input;
        std::cout << "mygdb> ";
        std::getline(std::cin, input);
        this->_debugger->call_correct(input);
    }
}
