#pragma once

#define PACKAGE 1
#define PACKAGE_VERSION 1

#include "bfd.h"
#include <unistd.h>
#include <dlfcn.h>
#include <string>
#include <stdlib.h>

unsigned long addr_from_name(const char * filename, const std::string& symname);
bool contains_debug_info(const char* filename);
