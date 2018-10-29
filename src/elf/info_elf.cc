#include "info_elf.h"

unsigned long addr_from_name(char * filename, const char * symname)
{
    unsigned long symaddress = 0;

    bfd_init();

    bfd *ibfd = bfd_openr(filename, NULL);
    bfd_check_format(ibfd, bfd_object);

    long size = bfd_get_symtab_upper_bound(ibfd);
    asymbol **symtab = reinterpret_cast<asymbol**>(malloc(size));
    long syms = bfd_canonicalize_symtab(ibfd, symtab);

    for(auto i = 0; i < syms; i++) {
        if(strcmp(symtab[i]->name, symname) == 0) {
            symbol_info info;
            bfd_symbol_info(symtab[i], &info);
            symaddress = info.value;
        }
    }

    bfd_close(ibfd);
    return symaddress;
}
