#pragma once
#include "debugger.h"
#include <map>
#include <memory>

template <typename T>
class Parser
{
  public:
    Parser(T debugger)
    {
        _debugger = std::make_shared<T>(debugger);
    }
    void input_loop(void);

  private:
    std::shared_ptr<T> _debugger;
};

#include "parser.hxx"
