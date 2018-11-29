#include "info_elf.h"

unsigned long addr_from_name(const char *filename,  const std::string& symname)
{
    unsigned long symaddress = 0;

    bfd_init();

    bfd *ibfd = bfd_openr(filename, NULL);
    bfd_check_format(ibfd, bfd_object);

    long size = bfd_get_symtab_upper_bound(ibfd);
    asymbol **symtab = reinterpret_cast<asymbol**>(malloc(size));
    long syms = bfd_canonicalize_symtab(ibfd, symtab);

    for(auto i = 0; i < syms; i++)
    {
        if (symtab[i]->name == symname)
        {
            symbol_info info;
            bfd_symbol_info(symtab[i], &info);
            symaddress = info.value;
        }
    }

    free(symtab);
    bfd_close(ibfd);
    return symaddress;
}

bool contains_debug_info(const char* filename)
{
    bfd_init();

    bfd *ibfd = bfd_openr(filename, NULL);
    bfd_check_format(ibfd, bfd_object);
    return bfd_get_section_by_name(ibfd, ".debug_info") != NULL;
}
