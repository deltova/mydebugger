#include "info_elf.h"

void *open_binary(char *binary_name)
{
    return dlopen(binary_name, RTLD_LAZY|RTLD_GLOBAL);
}

unsigned long addr_from_name(char * filename, const char * symname)
{
    bfd *        ibfd;
    asymbol **   symtab;
    symbol_info  info;
    unsigned long symaddress = 0;
    long         size;
    long         syms;
    unsigned int i;

    bfd_init();

    ibfd = bfd_openr(filename, NULL);
    bfd_check_format(ibfd, bfd_object);

    size   = bfd_get_symtab_upper_bound(ibfd);
    symtab = reinterpret_cast<asymbol**>(malloc(size));
    syms   = bfd_canonicalize_symtab(ibfd, symtab);

    for(i = 0; i < syms; i++) {
        if(strcmp(symtab[i]->name, symname) == 0) {
            bfd_symbol_info(symtab[i], &info);
            symaddress = info.value;
        }
    }

    bfd_close(ibfd);
    return symaddress;
}
