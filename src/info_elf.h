#ifndef INFO_ELF_H
#define INFO_ELF_H
#define PACKAGE 1
#define PACKAGE_VERSION 1
#include "bfd.h"
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
void *open_binary(char *binary_name);
unsigned long addr_from_name(char * filename, char * symname);
#endif
