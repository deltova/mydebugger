#pragma once

#define PACKAGE 1
#define PACKAGE_VERSION 1

#include "bfd.h"
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>

unsigned long addr_from_name(const char * filename, const char* symname);
