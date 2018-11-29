#include "parser.h"
#include <iostream>
#include <stdlib.h>

template <typename T>
inline void Parser<T>::input_loop(void)
{
    for (;;)
    {
        std::string input;
        std::cout << "mygdb> ";
        if (!std::getline(std::cin, input))
        {
            std::cout << std::endl;
            exit(0);
        }
        this->_debugger->call_correct(input);
    }
}
