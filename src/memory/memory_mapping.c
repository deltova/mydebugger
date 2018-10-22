#define _GNU_SOURCE 1
#include "memory_mapping.h"
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

static mem_mapping_t fill(char *line, char *exec_name)
{
    mem_mapping_t res = {.beg_addr = 0, .end_addr = 0};
    unsigned long from, to;
    unsigned int pgoff, major, minor;
    unsigned long ino;
    char flags[4];
    sscanf(line, "%lx-%lx %4c %x %x:%x %lu ", &from, &to, flags, &pgoff, &major,
            &minor, &ino);
    while (line && *line != '/')
        line++;
    if (!strcmp(line, exec_name))
    {
        res.beg_addr = from;
        res.end_addr = to;
    }
    else
    {
        res.beg_addr = 0;
        res.end_addr = 0;
    }
    return res;
}

static mem_mapping_t parse_and_fill(FILE *file, char *exec_name)
{
    char **line = malloc(sizeof(char*));
    *line = NULL;
    size_t *size = malloc(sizeof(size_t));
    *size = 0;
    mem_mapping_t res;
    do {
        int len = getline(line, size, file);
        (*line)[len - 1] = 0;
        printf("%s\n", *line);
        res = fill(*line, exec_name);
        if (res.beg_addr == res.end_addr && res.beg_addr == 0)
            break;
    }
    while (!res.beg_addr);
    free(size);
    free(line);
    return res;
}

static char *inject_full_path(char *exec)
{
    char *dir_name = get_current_dir_name();
    char *res = malloc(strlen(dir_name) + strlen(exec) + 2);
    sprintf(res,"%s/%s", dir_name, exec);
    return res;

}

mem_mapping_t dump_mem(int pid, char *executable_name)
{
    executable_name = inject_full_path(executable_name);
    char *value = malloc(20);
    sprintf(value, "/proc/%d/maps", pid);
    FILE *file = fopen(value, "r");
    mem_mapping_t res = parse_and_fill(file, executable_name);
    fclose(file);
    return res;
}
